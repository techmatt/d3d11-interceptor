
#include "Main.h"

const int textureOutputCount = 0;
const bool outputImages = true;

void Logger::recordSignatureColorPreDraw(MyD3DAssets &assets, DrawParameters &params)
{
    static const int frameSignatureCaptureFrequency = 200;
    capturingColorSignature = false;

    if (!assets.viewportFullScreen() || frameIndex % frameSignatureCaptureFrequency != 0)
    {
        return;
    }

    params.signature = LocalizedObject::computeSignature(assets, params);
    if (params.signature == 0)
    {
        return;
    }

    auto result = g_logger->colorMap.colors.find(params.signature);
    if (result == g_logger->colorMap.colors.end() || result->second.pixelCount <= 4)
    {
        capturingColorSignature = true;
        assets.context->readRenderTarget(preRenderImage);
    }
}

void Logger::recordSignatureColorPostDraw(MyD3DAssets &assets, const DrawParameters &params)
{
    if (!capturingColorSignature)
    {
        return;
    }

    assets.context->readRenderTarget(postRenderImage);

    vec3f diffSum = vec3f::origin;
    int diffCount = 0;
    for (auto &p : postRenderImage)
    {
        if (p.value != preRenderImage(p.x, p.y))
        {
            diffSum += vec3f(p.value.getVec3()) / 255.0f;
            diffCount++;
        }
    }

    g_logger->newSignaturesThisFrame++;
    colorMap.record(params.signature, diffCount == 0 ? vec3f::origin : diffSum / (float)diffCount, diffCount);
}

void Logger::recordDrawEvent(MyD3DAssets &assets, const DrawParameters &params)
{
    if (capturingFrame && assets.viewportFullScreen())
    {
        LocalizedObject object;

        object.load(assets, params);
        
        if (object.vertices.size() > 0)
            frameCaptureObjects.objects.push_back(object);

        assets.loadVSConstantBuffer();
        const auto &indexBuffer = assets.getActiveIndexBuffer();
        const auto &vertexBuffer = assets.getActiveVertexBuffer();
        
        const string imagePrefix = "render" + util::zeroPad(frameRenderIndex, 5);
        const string frameImageFile = imagePrefix + "_frame.png";
        const string frameDeltaImageFile = imagePrefix + "_delta.png";
        const string texImageFile = imagePrefix + "_tex";

        if (outputImages)
        {
            Bitmap image;
            assets.context->readRenderTarget(image);
            LodePNG::save(image, g_logger->captureDir + frameImageFile);

            if (prevCaptureImage.getDimensions() == image.getDimensions())
            {
                Bitmap deltaImage = image;
                for (auto &p : deltaImage)
                {
                    if (p.value == prevCaptureImage(p.x, p.y))
                        p.value = vec4uc(0, 0, 0, 255);
                }

                LodePNG::save(deltaImage, g_logger->captureDir + frameDeltaImageFile);
            }

            prevCaptureImage = image;
        }

        auto modifyImage = [](Bitmap &b)
        {
            if (b.size() == 0) return;
            for (auto &p : b)
            {
                p.value.r = unsigned char((int)p.value.r * (int)p.value.a / 255);
                p.value.g = unsigned char((int)p.value.g * (int)p.value.a / 255);
                p.value.b = unsigned char((int)p.value.b * (int)p.value.a / 255);
                p.value.a = 255;
            }
        };

        for (int textureIndex = 0; textureIndex < textureOutputCount; textureIndex++)
        {
            assets.loadPSTexture(textureIndex);
            modifyImage(assets.PSTexture);
            if (assets.PSTexture.size() > 0 && outputImages)
                LodePNG::save(assets.PSTexture, g_logger->captureDir + texImageFile + to_string(textureIndex) + ".png");
        }

        auto makeHTMLImage = [](const string &filename)
        {
            return "<img src=\"" + filename + "\" />";
        };

        logFrameCaptureHtml << "<tr>" << endl;
        logFrameCaptureHtml << "<td>" << frameRenderIndex << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(frameImageFile) << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(frameDeltaImageFile) << "</td>" << endl;

        for (int texIndex = 0; texIndex < textureOutputCount; texIndex++)
            logFrameCaptureHtml << "<td>" << makeHTMLImage(texImageFile + to_string(texIndex) + ".png") << "</td>" << endl;
        
        auto viewport = assets.getViewport();
        logFrameCaptureHtml << "<td>" << object.signature << "<br />" << "viewport: " << viewport.Width << "," << viewport.Height << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << params.IndexCount << ", " << params.StartIndexLocation << ", " << params.BaseVertexLocation << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << ((indexBuffer.buffer == nullptr) ? "invalid" : to_string(indexBuffer.buffer->data.size())) + " " + pointerToString(indexBuffer.buffer->GPUHandle) << "</td>" << endl;
        //logFrameCaptureHtml << "<td>" << indexBuffer.offset << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << ((vertexBuffer.buffer == nullptr) ? "invalid" : to_string(vertexBuffer.buffer->data.size())) + " " + pointerToString(vertexBuffer.buffer->GPUHandle) << "</td>" << endl;
        //logFrameCaptureHtml << "<td>" << vertexBuffer.offset << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << vertexBuffer.stride << "</td>" << endl;

        string v0Data = "<none>";
        if (params.BaseVertexLocation != -1 && vertexBuffer.buffer != nullptr && indexBuffer.buffer != nullptr)
        {
            const WORD *indexDataStart = (WORD *)indexBuffer.buffer->data.data() + params.StartIndexLocation;
            
            const BYTE *vertexData = vertexBuffer.buffer->data.data();

            v0Data = "";

            const auto *layout = assets.activeVertexLayout;

            if (layout == nullptr)
            {
                v0Data = "layout not found";
            }
            else
            {
                v0Data += layout->htmlDescription;
                v0Data += "data:<br />";

                for (int indexIndex = 0; indexIndex < min((int)params.IndexCount, 32); indexIndex++)
                {
                    const int curIndex = indexDataStart[indexIndex] + params.BaseVertexLocation;
                    const BYTE *vertexStart = (const BYTE *)(vertexData + (vertexBuffer.stride * curIndex));

                    if (vertexBuffer.buffer->data.size() < vertexBuffer.stride * (curIndex + 1))
                    {
                        continue;
                    }

                    if (vertexBuffer.stride / 4 >= 4)
                    {
                        const float *posStart = (const float *)(vertexStart + layout->positionOffset);
                        const vec3f basePos(posStart[1], posStart[2], posStart[3]);
                        const vec3f worldPos = assets.transformObjectToWorldGamecube(basePos, -1);
                        v0Data += "world=" + worldPos.toString(", ") + " ";
                    }

                    v0Data += "index=" + to_string(curIndex) + " ";

                    if (layout->blendOffset != -1)
                    {
                        int blendIndex = 0;
                        const vec4uc *blendStart = (const vec4uc *)(vertexStart + layout->blendOffset);
                        v0Data += "blendIndex=" + blendStart[0].toString(", ") + " ";
                    }

                    for (int i = 0; i < (int)vertexBuffer.stride / 4; i++)
                    {
                        v0Data += to_string(((const float *)vertexStart)[i]) + ", ";
                    }
                    v0Data += "<br />";
                }
            }
        }

        logFrameCaptureHtml << "<td>" << v0Data << "</td>" << endl;
        logFrameCaptureHtml << "</tr>" << endl;
    }

    frameRenderIndex++;
}

void Logger::beginFrameCapture()
{
    capturingFrame = true;
    frameCaptureObjects.objects.clear();
    logDrawFile << "Capturing frame " << frameIndex << endl;
    captureDir = logDir + "capture" + util::zeroPad(frameIndex, 6) + "/";
    util::makeDirectory(captureDir);
    logFrameCaptureFile.open(captureDir + "log.txt");

    logFrameCaptureHtml.open(captureDir + "log.html");
    logFrameCaptureHtml << "<html><body>" << endl;
    logFrameCaptureHtml << "<table border=\"1\">" << endl;
    
    logFrameCaptureHtml << "<tr>" << endl;
    logFrameCaptureHtml << "<td>Index</td>" << endl;
    logFrameCaptureHtml << "<td>Frame</td>" << endl;
    logFrameCaptureHtml << "<td>Delta Frame</td>" << endl;
    for (int texIndex = 0; texIndex < textureOutputCount; texIndex++)
        logFrameCaptureHtml << "<td>Texture " << texIndex << "</td>" << endl;
    logFrameCaptureHtml << "<td>Signature</td>" << endl;
    logFrameCaptureHtml << "<td>Draw Params</td>" << endl;
    logFrameCaptureHtml << "<td>IBuffer Size</td>" << endl;
    //logFrameCaptureHtml << "<td>IBuffer Offset</td>" << endl;
    logFrameCaptureHtml << "<td>VBuffer Size</td>" << endl;
    //logFrameCaptureHtml << "<td>VBuffer Offset</td>" << endl;
    logFrameCaptureHtml << "<td>VBuffer Stride</td>" << endl;
    logFrameCaptureHtml << "<td>First vertex data</td>" << endl;
    logFrameCaptureHtml << "</tr>" << endl;
}

void Logger::endFrameCapture()
{
    frameCaptureObjects.save(captureDir + "objects.txt");
    capturingFrame = false;
    captureDir = "";
    prevCaptureImage.free();
    logFrameCaptureFile.close();

    logFrameCaptureHtml << "</table>" << endl;
    logFrameCaptureHtml << "</body></html>" << endl;
    logFrameCaptureHtml.close();
}

void initGlobalLogger()
{
    if (g_logger != nullptr) return;

    g_logger = new Logger();

    g_logger->logDir = "d3d11Logs/";
    util::makeDirectory(g_logger->logDir);

    g_logger->logInterfaceFile.open(g_logger->logDir + "interfaceCalls.txt");
    g_logger->logDrawFile.open(g_logger->logDir + "drawCalls.txt");
    g_logger->logErrorFile.open(g_logger->logDir + "errors.txt");
    g_logger->logInputLayoutFile.open(g_logger->logDir + "inputLayouts.txt");
    g_logger->logSignatureFile.open(g_logger->logDir + "signatureCounts.txt");

    g_logger->colorMap.load(g_logger->colorMapFilename());
    g_logger->logSignatureFile << "initial signature color count = " << g_logger->colorMap.colors.size() << endl;
}

void initGlobalState()
{
    if (g_state != nullptr) return;

    g_state = new GlobalState();
    g_state->D3D11Handle = LoadLibrary("C:\\Windows\\System32\\d3d11Original.dll");
    if (g_state->D3D11Handle == nullptr)
    {
        g_logger->log("failed to load system d3d11.dll");
    }

    g_state->AI = GameAICreate();
    if (g_state->AI == nullptr)
    {
        g_logger->log("failed to create game AI");
    }
}

void initGlobals()
{
    initGlobalLogger();
    initGlobalState();
}
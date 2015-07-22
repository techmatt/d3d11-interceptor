
#include "Main.h"

void Logger::recordDrawEvent(MyD3DAssets &assets, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
    if (capturingFrame)
    {
        assets.loadVSConstantBuffer();
        const auto &indexBuffer = assets.getActiveIndexBuffer();
        const auto &vertexBuffer = assets.getActiveVertexBuffer();
        
        string constantList;
        for (size_t i = 0; i < assets.VSBufferSize; i++)
            constantList += to_string(assets.VSBufferStorage[i]) + " ";
        
        //g_logger->logFrameCaptureFile << "constants: " << constantList << endl;

        const string imagePrefix = "render" + util::zeroPad(captureRenderIndex, 5);
        const string frameImageFile = imagePrefix + "_frame.png";
        const string frameDeltaImageFile = imagePrefix + "_delta.png";
        const string texImageFile = imagePrefix + "_tex";

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

        for (int textureIndex = 0; textureIndex < 0; textureIndex++)
        {
            assets.loadPSTexture(textureIndex);
            modifyImage(assets.PSTexture);
            if (assets.PSTexture.size() > 0)
                LodePNG::save(assets.PSTexture, g_logger->captureDir + texImageFile + to_string(textureIndex) + ".png");
        }

        auto makeHTMLImage = [](const string &filename)
        {
            return "<img src=\"" + filename + "\" />";
        };

        logFrameCaptureHtml << "<tr>" << endl;
        logFrameCaptureHtml << "<td>" << captureRenderIndex << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(frameImageFile) << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(frameDeltaImageFile) << "</td>" << endl;
        /*logFrameCaptureHtml << "<td>" << makeHTMLImage(texImageFile + "0.png") << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(texImageFile + "1.png") << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(texImageFile + "2.png") << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(texImageFile + "3.png") << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(texImageFile + "4.png") << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(texImageFile + "5.png") << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(texImageFile + "6.png") << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << makeHTMLImage(texImageFile + "7.png") << "</td>" << endl;*/
        logFrameCaptureHtml << "<td>" << IndexCount << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << StartIndexLocation << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << BaseVertexLocation << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << ((indexBuffer.buffer == nullptr) ? "invalid" : to_string(indexBuffer.buffer->data.size())) + " " + pointerToString(indexBuffer.buffer->handle) << "</td>" << endl;
        //logFrameCaptureHtml << "<td>" << indexBuffer.offset << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << ((vertexBuffer.buffer == nullptr) ? "invalid" : to_string(vertexBuffer.buffer->data.size())) + " " + pointerToString(vertexBuffer.buffer->handle) << "</td>" << endl;
        //logFrameCaptureHtml << "<td>" << vertexBuffer.offset << "</td>" << endl;
        logFrameCaptureHtml << "<td>" << vertexBuffer.stride << "</td>" << endl;

        string v0Data = "<none>";
        if (BaseVertexLocation != -1 && vertexBuffer.buffer != nullptr && indexBuffer.buffer != nullptr)
        {
            const WORD *indexDataStart = (WORD *)indexBuffer.buffer->data.data() + StartIndexLocation;
            
            const BYTE *vertexData = vertexBuffer.buffer->data.data();

            v0Data = "data:<br />";

            for (int indexIndex = 0; indexIndex < min((int)IndexCount, 16); indexIndex++)
            {
                const int curIndex = indexDataStart[indexIndex] + BaseVertexLocation;
                const float *curVertex = (const float *)(vertexData + (vertexBuffer.stride * curIndex));

                if (vertexBuffer.buffer->data.size() >= vertexBuffer.stride * (curIndex + 1))
                {
                    continue;
                }

                if (vertexBuffer.stride / 4 >= 4)
                {
                    const vec3f basePos(curVertex[1], curVertex[2], curVertex[3]);
                    const vec3f worldPos = assets.transformObjectToWorldGamecube(basePos);
                    v0Data += worldPos.toString(", ") + " --- ";
                }

                for (int i = 0; i < (int)vertexBuffer.stride / 4; i++)
                {
                    v0Data += to_string(curVertex[i]) + ", ";
                }
                v0Data += "<br />";
            }
        }

        logFrameCaptureHtml << "<td>" << v0Data << "</td>" << endl;
        logFrameCaptureHtml << "</tr>" << endl;

        captureRenderIndex++;
    }
}

void Logger::beginFrameCapture()
{
    captureRenderIndex = 0;
    capturingFrame = true;
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
    /*logFrameCaptureHtml << "<td>Texture 0</td>" << endl;
    logFrameCaptureHtml << "<td>Texture 1</td>" << endl;
    logFrameCaptureHtml << "<td>Texture 2</td>" << endl;
    logFrameCaptureHtml << "<td>Texture 3</td>" << endl;
    logFrameCaptureHtml << "<td>Texture 4</td>" << endl;
    logFrameCaptureHtml << "<td>Texture 5</td>" << endl;
    logFrameCaptureHtml << "<td>Texture 6</td>" << endl;
    logFrameCaptureHtml << "<td>Texture 7</td>" << endl;*/
    logFrameCaptureHtml << "<td>Index Count</td>" << endl;
    logFrameCaptureHtml << "<td>Start Index</td>" << endl;
    logFrameCaptureHtml << "<td>Base Vertex Location</td>" << endl;
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
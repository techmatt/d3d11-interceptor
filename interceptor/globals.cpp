
#include "Main.h"

void Logger::recordDrawEvent(MyD3DAssets &assets)
{
    if (capturingFrame)
    {
        assets.loadVSConstantBuffer();
        
        string constantList;
        for (size_t i = 0; i < assets.VSBufferSize; i++)
            constantList += to_string(assets.VSBufferStorage[i]) + " ";
        
        //g_logger->logFrameCaptureFile << "constants: " << constantList << endl;

        Bitmap image;
        assets.context->readRenderTarget(image);
        LodePNG::save(image, g_logger->captureDir + "render" + util::zeroPad(captureRenderIndex, 5) + "_frame.png");

        assets.loadPSTexture(0);
        if (assets.PSTexture.size() > 0)
            LodePNG::save(assets.PSTexture, g_logger->captureDir + "render" + util::zeroPad(captureRenderIndex, 5) + "_tex0.png");

        assets.loadPSTexture(1);
        if (assets.PSTexture.size() > 0)
            LodePNG::save(assets.PSTexture, g_logger->captureDir + "render" + util::zeroPad(captureRenderIndex, 5) + "_tex1.png");

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
}

void Logger::endFrameCapture()
{
    capturingFrame = false;
    captureDir = "";
    logFrameCaptureFile.close();
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
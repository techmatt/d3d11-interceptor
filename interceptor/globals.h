
struct MyD3DAssets;
struct DrawParameters;

template<class T>
inline string pointerToString(T *ptr)
{
    std::ostringstream address;
    address << (void const *)ptr;
    return address.str();
}


struct Logger
{
    Logger()
    {
        frameIndex = 0;
        logInterfaceCalls = false;
        logDrawCalls = false;
        capturingFrame = false;
    }

    void log(const string &s)
    {
        logInterfaceFile << s << endl;
        if (capturingFrame)
            logFrameCaptureFile << s << endl;
    }

    void logDraw(const string &s)
    {
        logDrawFile << s << endl;
    }

    void recordSignatureColorPreDraw(MyD3DAssets &assets, const DrawParameters &params);
    void recordSignatureColorPostDraw(MyD3DAssets &assets, const DrawParameters &params, const GPUDrawBuffers &buffers);
    void recordDrawEvent(MyD3DAssets &assets, const DrawParameters &params);
    void beginFrameCapture();
    void endFrameCapture();

    string colorMapFilename()
    {
        return logDir + "../signatureColorMap.dat";
    }

    ofstream logSignatureFile;
    ofstream logInputLayoutFile;
    ofstream logErrorFile;
    ofstream logInterfaceFile;
    ofstream logDrawFile;
    ofstream logFrameCaptureFile;
    ofstream logFrameCaptureHtml;

    bool logInterfaceCalls;
    bool logDrawCalls;
    long frameIndex;
    long frameRenderIndex;

    string logDir;

    //
    // Databases
    //
    SignatureColorMap colorMap;
    GeometryDatabase geoDatabase;

    bool capturingColorSignature;
    Bitmap preRenderImage;
    Bitmap postRenderImage;
    int newSignaturesThisFrame;

    //
    // Single frame capture state
    //
    bool capturingFrame;
    string captureDir;
    Bitmap prevCaptureImage;
    FrameCollection frameCaptureObjects;

    //
    // All frame capture state
    //
    FrameObjectData *curFrame;
    FrameCollection allFrames;
};

extern Logger *g_logger;

struct GlobalState
{
    FARPROC getProcedure(LPCSTR procName)
    {
        auto result = GetProcAddress(D3D11Handle, procName);
        if (result == nullptr)
        {
            g_logger->log(string("Failed to load procedure: ") + procName);
        }
        return result;
    }

    HMODULE D3D11Handle;
    GameAIInterface *AI;
};

extern GlobalState *g_state;

void initGlobals();
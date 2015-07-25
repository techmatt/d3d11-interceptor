
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
        logInterfaceCalls = true;
        logDrawCalls = true;
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

    void recordSignatureColorPreDraw(MyD3DAssets &assets, DrawParameters &params);
    void recordSignatureColorPostDraw(MyD3DAssets &assets, const DrawParameters &params);
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

    bool capturingFrame;

    string logDir;
    string captureDir;

    SignatureColorMap colorMap;
    Bitmap preRenderImage;
    Bitmap postRenderImage;
    int newSignaturesThisFrame;

    Bitmap prevCaptureImage;

    ObjectCollection frameCaptureObjects;
    LocalizedObject objectStore;
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
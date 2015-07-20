
struct MyD3DAssets;

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

    void recordDrawEvent(MyD3DAssets &assets);
    void beginFrameCapture();
    void endFrameCapture();

    ofstream logErrorFile;
    ofstream logInterfaceFile;
    ofstream logDrawFile;
    ofstream logFrameCaptureFile;

    bool logInterfaceCalls;
    bool logDrawCalls;
    long frameIndex;
    long captureRenderIndex;

    bool capturingFrame;

    string logDir;
    string captureDir;
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

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
    }

    void logDraw(const string &s)
    {
        logDrawFile << s << endl;
    }

    void beginFrameCapture()
    {
        capturingFrame = true;
        logDrawFile << "Capturing frame " << frameIndex << endl;
        captureDir = logDir + "capture" + util::zeroPad(frameIndex, 6) + "/";
        util::makeDirectory(captureDir);
        logFrameCaptureFile.open(captureDir + "log.txt");
    }

    void endFrameCapture()
    {
        capturingFrame = false;
        captureDir = "";
        logFrameCaptureFile.close();
    }

    ofstream logErrorFile;
    ofstream logInterfaceFile;
    ofstream logDrawFile;
    ofstream logFrameCaptureFile;

    bool logInterfaceCalls;
    bool logDrawCalls;
    long frameIndex;

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
};

extern GlobalState *g_state;

inline void initGlobalLogger()
{
    if (g_logger != nullptr) return;

    g_logger = new Logger();

    g_logger->logDir = "d3d11Logs/";
    util::makeDirectory(g_logger->logDir);

    g_logger->logInterfaceFile.open(g_logger->logDir + "interfaceCalls.txt");
    g_logger->logDrawFile.open(g_logger->logDir + "drawCalls.txt");
    g_logger->logErrorFile.open(g_logger->logDir + "errors.txt");
}

inline void initGlobalState()
{
    if (g_state != nullptr) return;

    g_state = new GlobalState();
    g_state->D3D11Handle = LoadLibrary("C:\\Windows\\System32\\d3d11.dll");
    if (g_state->D3D11Handle == nullptr)
    {
        g_logger->log("failed to load system d3d11.dll");
    }
}

inline void initGlobals()
{
    initGlobalLogger();
    initGlobalState();
}
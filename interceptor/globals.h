
struct Logger
{
    Logger()
    {
        logInterfaceCalls = true;
    }

    void log(const string &s)
    {
        logFile << s << endl;
    }
    ofstream logFile;

    bool logInterfaceCalls;
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
    g_logger->logFile.open("d3d11InterceptorLog.txt");
}

inline void initGlobalState()
{
    if (g_state != nullptr) return;

    g_state = new GlobalState();
    g_state->D3D11Handle = LoadLibrary("C:\\Windows\\System32\\d3d11Original.dll");
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
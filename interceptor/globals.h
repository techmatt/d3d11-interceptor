
struct Logger
{
    Logger()
    {
        logInterfaceCalls = true;
        logDrawCalls = true;
    }

    void log(const string &s)
    {
        logInterfaceFile << s << endl;
    }

    void logDraw(const string &s)
    {
        logDrawFile << s << endl;
    }

    ofstream logInterfaceFile;
    ofstream logDrawFile;

    bool logInterfaceCalls;
    bool logDrawCalls;
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
    g_logger->logInterfaceFile.open("d3d11LogInterface.txt");
    g_logger->logDrawFile.open("d3d11LogDraw.txt");
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
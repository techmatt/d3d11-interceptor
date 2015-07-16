
struct Logger
{
    void log(const string &s)
    {
        logFile << s << endl;
    }
    ofstream logFile;
};

extern Logger *g_logger;

inline void initGlobalLogger()
{
    if (g_logger != nullptr) return;

    g_logger = new Logger();
    g_logger->logFile.open("d3d11InterceptorLog.txt");
}
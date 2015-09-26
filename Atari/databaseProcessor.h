
struct AppState;

struct DatabaseProcessor
{
public:
    void go(AppState &state);

private:
    void initSegmentManager(AppState &state);
    void processRawReplays(AppState &state);
};
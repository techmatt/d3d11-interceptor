
struct AppState;

struct DatabaseProcessor
{
public:
    void go(AppState &state);
    
private:
    void processRawReplays(AppState &state);
    void computeObjects(AppState &state);
};

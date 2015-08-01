
struct DatasetProcessor
{
    void go(AppState &state);

private:
    void alignFrames(AppState &state);
    void loadAlignedFrames(AppState &state);
};
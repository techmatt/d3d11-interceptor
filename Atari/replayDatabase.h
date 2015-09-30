
struct AnnotatedReplay
{
    Replay *replay;
    vector<Game::StateInst> states;
};

struct ReplayDatabase
{
    void init();
    void loadAnnotatedReplays();
    void loadGameStates(AppState &state);
    
    ReplayFrame& getFrame(FrameID id);
    const ReplayFrame& getFrame(FrameID id) const;

    vector<AnnotatedReplay*> replays;
};

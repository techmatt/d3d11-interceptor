
struct ReplayDatabase
{
    void init();
    void loadAnnotatedReplays();
    
    ReplayFrame& getFrame(FrameID id);
    const ReplayFrame& getFrame(FrameID id) const;

    vector<Replay*> replays;
};

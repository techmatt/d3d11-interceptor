
struct FrameID
{
    FrameID()
    {
        replayIndex = -1;
        frameIndex = -1;
    }
    FrameID(int _replayIndex, int _frameIndex)
    {
        replayIndex = _replayIndex;
        frameIndex = _frameIndex;
    }
    FrameID next() const
    {
        return FrameID(replayIndex, frameIndex + 1);
    }
    FrameID delta(int framesToAdvance) const
    {
        return FrameID(replayIndex, frameIndex + framesToAdvance);
    }
    string toString() const
    {
        return "r" + to_string(replayIndex) + "-f" + to_string(frameIndex);
    }
    int replayIndex;
    int frameIndex;
};

inline bool operator < (const FrameID &a, const FrameID &b)
{
    if (a.replayIndex < b.replayIndex)
        return true;
    if (a.replayIndex > b.replayIndex)
        return false;
    return a.frameIndex < b.frameIndex;
}

struct ProcessedFrame
{
    ProcessedFrame() {}
    ProcessedFrame(FrameObjectData *_frame, const FrameID &_frameID)
    {
        frame = _frame;
        frameID = _frameID;
        signatureMap = frame->makeSignatureMap();
    }

    FrameID frameID;
    FrameObjectData *frame;
    map<UINT64, vector<const LocalizedObjectData*> > signatureMap;
};

struct FramePair
{
    const ProcessedFrame *f0;
    const ProcessedFrame *f1;
};

struct ReplayDatabaseEntry
{
    int replayIndex;

    vector<FramePair> pairs;
    vector<ProcessedFrame> processedFrames;

    GameReplay *replay;
};

struct ReplayDatabase
{
    void addEntry(const string &alignedFrameFilename);

    ProcessedFrame* getFrame(const FrameID &frame)
    {
        ReplayDatabaseEntry &replay = entries[frame.replayIndex].get();
        if (frame.frameIndex < 0 || frame.frameIndex >= replay.processedFrames.size())
            return nullptr;
        return &replay.processedFrames[frame.frameIndex];
    }

    const ProcessedFrame* getFrame(const FrameID &frame) const
    {
        ReplayDatabaseEntry &replay = entries[frame.replayIndex].get();
        if (frame.frameIndex < 0 || frame.frameIndex >= replay.processedFrames.size())
            return nullptr;
        return &replay.processedFrames[frame.frameIndex];
    }

    vector< reference_wrapper<ReplayDatabaseEntry> > entries;
};
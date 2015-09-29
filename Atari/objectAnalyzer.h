
struct ObjectTrack
{
    ObjectTrack()
    {
        colorSignature = AtariUtil::randomSignatureColor();
    }
    
    FrameID startFrame;

    // track is the index of the object in ReplayFrame::objectAnnotations
    vector<int> track;

    vec4uc colorSignature;
};

struct ObjectStatistics
{
    ObjectStatistics()
    {
        name = "unnamed";
    }
    string name;
    unordered_set<UINT64> segmentHashes;
    vector<int> frameOccurrenceCount;
    Bitmap heatmap;
    vector<ObjectTrack*> tracks;
};

struct ObjectAnalyzer
{
    static UINT64 frameObjectHash(FrameID frame, int objectIndex);
    const ObjectTrack& findObjectTrack(FrameID frame, int objectIndex) const;

    void init(const SegmentManager &segments, const vec2i &screenDimensions);
    void AnalyzeFrame(const SegmentManager &segments, const ReplayFrame &frame);

    void startTracking(const ReplayFrame &firstFrame, int replayIndex);
    void trackFrames(const ReplayFrame &frameA, const ReplayFrame &frameB);

    void finalizeTracks();

    void assignObjectNames();

    void outputSegmentBlacklist(AppState &state, const Replay &replay) const;
    void outputViz(const string &dir) const;

    vector<ObjectStatistics> objects;

    // object tracks are index by FrameID + objectIndex pairs
    map<UINT64, ObjectTrack*> objectTracks;

    int liveReplayIndex;

    // map from previous object index to active track for that object
    map<int, ObjectTrack*> liveTracks;

private:
    
};

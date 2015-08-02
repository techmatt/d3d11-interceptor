
struct SegmentAnalyzer;

struct CharacterSegmentInstance
{
    UINT64 signature;
    vec3f worldCentroid;
    vec3f centeredCentroid;
};

struct CharacterFrameInstance
{
    CharacterFrameInstance()
    {
        animationClusterIndex = -1;
    }
    int animationClusterIndex;
    map<UINT64, CharacterSegmentInstance> segments;
};

struct AnimationTransition
{
    AnimationTransition()
    {
        frameCount = 0;
    }
    int frameCount;
};

struct AnimationCluster
{
    int index;
    CharacterFrameInstance seedInstance;
    int observations;
    map<int, AnimationTransition> transitionsTo;
    map<int, AnimationTransition> transitionsFrom;
};

struct Character
{
    void init(const vector<UINT64> &segments, int _characterIndex);
    void recordAllFrames(const ReplayDatabase &frames);

    const CharacterFrameInstance* findInstanceAtFrame(const string &frameID) const
    {
        if (allInstances.count(frameID) == 0)
            return nullptr;
        return &(allInstances.find(frameID)->second);
    }

    static double frameInstanceDistSqAvg(const CharacterFrameInstance &a, const CharacterFrameInstance &b);
    static double frameInstanceDistSqMax(const CharacterFrameInstance &a, const CharacterFrameInstance &b);

    set<UINT64> allSegments;
    int characterIndex;

    // maps from frame ID to character instance
     map<string, CharacterFrameInstance> allInstances;

    vector<AnimationCluster> clusters;

private:
    void updateClusters(CharacterFrameInstance &newInstance);
    void recordFrameAnimation(const ProcessedFrame &frame);
    void recordFrameTransition(const FramePair &pair);
};

struct CharacterDatabase
{
    void init(SegmentAnalyzer &segmentInfo);
    void recordAllFrames(const ReplayDatabase &frames);

    vector<Character> characters;
};
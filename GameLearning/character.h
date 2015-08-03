
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
        poseClusterIndex = -1;
        animationIndex = -1;
    }
    int poseClusterIndex;
    int animationIndex;
    map<UINT64, CharacterSegmentInstance> segments;
};

struct PoseTransition
{
    PoseTransition()
    {
        frameCount = 0;
    }
    int frameCount;
};

struct PoseCluster
{
    static float transitionSaliency(const map<int, PoseTransition> &map, int start, int candidate)
    {
        int frameSum = 0, candidateSum = 0;
        for (auto &e : map)
        {
            if (e.first != start)
                frameSum += e.second.frameCount;
            if (e.first == candidate)
                candidateSum += e.second.frameCount;
        }
        if (frameSum == 0) return 0.0f;
        if (candidateSum < learningParams().requiredAnimationTransitionFrames) return 0.0f;

        return (float)candidateSum / (float)frameSum;
    }

    int index;
    CharacterFrameInstance seedInstance;
    int observations;
    map<int, PoseTransition> transitionsTo;
    map<int, PoseTransition> transitionsFrom;
};

struct AnimationSequence
{
    int index;
    vector< vector<int> > poses;
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

    vector<PoseCluster> poseClusters;

    vector<AnimationSequence> sequences;

private:
    void updateClusters(CharacterFrameInstance &newInstance);
    void recordFramePoses(const ProcessedFrame &frame);
    void recordFrameTransition(const FramePair &pair);
    void computeAnimationSequences();
};

struct CharacterDatabase
{
    void init(SegmentAnalyzer &segmentInfo);
    void recordAllFrames(const ReplayDatabase &frames);

    vector<Character> characters;
};
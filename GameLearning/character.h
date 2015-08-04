
struct SegmentAnalyzer;
struct PoseCluster;
struct AnimationSequence;

struct PoseTransition
{
    PoseTransition()
    {
        frameCount = 0;
    }
    int frameCount;
};

struct CharacterSegmentInstance
{
    UINT64 signature;
    vec3f worldCentroid;
    vec3f centeredCentroid;
};

struct CharacterFrameInstance
{
    void makePoseDescriptor(const vector<UINT64> &segmentList, vector<float> &result);

    // this is the set of all clusters within clusterSoftAssignmentThreshold
    vector<PoseCluster*> poseClusters;

    // the set of all animation sequences this instance is a part of (should generally be 0 or 1)
    vector<AnimationSequence*> sequences;

    map<UINT64, CharacterSegmentInstance> segments;
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
    int observations;
    CharacterFrameInstance seedInstance;
    map<int, PoseTransition> transitionsTo;
    map<int, PoseTransition> transitionsFrom;
};

struct AnimationSequence
{
    int index;
    vector< vector<int> > poses;
    vector<FrameID> instances;
};

struct Character
{
    void init(const vector<UINT64> &segments, int _characterIndex);
    void recordAllFrames(const ReplayDatabase &frames);

    const CharacterFrameInstance* findInstanceAtFrame(const FrameID &frameID) const
    {
        if (allInstances.count(frameID) == 0)
            return nullptr;
        return &(allInstances.find(frameID)->second);
    }

    CharacterFrameInstance* findInstanceAtFrame(const FrameID &frameID)
    {
        if (allInstances.count(frameID) == 0)
            return nullptr;
        return &(allInstances.find(frameID)->second);
    }

    static double frameInstanceDistSqAvg(const CharacterFrameInstance &a, const CharacterFrameInstance &b);
    static double frameInstanceDistSqMax(const CharacterFrameInstance &a, const CharacterFrameInstance &b);

    vector<UINT64> allSegments;
    int characterIndex;

    // maps from frame ID to character instance
    map<FrameID, CharacterFrameInstance> allInstances;

    vector<PoseCluster> poseClusters;

    vector<AnimationSequence> sequences;

    map<int, vector<AnimationSequence*> > sequencesByFirstPose;

private:
    void addNewCluster(CharacterFrameInstance &newInstance);
    void assignClusters(CharacterFrameInstance &newInstance);
    void recordFramePoses(const ProcessedFrame &frame);
    void recordFrameTransition(const FramePair &pair);
    void computeAnimationSequences();
    void updateFirstPoseMap();

    void labelAnimationInstances();
    bool animationAtFrame(const AnimationSequence &sequence, const FrameID &startFrame) const;
};

struct CharacterDatabase
{
    void init(SegmentAnalyzer &segmentInfo);
    void recordAllFrames(const ReplayDatabase &frames);

    vector<Character> characters;
};
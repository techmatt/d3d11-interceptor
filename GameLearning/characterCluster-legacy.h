
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

struct InstanceAnimationEntry
{
    int sequenceIndex;
    int sequenceOffset;
    float weight;
};

struct CharacterFrameInstance
{
    CharacterFrameInstance()
    {
        candidateAnimationLength = -1;
    }

    FrameID frameID;
    void makeRawPoseDescriptor(const vector<UINT64> &segmentList, float *result);

    // this is the set of all clusters within clusterSoftAssignmentThreshold
    vector<PoseCluster*> poseClusters;

    // the set of all animation sequences this instance is a part of
    vector<InstanceAnimationEntry> sequences;

    // the ideal of an animation, if one were to start at this instance
    int candidateAnimationLength;

    map<UINT64, CharacterSegmentInstance> segments;
};

struct CharacterFrameInstanceCompare
{
    bool operator() (const CharacterFrameInstance *a, const CharacterFrameInstance *b)
    {
        return a->candidateAnimationLength < b->candidateAnimationLength;
    }
};

struct PoseCluster
{
    int index;
    CharacterFrameInstance seedInstance;
    vector<FrameID> observations;

    map<int, PoseTransition> transitionsTo;
    map<int, PoseTransition> transitionsFrom;
};

struct AnimationSequence
{
    int index;
    vec3f color;
    vector< set<int> > poses;
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

    int characterIndex;
    vector<UINT64> allSegmentsVec;
    set<UINT64> allSegmentsSet;
    
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
    
    int computeAnimationOverlap(const CharacterFrameInstance &instanceAStart, const CharacterFrameInstance &instanceBStart, int animationLength) const;
    int computeAnimationInstances(const CharacterFrameInstance &startInstance, int animationLength, vector< set<PoseCluster*> > &poses, vector<FrameID> &startingFrames, bool emitPoses) const;
    vector<int> computeAnimationInstanceCounts(const CharacterFrameInstance &startInstance) const;

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
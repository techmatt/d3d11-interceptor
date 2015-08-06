
struct SegmentAnalyzer;
struct AnimationSequence;

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

//
// an instance of a character in a single frame
//
struct CharacterInstance
{
    FrameID frameID;
    void makeRawPoseDescriptor(const vector<UINT64> &segmentList, float *result) const;

    vector<float> reducedPoseDescriptor;
    vector<float> reducedAnimationDescriptor;

    // the set of all animation sequences this instance is a part of
    vector<InstanceAnimationEntry> sequences;

    map<UINT64, CharacterSegmentInstance> segments;

    int optimalAnimationWindowSize;
    int estimatedAnimationInstanceCount;
};

struct CharacterInstanceCompare
{
    bool operator() (const CharacterInstance *a, const CharacterInstance *b)
    {
        if (a->optimalAnimationWindowSize == b->optimalAnimationWindowSize)
            return a->estimatedAnimationInstanceCount < b->estimatedAnimationInstanceCount;
        return a->optimalAnimationWindowSize < b->optimalAnimationWindowSize;
    }
};

struct AnimationSequence
{
    int index;
    vec3f color;
    vector< vector<float> > animationDescriptors;

    // the frameID of the every instance of this animation
    vector<FrameID> instances;
};

struct Character
{
    void init(const vector<UINT64> &segments, int _characterIndex);
    void recordAllFrames(const ReplayDatabase &frames);
    
    const CharacterInstance* findInstanceAtFrame(const FrameID &frameID) const
    {
        if (allInstances.count(frameID) == 0)
            return nullptr;
        return &(allInstances.find(frameID)->second);
    }

    CharacterInstance* findInstanceAtFrame(const FrameID &frameID)
    {
        if (allInstances.count(frameID) == 0)
            return nullptr;
        return &(allInstances.find(frameID)->second);
    }

    int characterIndex;
    vector<UINT64> allSegmentsVec;
    set<UINT64> allSegmentsSet;
    
    // maps from frame ID to character instance
    map<FrameID, CharacterInstance> allInstances;
    vector<CharacterInstance*> allInstancesVec;

    vector<AnimationSequence> sequences;

    PCAf posePCA;
    int posePCADimension;

    PCAf animationPCA;
    int animationPCADimension;

    LSHEuclidean<CharacterInstance*> animationSearch;

    float animationDistance(const FrameID &a, const FrameID &b) const
    {
        const CharacterInstance *aInst = findInstanceAtFrame(a);
        const CharacterInstance *bInst = findInstanceAtFrame(b);
        if (aInst == nullptr || bInst == nullptr)
            return numeric_limits<float>::max();
        return math::distSq(aInst->reducedAnimationDescriptor, bInst->reducedAnimationDescriptor);
    }

private:
    void recordFramePoses(const ProcessedFrame &frame);
    bool computeAnimationDescriptor(const FrameID &centerFrame, float *result);

    void computePosePCA();
    void computePoseDescriptors();
    void computeAnimationPCA();
    void computeAnimationDescriptors();
    void makeAnimationSearch();
    void testAnimationSearch(float pNorm, UINT miniHashFunctionCount, UINT macroTableCount);

    int evaluateAnimationMatchingFrames(const CharacterInstance &frameA, const CharacterInstance &frameB, int windowSize);
    int evaluateAnimationInstances(const CharacterInstance &seed, int windowSize, vector<FrameID> *instanceFrames = nullptr);

    int evaluateBestWindowSize(const CharacterInstance &seed, int &instanceCount);

    void computeAnimationSequences();
};

struct CharacterDatabase
{
    void init(SegmentAnalyzer &segmentInfo);
    void recordAllFrames(const ReplayDatabase &frames);

    vector<Character> characters;
};
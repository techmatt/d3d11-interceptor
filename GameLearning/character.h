
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

struct CharacterFrameInstance
{
    FrameID frameID;
    void makeRawPoseDescriptor(const vector<UINT64> &segmentList, float *result) const;

    vector<float> reducedPoseDescriptor;
    vector<float> reducedAnimationDescriptor;

    // the set of all animation sequences this instance is a part of
    vector<InstanceAnimationEntry> sequences;

    map<UINT64, CharacterSegmentInstance> segments;
};

struct AnimationSequence
{
    int index;
    vec3f color;
    vector< vector<float> > animationDescriptors;
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

    int characterIndex;
    vector<UINT64> allSegmentsVec;
    set<UINT64> allSegmentsSet;
    
    // maps from frame ID to character instance
    map<FrameID, CharacterFrameInstance> allInstances;
    vector<CharacterFrameInstance*> allInstancesVec;

    vector<AnimationSequence> sequences;

    PCAf posePCA;
    int posePCADimension;

    PCAf animationPCA;
    int animationPCADimension;

    LSHEuclidean<CharacterFrameInstance*> animationSearch;

    static float L2DistSq(const vector<float> &a, const vector<float> &b)
    {
        const size_t n = a.size();
        float sum = 0.0f;
        for (size_t i = 0; i < n; i++)
        {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        return sum;
    }

    float animationDistance(const FrameID &a, const FrameID &b) const
    {
        const CharacterFrameInstance *aInst = findInstanceAtFrame(a);
        const CharacterFrameInstance *bInst = findInstanceAtFrame(b);
        if (aInst == nullptr || bInst == nullptr)
            return numeric_limits<float>::max();
        return L2DistSq(aInst->reducedAnimationDescriptor, bInst->reducedAnimationDescriptor);
    }

private:
    void computePosePCA();
    void computePoseDescriptors();
    void computeAnimationPCA();
    void computeAnimationDescriptors();
    void makeAnimationSearch();
    void testAnimationSearch(float pNorm, UINT miniHashFunctionCount, UINT macroTableCount);

    void computeAnimationDescriptor(const FrameID &centerFrame, float *result);

    void recordFramePoses(const ProcessedFrame &frame);
    void computeAnimationSequences();
};

struct CharacterDatabase
{
    void init(SegmentAnalyzer &segmentInfo);
    void recordAllFrames(const ReplayDatabase &frames);

    vector<Character> characters;
};
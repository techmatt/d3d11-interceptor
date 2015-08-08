
struct SegmentAnalyzer;
struct AnimationSequence;

struct CharacterSegmentInstance
{
    UINT64 signature;
    vec3f worldCentroid;
    vec3f centeredCentroid;
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

    AnimationSequence *sequence;

    map<UINT64, CharacterSegmentInstance> segments;
};

struct AnimationGraphNeighborEdge
{
    float totalDeviation() const
    {
        return deviationAToB + deviationBToA;
    }
    CharacterInstance *instA;
    CharacterInstance *instB;

    float deviationAToB;
    float deviationBToA;
};

inline bool operator < (const AnimationGraphNeighborEdge &a, const AnimationGraphNeighborEdge &b)
{
    return a.totalDeviation() > b.totalDeviation();
}

struct AnimationGraphSimilarEdge
{
    CharacterInstance *instA;
    CharacterInstance *instB;

    float distSq;
};

inline bool operator < (const AnimationGraphSimilarEdge &a, const AnimationGraphSimilarEdge &b)
{
    return a.distSq > b.distSq;
}

struct AnimationSequence
{
    AnimationSequence(FrameID seedFrame)
    {
        instances.push_back(seedFrame);
        maxDeviation = 0.0f;
        minSeedDistSq = 0.0f;

        color = vec3f::origin;
        while (color.length() < 0.5f)
            color = vec3f((float)util::randomUniform(), (float)util::randomUniform(), (float)util::randomUniform());

    }
    void addDeviation(float deviation)
    {
        deviations.push_back(deviation);
        maxDeviation = max(maxDeviation, deviation);
    }
    vector<float> deviations;
    float maxDeviation;

    float minSeedDistSq;

    int index;
    vec3f color;
    
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

    set<AnimationSequence*> sequences;

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

    float poseDistance(const FrameID &a, const FrameID &b) const
    {
        const CharacterInstance *aInst = findInstanceAtFrame(a);
        const CharacterInstance *bInst = findInstanceAtFrame(b);
        if (aInst == nullptr || bInst == nullptr)
            return numeric_limits<float>::max();
        return math::distSq(aInst->reducedPoseDescriptor, bInst->reducedPoseDescriptor);
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

    void mergeAnimations(AnimationSequence *a, AnimationSequence *b, float seedDistSq, float neighborDeviation);
    bool animationsShouldMergeDeviation(AnimationSequence *a, AnimationSequence *b, float deviation);
    bool animationsShouldMergeSeed(AnimationSequence *a, AnimationSequence *b, float seedDistSq);

    vector<CharacterInstance*> findKNearestInstances(const CharacterInstance &instance, int k);
    float computeTransitionDeviation(const CharacterInstance &instStart, int delta);

    void computeAnimationSequences();
    void mergeNearestSequences();
    void mergeAdjacentFrames();
};

struct CharacterDatabase
{
    void init(SegmentAnalyzer &segmentInfo);
    void recordAllFrames(const ReplayDatabase &frames);

    vector<Character> characters;
};
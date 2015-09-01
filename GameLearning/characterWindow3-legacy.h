
struct SegmentAnalyzer;
struct AnimationSequence;

struct CharacterSegmentInstance
{
    UINT64 signature;
    vec3f worldCentroid;
    vec3f centeredCentroid;
};

struct AnimationFrame
{
    AnimationFrame()
    {
        animation = nullptr;
        offset = -1;
        poseDistSq = std::numeric_limits<float>::max();
    }
    AnimationFrame(AnimationSequence *_animation, int _offset, float _poseDistSq)
    {
        animation = _animation;
        offset = _offset;
        poseDistSq = _poseDistSq;
    }
    AnimationSequence *animation;
    int offset;
    float poseDistSq;
};

//
// an instance of a character in a single frame
//
struct CharacterInstance
{
    FrameID frameID;
    void makeRawPoseDescriptor(const vector<UINT64> &segmentList, float *result) const;

    vector<float> reducedPoseDescriptor;
    vector<float> reducedPoseChainDescriptor;

    AnimationFrame animation;

    map<UINT64, CharacterSegmentInstance> segments;

    int optimalAnimationLength;
    int estimatedAnimationInstanceCount;
};

struct CharacterInstanceCompare
{
    bool operator() (const CharacterInstance *a, const CharacterInstance *b)
    {
        if (a->optimalAnimationLength == b->optimalAnimationLength)
            return a->estimatedAnimationInstanceCount < b->estimatedAnimationInstanceCount;
        return a->optimalAnimationLength < b->optimalAnimationLength;
    }
};

struct AnimationSequence
{
    AnimationSequence(FrameID _seedFrame, int _length, int animationIndex)
    {
        index = animationIndex;
        seedFrame = _seedFrame;
        length = _length;
        instances.push_back(seedFrame);
        
        color = vec3f::origin;
        while (color.length() < 0.5f)
            color = vec3f((float)util::randomUniform(), (float)util::randomUniform(), (float)util::randomUniform());
    }

    int index;

    FrameID seedFrame;
    int length;

    vec3f color;
    vector<FrameID> instances;
};

inline bool operator < (const AnimationFrame &a, const AnimationFrame &b)
{
    if (a.animation->index == b.animation->index)
        return a.offset < b.offset;
    return a.animation->index < b.animation->index;
}

struct Character
{
    void init(const vector<UINT64> &segments, int _characterIndex);
    void recordAllFrames(const ReplayDatabase &frames);

    bool makeInstance(const ProcessedFrame &frame, CharacterInstance &result) const;
    
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

    float animationDistance(const FrameID &a, const FrameID &b) const
    {
        const CharacterInstance *aInst = findInstanceAtFrame(a);
        const CharacterInstance *bInst = findInstanceAtFrame(b);
        if (aInst == nullptr || bInst == nullptr)
            return numeric_limits<float>::max();
        return math::distSq(aInst->reducedPoseChainDescriptor, bInst->reducedPoseChainDescriptor);
    }

    float poseDistance(const FrameID &a, const FrameID &b) const
    {
        const CharacterInstance *aInst = findInstanceAtFrame(a);
        const CharacterInstance *bInst = findInstanceAtFrame(b);
        if (aInst == nullptr || bInst == nullptr)
            return numeric_limits<float>::max();
        return math::distSq(aInst->reducedPoseDescriptor, bInst->reducedPoseDescriptor);
    }

    int characterIndex;
    vector<UINT64> allSegmentsVec;
    set<UINT64> allSegmentsSet;
    
    // maps from frame ID to character instance
    map<FrameID, CharacterInstance> allInstances;
    vector<CharacterInstance*> allInstancesVec;

    vector<AnimationSequence*> animations;

    PCAf posePCA;
    int posePCADimension;

    PCAf poseChainPCA;
    int poseChainPCADimension;

    LSHEuclidean<AnimationFrame> animationFrameSearch;
    LSHEuclidean<CharacterInstance*> poseChainSearch;
    LSHEuclidean<CharacterInstance*> poseSearch;

    vector< AnimationFrame > findAnimationFramesRadius(const CharacterInstance &instance, float maxDistSq) const;
    vector< pair<CharacterInstance*, float> > findPosesRadius(const CharacterInstance &instance, float maxDistSq) const;
    vector< pair<CharacterInstance*, float> > findPoseChainsRadius(const CharacterInstance &instance, float maxDistSq) const;

private:
    void recordFramePoses(const ProcessedFrame &frame);
    bool computeAnimationDescriptor(const FrameID &centerFrame, float *result);

    void saveAnimationCurve();

    void saveAnimations(const string &filename) const;
    void loadAnimations(const string &filename);

    //
    // dimensionality reduction
    //
    void computePosePCA();
    void computePoseDescriptors();
    void computeAnimationPCA();
    void computeAnimationDescriptors();

    //
    // LSH
    //
    void makePoseLSHSearch();
    void makeAnimationLSHSearch();
    void testAnimationSearch(float pNorm, UINT miniHashFunctionCount, UINT macroTableCount);

    vector<int> animationMatchingFrames(const CharacterInstance &frameA, const CharacterInstance &frameB, int animationLength, float acceptanceScale);
    int countAnimationInstances(const CharacterInstance &seed, int animationLength, float acceptanceScale);
    int evaluateBestAnimationLength(const CharacterInstance &seed, float acceptanceScale, int &instanceCount);
    int computeAnimationDuration(const CharacterInstance &seed, const CharacterInstance &instance, int animationLength, float acceptanceScale);

    void addAnimationSequences(float acceptanceScale, int minWindowSize);

    void addNewAnimation(const CharacterInstance &seed, int animationLength);

    void computeAnimationSequences();
};

struct CharacterDatabase
{
    void init(SegmentAnalyzer &segmentInfo);
    void recordAllFrames(const ReplayDatabase &frames);

    vector<Character> characters;
};
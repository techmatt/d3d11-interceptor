
struct SegmentAnalyzer;
struct AnimationSequence;

struct CharacterSegmentInstance
{
    UINT64 signature;
    vec3f worldCentroid;
    vec3f centeredCentroid;
};

struct PoseCluster
{
    FrameID seedFrame;
    vector<FrameID> allInstances;

    int index;
    vector<AnimationSequence*> animations;
};

struct AnimationFrame
{
    AnimationFrame()
    {
        sequence = nullptr;
        offset = -1;
        poseDistSq = std::numeric_limits<float>::max();
    }
    AnimationFrame(AnimationSequence *_sequence, int _offset, float _poseDistSq)
    {
        sequence = _sequence;
        offset = _offset;
        poseDistSq = _poseDistSq;
    }
    AnimationSequence *sequence;
    int offset;
    float poseDistSq;
};

//
// an instance of a character in a single frame
//
struct CharacterInstance
{
    CharacterInstance()
    {
        poseCluster = nullptr;
        similarPoseChainCount = -1;
    }
    
    void makeRawPoseDescriptor(const vector<UINT64> &segmentList, float *result) const;

    vector<float> poseDescriptor;
    vector<float> poseChainForwardDescriptor;
    vector<float> poseChainReverseDescriptor;

    FrameID frameID;

    PoseCluster *poseCluster;
    AnimationFrame animation;

    vec3f worldCentroid;
    map<UINT64, CharacterSegmentInstance> segments;

    int similarPoseChainCount;
};

struct CharacterInstanceCompare
{
    bool operator() (const CharacterInstance *a, const CharacterInstance *b)
    {
        return a->similarPoseChainCount < b->similarPoseChainCount;
    }
};

struct AnimationSequence
{
    AnimationSequence(FrameID _seedFrame, int _seedLength, int animationIndex)
    {
        index = animationIndex;
        seedFrame = _seedFrame;
        seedLength = _seedLength;
        
        color = vec3f::origin;
        while (color.length() < 0.5f)
            color = vec3f((float)util::randomUniform(), (float)util::randomUniform(), (float)util::randomUniform());
    }

    int index;

    FrameID seedFrame;
    int seedLength;

    vec3f color;

    vector<FrameID> instances;
};

struct Character
{
    void init(const vector<UINT64> &segments, int _characterIndex);
    void recordAllFrames(const ReplayDatabase &frames);

    //bool makeInstance(const ProcessedFrame &frame, CharacterInstance &result) const;
    
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

    float poseChainForwardDistance(const FrameID &a, const FrameID &b) const
    {
        const CharacterInstance *aInst = findInstanceAtFrame(a);
        const CharacterInstance *bInst = findInstanceAtFrame(b);
        if (aInst == nullptr || bInst == nullptr)
            return numeric_limits<float>::max();
        return math::distSq(aInst->poseChainForwardDescriptor, bInst->poseChainForwardDescriptor);
    }

    float poseChainReverseDistance(const FrameID &a, const FrameID &b) const
    {
        const CharacterInstance *aInst = findInstanceAtFrame(a);
        const CharacterInstance *bInst = findInstanceAtFrame(b);
        if (aInst == nullptr || bInst == nullptr)
            return numeric_limits<float>::max();
        return math::distSq(aInst->poseChainReverseDescriptor, bInst->poseChainReverseDescriptor);
    }

    float poseChainReverseDistance(const deque<const PoseCluster *> &clusterHistory, const FrameID &a) const
    {
        if (clusterHistory.size() != Constants::poseChainReverseLength) return -1.0f;

        const CharacterInstance *aInst = findInstanceAtFrame(a);
        if (aInst == nullptr)
            return numeric_limits<float>::max();

        vector<float> rawPoseChainReverseDescriptor(poseChainReverseFeatureCount());
        vector<float> reducedDescriptor;

        computePoseChainReverseDescriptor(clusterHistory, rawPoseChainReverseDescriptor.data());

        poseChainReversePCA.transform(rawPoseChainReverseDescriptor, poseChainReversePCADimension, reducedDescriptor);

        return math::distSq(aInst->poseChainReverseDescriptor, reducedDescriptor);
    }

    float poseDistance(const FrameID &a, const FrameID &b) const
    {
        const CharacterInstance *aInst = findInstanceAtFrame(a);
        const CharacterInstance *bInst = findInstanceAtFrame(b);
        if (aInst == nullptr || bInst == nullptr)
            return numeric_limits<float>::max();
        return math::distSq(aInst->poseDescriptor, bInst->poseDescriptor);
    }

    int characterIndex;
    vector<UINT64> allSegmentsVec;
    set<UINT64> allSegmentsSet;
    
    // maps from frame ID to character instance
    map<FrameID, CharacterInstance> allInstances;
    vector<CharacterInstance*> allInstancesVec;

    vector<PoseCluster*> poseClusters;

    vector<AnimationSequence*> animations;

    PCAf posePCA;
    int posePCADimension;

    PCAf poseChainForwardPCA;
    int poseChainForwardPCADimension;

    PCAf poseChainReversePCA;
    int poseChainReversePCADimension;

    LSHEuclidean<CharacterInstance*> poseSearch;
    LSHEuclidean<CharacterInstance*> poseChainForwardSearch;
    LSHEuclidean<CharacterInstance*> poseChainReverseSearch;
    LSHEuclidean<PoseCluster*> poseClusterSearch;

    vector< pair<CharacterInstance*, float> > findPosesRadius(const CharacterInstance &instance, float maxDistSq) const;
    vector< pair<CharacterInstance*, float> > findPoseChainsForwardRadius(const CharacterInstance &instance, float maxDistSq) const;
    vector< pair<CharacterInstance*, float> > findPoseChainsReverseRadius(const CharacterInstance &instance, float maxDistSq) const;
    vector< pair<CharacterInstance*, float> > findSimilarClusterHistoryInstances(const deque<const PoseCluster *> &clusterHistory, float maxDistSq) const;
    PoseCluster* findBestPoseCluster(const CharacterInstance &instance, float maxDistSq) const;
    vector< pair<PoseCluster*, float> > findPoseClustersRadius(const CharacterInstance &instance, float maxDistSq) const;
    
private:

    //int animationsOverlap(const CharacterInstance &seedInstance, const CharacterInstance &otherInstance, int animationLength);
    //int countSimilarAnimations(const CharacterInstance &seedInstance, int animationLength);

    void recordFramePoses(const ProcessedFrame &frame);
    bool computePoseChainForwardDescriptor(const FrameID &startFrame, float *result) const;
    bool computePoseChainReverseDescriptor(const FrameID &startFrame, float *result) const;
    void computePoseChainReverseDescriptor(const deque<const PoseCluster *> &clusterHistory, float *result) const;
    
    int poseChainReverseFeatureCount() const;

    void saveAnimationCurve();
    void saveControllerCurve();

    void saveAnimations(const string &filename) const;
    void loadAnimations(const string &filename);

    //
    // dimensionality reduction
    //
    void computePosePCA();
    void computePoseDescriptors();
    void computePoseChainForwardPCA();
    void computePoseChainForwardDescriptors();
    void computePoseChainReversePCA();
    void computePoseChainReverseDescriptors();

    //
    // LSH
    //
    void makePoseLSHSearch();
    void testAnimationSearch(float pNorm, UINT miniHashFunctionCount, UINT macroTableCount);

    void addNewAnimation(const CharacterInstance &seed);

    void makePoseClusters();
    void computeAnimationSequences();
};

struct CharacterDatabase
{
    void init(SegmentAnalyzer &segmentInfo);
    void recordAllFrames(const ReplayDatabase &frames);

    vector<Character> characters;
};
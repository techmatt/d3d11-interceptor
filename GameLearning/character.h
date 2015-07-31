
struct CharacterSegment
{
    CharacterSegment() {}
    CharacterSegment(UINT64 _signature)
    {
        signature = _signature;
    }
    UINT64 signature;
};

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
    int frameIndex;
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
    int seedFrameIndex;
    int observations;
    map<int, AnimationTransition> transitionsTo;
    map<int, AnimationTransition> transitionsFrom;
};

struct Character
{
    void load(const FrameCollection &frames, const vector<CharacterSegment> &segments, int _characterIndex);

    const CharacterFrameInstance* findInstanceAtFrame(int frameIndex) const
    {
        if (allInstances.count(frameIndex) == 0)
            return nullptr;
        return &(allInstances.find(frameIndex)->second);
    }

    static double frameInstanceDistMaxError(const CharacterFrameInstance &a, const CharacterFrameInstance &b);
    static double frameInstanceDistL2(const CharacterFrameInstance &a, const CharacterFrameInstance &b);

    map<UINT64, CharacterSegment> allSegments;
    int characterIndex;

    // maps from frame index to character instance
    map<int, CharacterFrameInstance> allInstances;

    vector<AnimationCluster> clusters;

private:
    void clusterAnimations();
    void makeTransitionTables(const FrameCollection &frames);
};

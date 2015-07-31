
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

struct Character
{
    void load(const FrameCollection &frames, const vector<CharacterSegment> &segments, int _characterIndex);

    map<UINT64, CharacterSegment> allSegments;
    int characterIndex;

    vector<CharacterFrameInstance> allInstances;

    int animationClusterCount;

private:
    double frameInstanceDistL2(const CharacterFrameInstance &a, const CharacterFrameInstance &b);
    void clusterAnimations();
};
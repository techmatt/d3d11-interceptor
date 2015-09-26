
struct SegmentAnimation;

struct GameObject
{
    GameObject()
    {
        index = -1;
    }
    int index;
    vector<SegmentAnimation*> segments;
};

struct SegmentAnimation
{
    SegmentAnimation()
    {
        index = -1;
        object = nullptr;
    }

    int index;
    UINT64 hash;
    vec2s dimensions;
    BYTE color;
    int count;
    set<vec2s> mask;

    GameObject *object;
};

/*struct Segment
{
    BYTE color;
    vector<SegmentAnimation*> animations;
};*/

struct SegmentManager
{
    void init();

    void recordAndAnnotateSegments(const ColourPalette &palette, ReplayFrame &frame);

    void saveVizColors(const ColourPalette &palette, const string &dir);
    void saveVizObjects(const ColourPalette &palette, const string &dir);

    void save(const string &filename) const;
    void load(const string &filename);

    SegmentAnimation* getSegment(UINT64 hash) const
    {
        if (segmentsByHash.count(hash) == 0)
            return nullptr;
        return segmentsByHash.find(hash)->second;
    }

    size_t segmentCount() const
    {
        return segmentsByHash.size();
    }

    static const int segmentMaxRadius = 80;

    set<string> processedReplays;

    map<BYTE, vector<SegmentAnimation*> > segmentsByColor;
    map<UINT64, SegmentAnimation* > segmentsByHash;

    vector<GameObject*> objects;

private:

    void recordAndAnnotateSegments(ReplayFrame &frame, BYTE color);
    set<vec2s> extractMask(const ReplayFrame &frame, const vec2s &seed, vec2s &maskOriginOut);
    pair<SegmentAnimation*, int> findClosestMask(const set<vec2s> &mask, BYTE color);
    SegmentAnimation* findExactMask(const set<vec2s> &mask, BYTE color);

    //
    // utility
    //
    //Bitmap makeAnimationViz(const ColourPalette &palette, BYTE color);

    Grid2<BYTE> scratchpad;
    vector<vec4uc> colorBlacklist;
};
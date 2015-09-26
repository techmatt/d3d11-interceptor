
struct SegmentAnimation
{
    UINT64 hash;
    vec2s dimensions;
    BYTE color;
    int count;
    set<vec2s> mask;
};

/*struct Segment
{
    BYTE color;
    vector<SegmentAnimation*> animations;
};*/

struct SegmentManager
{
    void recordSegments(const ColourPalette &palette, const ReplayFrame &frame);
    void saveAllViz(const ColourPalette &palette, const string &dir);

    void save(const string &filename) const;
    void load(const string &filename);

    set<string> processedReplays;

    size_t segmentCount() const
    {
        return animationsByHash.size();
    }

private:
    void recordSegments(const ReplayFrame &frame, BYTE color);
    set<vec2s> extractMask(const ReplayFrame &frame, const vec2s &seed);
    pair<SegmentAnimation*, int> findClosestMask(const set<vec2s> &mask, BYTE color);
    SegmentAnimation* findExactMask(const set<vec2s> &mask, BYTE color);

    //
    // utility
    //
    Bitmap makeAnimationViz(const ColourPalette &palette, BYTE color);

    static const int segmentMaxRadius = 80;

    map<BYTE, vector<SegmentAnimation*> > animationsByColor;
    map<UINT64, SegmentAnimation* > animationsByHash;

    Grid2<BYTE> scratchpad;
};
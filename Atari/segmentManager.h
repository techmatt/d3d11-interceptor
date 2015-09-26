
struct SegmentAnimation
{
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

private:
    void recordSegments(const ReplayFrame &frame, BYTE color);
    set<vec2s> extractMask(const ReplayFrame &frame, const vec2s &seed);
    pair<SegmentAnimation*, int> findClosestMask(const set<vec2s> &mask, BYTE color);

    //
    // utility
    //
    static int maskDiff(const set<vec2s> &a, const set<vec2s> &b);
    Bitmap makeAnimationViz(const ColourPalette &palette, BYTE color);

    static const int segmentMaxRadius = 80;

    map<BYTE, vector<SegmentAnimation*> > allAnimations;

    Grid2<BYTE> scratchpad;
};

struct FrameID
{
    FrameID()
    {
        replayIndex = -1;
        frameIndex = -1;
    }
    FrameID(int _replayIndex, int _frameIndex)
    {
        replayIndex = _replayIndex;
        frameIndex = _frameIndex;
    }
    FrameID next() const
    {
        return FrameID(replayIndex, frameIndex + 1);
    }
    FrameID delta(int framesToAdvance) const
    {
        return FrameID(replayIndex, frameIndex + framesToAdvance);
    }
    string toString() const
    {
        return "r" + to_string(replayIndex) + "-f" + to_string(frameIndex);
    }
    int replayIndex;
    int frameIndex;
};

struct AtariImage
{
    void fromScreen(const ALEScreen &screen);
    Bitmap toBmp(const ColourPalette &palette) const;
    void toBmp(const ColourPalette &palette, Bitmap &bmpOut) const;

    Grid2<BYTE> data;
};

struct SegmentAnnotation
{
    SegmentAnnotation()
    {
        origin = vec2s(-1000, -1000);
        segmentHash = 0;
    }
    SegmentAnnotation(const vec2s &_origin, UINT64 _segmentHash)
    {
        origin = _origin;
        segmentHash = _segmentHash;
    }
    vec2s origin;
    UINT64 segmentHash;
};

struct ObjectAnnotation
{
    ObjectAnnotation()
    {
        objectID = -1;
    }
    ObjectAnnotation(const vec2s &_origin)
    {
        objectID = -1;
        origin = _origin;
    }
    int objectID;
    vec2s origin;
    vector<short> segments;
};

template<class BinaryDataBuffer, class BinaryDataCompressor>
inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator<<(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, const ObjectAnnotation &o) {
    s << o.origin;
    s.writePrimitiveVector(o.segments);
    return s;
}

template<class BinaryDataBuffer, class BinaryDataCompressor>
inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator>>(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, ObjectAnnotation &o) {
    s >> o.origin;
    s.readPrimitiveVector(o.segments);
    return s;
}

struct ReplayFrame
{
    void updateObjectIDs(const SegmentDatabase &database);

    int index;
    Action action;
    int reward;

    vector<SegmentAnnotation> segmentAnnotations;
    vector<ObjectAnnotation> objectAnnotations;

    AtariImage image;
};

struct Replay
{
    Replay()
    {
        id = ml::util::hash64(ml::util::randomInteger(0, 0xFFFFFF) + rand());
        romName = "unknown";
        index = -1;
    }

    void save(const string &filename) const;
    void load(const string &filename);

    void updateObjectIDs(const SegmentDatabase &database);

    void clearImages();

    int index;
    string romName;
    UINT64 id;
    vector< ReplayFrame* > frames;
};

struct AtariImage
{
    void fromScreen(const ALEScreen &screen);
    void toBmp(const ColourPalette &palette, Bitmap &bmpOut) const;

    Grid2<BYTE> data;
};

struct ReplayAnnotation
{
    ReplayAnnotation() {}
    ReplayAnnotation(const vec2s &_origin, UINT64 _segmentHash)
    {
        origin = _origin;
        segmentHash = _segmentHash;
    }
    vec2s origin;
    UINT64 segmentHash;
};

struct ReplayFrame
{
    int index;
    Action action;
    int reward;

    vector<ReplayAnnotation> annotations;

    AtariImage image;
};

struct Replay
{
    Replay()
    {
        id = ml::util::hash64(ml::util::randomInteger(0, 0xFFFFFF) + rand());
        romName = "unknown";
    }

    void save(const string &filename) const;
    void load(const string &filename);

    void clearImages();

    string romName;
    UINT64 id;
    vector< ReplayFrame* > frames;
};
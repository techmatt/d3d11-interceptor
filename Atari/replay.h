
struct AtariImage
{
    void fromScreen(const ALEScreen &screen);
    void toBmp(const ColourPalette &palette, Bitmap &bmpOut) const;

    Grid2<BYTE> data;
};

struct ReplayFrame
{
    int index;
    Action action;
    int reward;
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

    string romName;
    UINT64 id;
    vector< ReplayFrame* > frames;
};
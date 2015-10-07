
namespace Game
{
    struct ObjectInst;
}

struct LineConstraint
{
    LineConstraint(int _value, bool _vertical, float _weight)
    {
        value = _value;
        vertical = _vertical;
        weight = _weight;
    }
    int value;
    bool vertical;
    float weight;
};

class AtariUtil
{
public:
    static UINT64 animationHash(const set<vec2s> &mask, BYTE color);
    static int maskDiff(const set<vec2s> &a, const set<vec2s> &b);
    static Action actionFromKeyboard();
    static vec4uc getAtariColor(BYTE color, const ColourPalette &palette);
    static Bitmap makeSegmentViz(const ColourPalette &palette, const vector<SegmentAnimation*> &segments);
    static Bitmap makeFrameObjectImage(const SegmentDatabase &segments, const ColourPalette &palette, const ObjectAnalyzer &tracks, int replayIndex, const ReplayFrame &frame);
    static vec4uc randomSignatureColor();

    static void saveStateGraph(const vector<Game::StateInst> &states, const string &filename);

    static double compareVelocityDescriptorDistSingleton(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName, int historyDepth);
    static double compareAnimationDescriptorDistSingleton(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName, int historyDepth);
    static double comparePositionDescriptorDistSingleton(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName, int historyDepth);
    static double compareContactDescriptorDistSingleton(const SegmentDatabase &segments, const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectNameA, const string &objectNameB);
    static double compareOffsetDescriptorDistSingleton(const SegmentDatabase &segments, const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectNameA, const string &objectNameB);

    static double compareLineConstraintsSingleton(const vector<Game::StateInst> &statesA, int frameIndexA, const vector<Game::StateInst> &statesB, int frameIndexB, const string &objectName, const vector<LineConstraint> &lines);
    static double compareActionDescriptorDist(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, int historyDepth);

    static const Game::ObjectInst* findSingleton(const vector<Game::StateInst> &states, int frameIndex, const string &objectName);
    static int computeSingletonOffset(const vector<Game::StateInst> &statesA, int frameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectNameA, const string &objectNameB, int historyDepth);

    static void overlayModelFrame(AppState &state, const Game::StateInst &gameState, Bitmap &bmp);
};

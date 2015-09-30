
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

    static int compareAnimationDescriptorDistSingleton(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName, int historyDepth);
    static int compareActionDescriptorDist(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, int historyDepth);
};


class AtariUtil
{
public:
    static UINT64 animationHash(const set<vec2s> &mask, BYTE color);
    static int maskDiff(const set<vec2s> &a, const set<vec2s> &b);
    static Action actionFromKeyboard();
    static vec4uc getAtariColor(BYTE color, const ColourPalette &palette);
};

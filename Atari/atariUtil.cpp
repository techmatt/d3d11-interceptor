
#include "main.h"

int AtariUtil::maskDiff(const set<vec2s> &a, const set<vec2s> &b)
{
    int diffCount = 0;
    for (vec2s aValue : a)
    {
        if (b.count(aValue) == 0)
            diffCount++;
    }
    for (vec2s bValue : b)
    {
        if (a.count(bValue) == 0)
            diffCount++;
    }
    return diffCount;
}

UINT64 AtariUtil::animationHash(const set<vec2s> &mask, BYTE color)
{
    UINT64 result = util::hash32(color);
    for (vec2s v : mask)
    {
        result += util::hash32(v);
    }
    return result;
}

vec4uc AtariUtil::getAtariColor(BYTE color, const ColourPalette &palette)
{
    int r, g, b;
    palette.getRGB(color, r, g, b);
    return vec4uc(r, g, b, 255);
}

Action AtariUtil::actionFromKeyboard()
{
    bool left = GetAsyncKeyState(VK_LEFT) != 0;
    bool right = GetAsyncKeyState(VK_RIGHT) != 0;
    bool up = GetAsyncKeyState(VK_UP) != 0;
    bool down = GetAsyncKeyState(VK_DOWN) != 0;
    bool fire = GetAsyncKeyState(VK_SPACE) != 0;

    Action a = PLAYER_A_NOOP;

    if (fire) a = PLAYER_A_FIRE;
    if (up) a = PLAYER_A_UP;
    if (right) a = PLAYER_A_RIGHT;
    if (left) a = PLAYER_A_LEFT;
    if (down) a = PLAYER_A_DOWN;
    if (right && up) a = PLAYER_A_UPRIGHT;
    if (left && up) a = PLAYER_A_UPLEFT;
    if (right && down) a = PLAYER_A_DOWNRIGHT;
    if (left && down) a = PLAYER_A_DOWNLEFT;
    if (up && fire) a = PLAYER_A_UPFIRE;
    if (right && fire) a = PLAYER_A_RIGHTFIRE;
    if (left && fire) a = PLAYER_A_LEFTFIRE;
    if (down && fire) a = PLAYER_A_DOWNFIRE;
    if (left && up && fire) a = PLAYER_A_UPLEFTFIRE;
    if (right && up && fire) a = PLAYER_A_UPRIGHTFIRE;
    if (down && right && fire) a = PLAYER_A_DOWNRIGHTFIRE;
    if (left && down && fire) a = PLAYER_A_DOWNLEFTFIRE;
    
    return a;
}

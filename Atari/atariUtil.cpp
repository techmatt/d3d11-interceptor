
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

Bitmap AtariUtil::makeSegmentViz(const ColourPalette &palette, const vector<SegmentAnimation*> &segments)
{
    vec2s maxDimensions;
    for (SegmentAnimation *segment : segments)
        maxDimensions = vec2s(max(maxDimensions.x, segment->dimensions.x), max(maxDimensions.y, segment->dimensions.y));

    int padding = 1;
    vec2s cellDims = maxDimensions + vec2s(padding * 2, padding * 2);

    const int maxImageWidth = 512;

    int XBlocks = min((int)segments.size(), maxImageWidth / cellDims.x);

    const int YBlocks = (int)ceil((int)segments.size() / XBlocks);

    Bitmap bmp(XBlocks * cellDims.x, YBlocks * cellDims.y);
    bmp.setPixels(vec4uc(0, 0, 0, 255));

    int segmentIndex = 0;
    for (int yBlock = 0; yBlock < YBlocks; yBlock++)
    {
        for (int xBlock = 0; xBlock < XBlocks; xBlock++)
        {
            if (segmentIndex >= segments.size())
                continue;

            for (int xRect = 0; xRect < maxDimensions.x; xRect++)
                for (int yRect = 0; yRect < maxDimensions.y; yRect++)
                    bmp(xBlock * cellDims.x + padding + xRect, yBlock * cellDims.y + padding + yRect) = vec4uc(255, 0, 255, 255);

            const vec4uc colorRGB = AtariUtil::getAtariColor(segments[segmentIndex]->color, palette);

            for (vec2s v : segments[segmentIndex++]->mask)
            {
                bmp(xBlock * cellDims.x + padding + v.x, yBlock * cellDims.y + padding + v.y) = colorRGB;
            }
        }
    }

    return bmp;
}

Bitmap AtariUtil::makeFrameObjectImage(const SegmentDatabase &segments, const ColourPalette &palette, const ObjectAnalyzer &tracks, int replayIndex, const ReplayFrame &frame)
{
    Bitmap result;
    frame.image.toBmp(palette, result);
    result.setPixels(vec4uc(0, 0, 0, 255));
    
    for (int objectIndex = 0; objectIndex < frame.objectAnnotations.size(); objectIndex++)
    {
        const ObjectAnnotation &o = frame.objectAnnotations[objectIndex];
        const SegmentAnnotation &segmentAnnotation = frame.segmentAnnotations[o.segments[0]];
        
        const SegmentAnimation *segmentInfo = segments.getSegment(segmentAnnotation.segmentHash);
        
        const ObjectTrack &track = tracks.findObjectTrack(FrameID(replayIndex, frame.index), objectIndex);
        
        //const vec4uc color = segmentInfo->object->colorSignature;
        const vec4uc color = track.colorSignature;

        for (vec2s offset : segmentInfo->mask)
        {
            const vec2s target = offset + segmentAnnotation.origin;
            if (result.isValidCoordinate(target.x, target.y))
            {
                result(target.x, target.y) = color;
            }
        }
        
    }
    return result;
}

vec4uc AtariUtil::randomSignatureColor()
{
    vec3f color = vec3f::origin;
    while (color.length() < 0.5f)
        color = vec3f((float)util::randomUniform(), (float)util::randomUniform(), (float)util::randomUniform());
    return vec4uc(color * 255.0f, 255);
}

void AtariUtil::saveStateGraph(const vector<Game::StateInst> &states, const string &filename)
{
    ofstream file(filename);

    file << "frame,";
    for (auto &o : states[0].variables)
    {
        file << o.first << ',';
    }

    for (auto &o : states[0].objects)
    {
        file << o.first << " count,";
    }

    for (auto &o : states[0].objects)
    {
        file << o.first << " X,";
        file << o.first << " Y,";
    }

    file << endl;

    const int frameCount = (int)states.size();
    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        file << frameIndex << ',';
        auto &state = states[frameIndex];
        for (auto &o : state.variables)
        {
            file << o.second << ',';
        }
        for (auto &o : state.objects)
        {
            file << o.second.size() << ',';
        }

        for (auto &o : state.objects)
        {
            if (o.second.size() == 0)
            {
                file << "0,0,";
            }
            else
            {
                file << o.second[0].origin.toString(",") << ",";
            }
        }

        file << endl;
    }
}

int AtariUtil::compareAnimationDescriptorDistSingleton(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName, int historyDepth)
{
    int sum = 0;
    for (int history = 0; history < historyDepth; history++)
    {
        const Game::StateInst &stateA = statesA[max(0, baseFrameIndexA - history)];
        const Game::StateInst &stateB = statesA[max(0, baseFrameIndexB - history)];
        const vector<Game::ObjectInst> &instancesA = stateA.objects.find(objectName)->second;
        const vector<Game::ObjectInst> &instancesB = stateB.objects.find(objectName)->second;

        if (instancesA.size() == 0 && instancesB.size() == 0)
            continue;

        if ((instancesA.size() == 1 && instancesB.size() == 0) ||
            (instancesA.size() == 0 && instancesB.size() == 1))
        {
            sum++;
            continue;
        }

        if (instancesA[0].segmentHash != instancesB[0].segmentHash)
            sum++;
    }
    return sum;
}

int AtariUtil::compareActionDescriptorDist(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, int historyDepth)
{
    int sum = 0;
    for (int history = 0; history < historyDepth; history++)
    {
        const Game::StateInst &stateA = statesA[max(0, baseFrameIndexA - history)];
        const Game::StateInst &stateB = statesA[max(0, baseFrameIndexB - history)];

        if (stateA.variables.find("action")->second != stateB.variables.find("action")->second)
            sum++;
    }
    return sum;
}

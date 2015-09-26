
#include "main.h"

Bitmap SegmentManager::makeAnimationViz(const ColourPalette &palette, BYTE color)
{
    const vec4uc colorRGB = AtariUtil::getAtariColor(color, palette);

    const vector<SegmentAnimation*> &animations = allAnimations[color];

    vec2s maxDimensions;
    for (SegmentAnimation *animation : animations)
        maxDimensions = vec2s(max(maxDimensions.x, animation->dimensions.x), max(maxDimensions.y, animation->dimensions.y));

    int padding = 1;
    vec2s cellDims = maxDimensions + vec2s(padding * 2, padding * 2);

    const int maxImageWidth = 1024;
    const int XBlocks = max(1, cellDims.x / maxImageWidth);
    const int YBlocks = (int)allAnimations[color].size() / XBlocks + 1;

    Bitmap bmp(XBlocks * cellDims.x, YBlocks * cellDims.y);
    bmp.setPixels(vec4uc(255, 0, 255, 255));

    int animationIndex = 0;
    for (int xBlock = 0; xBlock < XBlocks; xBlock++)
    {
        for (int yBlock = 0; yBlock < YBlocks; yBlock++)
        {
            if (animationIndex >= animations.size())
                continue;
            for (vec2s v : animations[animationIndex++]->mask)
            {
                bmp(xBlock * cellDims.x + padding + v.x, yBlock * cellDims.y + padding + v.y) = colorRGB;
            }
        }
    }

    return bmp;
}

void SegmentManager::saveAllViz(const ColourPalette &palette, const string &dir)
{
    util::makeDirectory(dir);
    for (auto &p : allAnimations)
    {
        if (p.second.size() > 0)
        {
            const Bitmap bmp = makeAnimationViz(palette, p.first);
            LodePNG::save(bmp, dir + to_string(p.first) + ".png");
        }
    }
}

int SegmentManager::maskDiff(const set<vec2s> &a, const set<vec2s> &b)
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

void SegmentManager::recordSegments(const ColourPalette &palette, const ReplayFrame &frame)
{
    Bitmap temp;
    frame.image.toBmp(palette, temp);
    LodePNG::save(temp, "debug3.png");

    scratchpad = frame.image.data;

    set<BYTE> allColors;
    for (auto &p : frame.image.data)
    {
        allColors.insert(p.value);
    }

    for (BYTE color : allColors)
    {
        const vec4uc RGBColor = AtariUtil::getAtariColor(color, palette);
        recordSegments(frame, color);
    }
}

pair<SegmentAnimation*, int> SegmentManager::findClosestMask(const set<vec2s> &mask, BYTE color)
{
    pair<SegmentAnimation*, int> result = pair<SegmentAnimation*, int>(nullptr, 100000);
    for (auto &p : allAnimations[color])
    {
        const int diff = maskDiff(p->mask, mask);
        if (diff < result.second)
            result = make_pair(p, diff);
    }
    return result;
}

void SegmentManager::recordSegments(const ReplayFrame &frame, BYTE color)
{
    scratchpad.setValues(0);
    for (auto &p : frame.image.data)
    {
        if (p.value == color)
        {
            vec2s coord = vec2s((short)p.x, (short)p.y);
            if (scratchpad(coord) == 1)
                continue;

            const set<vec2s> mask = extractMask(frame, coord);

            if (mask.size() == 0)
                continue;

            pair<SegmentAnimation*, int> bestFit = findClosestMask(mask, color);
            if (bestFit.second == 0)
            {
                bestFit.first->count++;
            }
            else
            {
                SegmentAnimation *newAnimation = new SegmentAnimation();
                newAnimation->color = color;
                newAnimation->count = 1;
                newAnimation->mask = mask;
                newAnimation->dimensions = vec2s(0, 0);
                for (vec2s v : mask)
                {
                    newAnimation->dimensions.x = max(newAnimation->dimensions.x, v.x);
                    newAnimation->dimensions.y = max(newAnimation->dimensions.y, v.y);
                }
                newAnimation->dimensions += vec2s(1, 1);
                allAnimations[color].push_back(newAnimation);
            }
        }
    }
}

set<vec2s> SegmentManager::extractMask(const ReplayFrame &frame, const vec2s &seed)
{
    const BYTE color = frame.image.data(seed);

    vector<vec2s> mask;

    vector<vec2s> queue;
    queue.push_back(seed);

    vec2s minCoord(10000, 10000);

    while (queue.size() > 0)
    {
        const vec2s baseCoord = queue.back();
        queue.pop_back();

        if (scratchpad(baseCoord) == 1)
            continue;

        scratchpad(baseCoord) = 1;

        mask.push_back(baseCoord);
        minCoord.x = min(minCoord.x, baseCoord.x);
        minCoord.y = min(minCoord.y, baseCoord.y);

        auto visit = [&](vec2s visitCoord)
        {
            if (!scratchpad.isValidCoordinate(visitCoord.x, visitCoord.y) ||
                scratchpad(visitCoord) == 1 ||
                frame.image.data(visitCoord) != color)
                return;

            queue.push_back(visitCoord);
        };
        visit(baseCoord + vec2s(-1, 0));
        visit(baseCoord + vec2s(1, 0));
        visit(baseCoord + vec2s(0, 1));
        visit(baseCoord + vec2s(0, -1));

        visit(baseCoord + vec2s(-1, -1));
        visit(baseCoord + vec2s(1, -1));
        visit(baseCoord + vec2s(1, 1));
        visit(baseCoord + vec2s(-1, 1));
    }

    set<vec2s> result;
    for (vec2s v : mask)
    {
        vec2s diff = v - minCoord;
        if (max(diff.x, diff.y) >= segmentMaxRadius)
            return set<vec2s>();
        result.insert(diff);
    }
    return result;
}
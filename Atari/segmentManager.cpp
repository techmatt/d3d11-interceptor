
#include "main.h"

void SegmentManager::save(const string &filename) const
{
    BinaryDataStreamFile file(filename, true);

    UINT64 animationCount = animationsByHash.size();
    file << processedReplays << animationCount;

    for (auto &a : animationsByHash)
    {
        file << a.second->hash;
        file << a.second->dimensions;
        file << a.second->color;
        file << a.second->count;
        file << a.second->mask;
    }

    file.closeStream();
}

void SegmentManager::load(const string &filename)
{
    if (!util::fileExists(filename))
    {
        cout << "File not found: " << filename << endl;
        return;
    }

    BinaryDataStreamFile file(filename, false);

    animationsByColor.clear();
    animationsByHash.clear();

    UINT64 animationCount;
    file >> processedReplays >> animationCount;

    for (UINT64 animationIndex = 0; animationIndex < animationCount; animationIndex++)
    {
        SegmentAnimation *newAnimation = new SegmentAnimation();
        file >> newAnimation->hash;
        file >> newAnimation->dimensions;
        file >> newAnimation->color;
        file >> newAnimation->count;
        file >> newAnimation->mask;
        animationsByColor[newAnimation->color].push_back(newAnimation);
        animationsByHash[newAnimation->hash] = newAnimation;
    }

    file.closeStream();
}

Bitmap SegmentManager::makeAnimationViz(const ColourPalette &palette, BYTE color)
{
    const vec4uc colorRGB = AtariUtil::getAtariColor(color, palette);

    const vector<SegmentAnimation*> &animations = animationsByColor[color];

    vec2s maxDimensions;
    for (SegmentAnimation *animation : animations)
        maxDimensions = vec2s(max(maxDimensions.x, animation->dimensions.x), max(maxDimensions.y, animation->dimensions.y));

    int padding = 1;
    vec2s cellDims = maxDimensions + vec2s(padding * 2, padding * 2);

    const int maxImageWidth = 512;

    int XBlocks = min((int)animations.size(), maxImageWidth / cellDims.x);
    
    const int YBlocks = (int)ceil((int)animations.size() / XBlocks);

    Bitmap bmp(XBlocks * cellDims.x, YBlocks * cellDims.y);
    bmp.setPixels(vec4uc(0, 0, 0, 255));

    int animationIndex = 0;
    for (int yBlock = 0; yBlock < YBlocks; yBlock++)
    {
        for (int xBlock = 0; xBlock < XBlocks; xBlock++)
        {
            if (animationIndex >= animations.size())
                continue;

            for (int xRect = 0; xRect < maxDimensions.x; xRect++)
                for (int yRect = 0; yRect < maxDimensions.y; yRect++)
                    bmp(xBlock * cellDims.x + padding + xRect, yBlock * cellDims.y + padding + yRect) = vec4uc(255, 0, 255, 255);

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
    cout << "Saving all visualiations to " << dir << endl;
    util::makeDirectory(dir);
    for (auto &p : animationsByColor)
    {
        if (p.second.size() > 0)
        {
            const Bitmap bmp = makeAnimationViz(palette, p.first);
            LodePNG::save(bmp, dir + to_string(p.first) + ".png");
        }
    }
}

void SegmentManager::recordSegments(const ColourPalette &palette, const ReplayFrame &frame)
{
    //Bitmap temp;
    //frame.image.toBmp(palette, temp);
    //LodePNG::save(temp, "debug3.png");

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

SegmentAnimation* SegmentManager::findExactMask(const set<vec2s> &mask, BYTE color)
{
    const UINT64 hash = AtariUtil::animationHash(mask, color);
    if (animationsByHash.count(hash) == 0)
        return nullptr;
    return animationsByHash.find(hash)->second;
}

pair<SegmentAnimation*, int> SegmentManager::findClosestMask(const set<vec2s> &mask, BYTE color)
{
    pair<SegmentAnimation*, int> result = pair<SegmentAnimation*, int>(nullptr, 100000);
    for (auto &p : animationsByColor[color])
    {
        const int diff = AtariUtil::maskDiff(p->mask, mask);
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

            //pair<SegmentAnimation*, int> bestFit = findClosestMask(mask, color);
            SegmentAnimation* match = findExactMask(mask, color);
            if (match != nullptr)
            {
                match->count++;
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
                newAnimation->hash = AtariUtil::animationHash(mask, color);

                animationsByColor[color].push_back(newAnimation);
                animationsByHash[newAnimation->hash] = newAnimation;
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
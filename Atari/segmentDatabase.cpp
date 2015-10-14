
#include "main.h"

void SegmentDatabase::init()
{
    for (const string &s : util::getFileLinesCreate(learningParams().ROMDatasetDir + "colorBlacklist.txt", 3))
    {
        auto parts = util::split(s, ' ');
        if (parts.size() == 3)
        {
            colorBlacklist.push_back(vec4uc(convert::toInt(parts[0]), convert::toInt(parts[1]), convert::toInt(parts[2]), 255));
        }
    }

    const string filename = learningParams().ROMDatasetDir + "blacklistedSegments.dat";
    if (util::fileExists(filename))
    {
        vector<UINT64> segmentBlacklistVec;
        BinaryDataStreamFile file(filename, false);
        file.readPrimitiveVector(segmentBlacklistVec);
        file.closeStream();

        cout << "Blacklisted segments: " << endl;
        for (UINT64 s : segmentBlacklistVec)
        {
            cout << s << ",";
            segmentBlacklist.insert(s);
        }
        cout << endl;
    }
    
    const string blacklistImageFile = learningParams().ROMDatasetDir + "blacklist.png";
    if (util::fileExists(blacklistImageFile))
    {
        blacklistImage = LodePNG::load(blacklistImageFile);
        //cout << "Blacklist image loaded " << blacklistImage(0, 0).toString(",") << endl;
    }
}

void SegmentDatabase::save(const string &filename) const
{
    BinaryDataStreamFile file(filename, true);

    UINT64 animationCount = segmentsByHash.size();
    file << processedReplays << animationCount;

    for (auto &a : segmentsByHash)
    {
        file << a.second->hash;
        file << a.second->dimensions;
        file << a.second->color;
        file << a.second->count;
        file << a.second->mask;
    }

    file.closeStream();
}

void SegmentDatabase::load(const string &filename)
{
    if (!util::fileExists(filename))
    {
        cout << "File not found: " << filename << endl;
        return;
    }

    BinaryDataStreamFile file(filename, false);

    segmentsByColor.clear();
    segmentsByHash.clear();

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
        segmentsByColor[newAnimation->color].push_back(newAnimation);
        segmentsByHash[newAnimation->hash] = newAnimation;
        newAnimation->index = (int)animationIndex;
    }

    file.closeStream();
}

void SegmentDatabase::saveVizColors(const ColourPalette &palette, const string &dir)
{
    cout << "Saving visualiation by color to " << dir << endl;
    util::makeDirectory(dir);
    for (auto &p : segmentsByColor)
    {
        if (p.second.size() > 0)
        {
            const Bitmap bmp = AtariUtil::makeSegmentViz(palette, p.second);
            LodePNG::save(bmp, dir + to_string(p.first) + ".png");
        }
    }
}

void SegmentDatabase::saveVizObjects(const ColourPalette &palette, const string &dir)
{
    cout << "Saving visualiation by object to " << dir << endl;
    util::makeDirectory(dir);
    for (GameObject *o : objects)
    {
        const Bitmap bmp = AtariUtil::makeSegmentViz(palette, o->segments);
        LodePNG::save(bmp, dir + to_string(o->index) + ".png");
    }
}

void SegmentDatabase::recordAndAnnotateSegments(const ColourPalette &palette, ReplayFrame &frame)
{
    frame.segmentAnnotations.clear();
    frame.objectAnnotations.clear();

    scratchpad = frame.image.data;
    scratchpad.setValues(0);

    for (auto &p : blacklistImage)
    {
        if (p.value == vec4uc(255, 0, 255, 255))
        {
            frame.image.data(p.x, p.y) = 1;
        }
    }

    set<BYTE> allColors;
    for (auto &p : frame.image.data)
    {
        allColors.insert(p.value);
    }

    //Bitmap temp;
    //frame.image.toBmp(palette, temp);
    //LodePNG::save(temp, "debug3.png");

    for (BYTE color : allColors)
    {
        const vec4uc RGBColor = AtariUtil::getAtariColor(color, palette);
        if (util::contains(colorBlacklist, RGBColor))
            continue;

        recordAndAnnotateSegments(frame, color);
    }
}

SegmentAnimation* SegmentDatabase::findExactMask(UINT64 segmentHash)
{
    if (segmentsByHash.count(segmentHash) == 0)
        return nullptr;
    return segmentsByHash.find(segmentHash)->second;
}

SegmentAnimation* SegmentDatabase::findExactMask(const set<vec2s> &mask, BYTE color)
{
    const UINT64 hash = AtariUtil::animationHash(mask, color);
    return findExactMask(hash);
}

pair<SegmentAnimation*, int> SegmentDatabase::findClosestMask(const set<vec2s> &mask, BYTE color)
{
    pair<SegmentAnimation*, int> result = pair<SegmentAnimation*, int>(nullptr, 100000);
    for (auto &p : segmentsByColor[color])
    {
        const int diff = AtariUtil::maskDiff(p->mask, mask);
        if (diff < result.second)
            result = make_pair(p, diff);
    }
    return result;
}

void SegmentDatabase::recordAndAnnotateSegments(ReplayFrame &frame, BYTE color)
{
    for (auto &p : frame.image.data)
    {
        if (p.value == color)
        {
            vec2s coord = vec2s((short)p.x, (short)p.y);
            if (scratchpad(coord) == 1)
                continue;

            vec2s maskOrigin;
            const set<vec2s> mask = extractMask(frame, coord, maskOrigin);

            if (mask.size() < learningParams().minMaskSize)
                continue;

            //pair<SegmentAnimation*, int> bestFit = findClosestMask(mask, color);
            const UINT64 hash = AtariUtil::animationHash(mask, color);
            if (segmentBlacklist.count(hash) > 0)
                continue;

            SegmentAnimation* match = findExactMask(hash);
            if (match == nullptr)
            {
                SegmentAnimation *newAnimation = new SegmentAnimation();
                newAnimation->color = color;
                newAnimation->count = 0;
                newAnimation->mask = mask;
                newAnimation->dimensions = vec2s(0, 0);
                for (vec2s v : mask)
                {
                    newAnimation->dimensions.x = max(newAnimation->dimensions.x, v.x);
                    newAnimation->dimensions.y = max(newAnimation->dimensions.y, v.y);
                }
                newAnimation->dimensions += vec2s(1, 1);
                newAnimation->hash = hash;

                segmentsByColor[color].push_back(newAnimation);
                segmentsByHash[newAnimation->hash] = newAnimation;

                match = newAnimation;
            }

            match->count++;

            frame.segmentAnnotations.push_back(SegmentAnnotation(maskOrigin, match->hash));
        }
    }
}

set<vec2s> SegmentDatabase::extractMask(const ReplayFrame &frame, const vec2s &seed, vec2s &maskOriginOut)
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

    maskOriginOut = minCoord;

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
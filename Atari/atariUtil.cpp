
#include "main.h"

string AtariUtil::toString(Action action)
{
    return action_to_string(action);
}

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
    /*for (auto &o : states[0].variables)
    {
        file << o.first << ',';
    }*/

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
        /*for (auto &o : state.variables)
        {
            file << o.second << ',';
        }*/
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

double AtariUtil::compareAnimationDescriptorDistSingleton(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName, int historyDepth)
{
    int sum = 0;
    for (int history = 0; history < historyDepth; history++)
    {
        const Game::StateInst &stateA = statesA[max(0, baseFrameIndexA - history)];
        const Game::StateInst &stateB = statesB[max(0, baseFrameIndexB - history)];
        const vector<Game::ObjectInst> &instancesA = stateA.getInstances(objectName);
        const vector<Game::ObjectInst> &instancesB = stateB.getInstances(objectName);

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

double AtariUtil::compareActionDescriptorDist(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, int historyDepth)
{
    int sum = 0;

    for (int history = 0; history < historyDepth; history++)
    {
        const Game::StateInst &stateA = statesA[max(0, baseFrameIndexA - history)];
        const Game::StateInst &stateB = statesB[max(0, baseFrameIndexB - history)];

        if (stateA.action != stateB.action)
            sum++;
    }
    return sum;
}


const Game::ObjectInst* AtariUtil::findSingleton(const Game::StateInst &state, const string &objectName)
{
    const vector<Game::ObjectInst> &instances = state.getInstances(objectName);
    if (instances.size() == 0)
        return nullptr;
    else
        return &instances[0];
}

const Game::ObjectInst* AtariUtil::findSingleton(const vector<Game::StateInst> &states, int frameIndex, const string &objectName)
{
    const Game::StateInst &state = states[max(0, frameIndex)];
    const vector<Game::ObjectInst> &instances = state.getInstances(objectName);
    if (instances.size() == 0)
        return nullptr;
    else
        return &instances[0];
}

double AtariUtil::compareOffsetDescriptorDistSingleton(const SegmentDatabase &segments, const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName1, const string &objectName2)
{
    const Game::ObjectInst *instA1 = findSingleton(statesA, baseFrameIndexA, objectName1);
    const Game::ObjectInst *instA2 = findSingleton(statesA, baseFrameIndexA, objectName2);

    const Game::ObjectInst *instB1 = findSingleton(statesB, baseFrameIndexB, objectName1);
    const Game::ObjectInst *instB2 = findSingleton(statesB, baseFrameIndexB, objectName2);

    if (instA1 == nullptr || instA2 == nullptr || instB1 == nullptr || instB2 == nullptr)
        return 1000.0;

    const bbox2f bboxA1 = instA1->bbox(segments);
    const bbox2f bboxA2 = instA2->bbox(segments);
    const bbox2f bboxB1 = instB1->bbox(segments);
    const bbox2f bboxB2 = instB2->bbox(segments);

    const vec2i diffA = bboxA2.getCenter() - bboxA1.getCenter();
    const vec2i diffB = bboxB2.getCenter() - bboxB1.getCenter();

    const double distA = bboxDist(bboxA1, bboxA2);
    const double distB = bboxDist(bboxB1, bboxB2);

    if (distA > learningParams().maxProximityDist && distB > learningParams().maxProximityDist)
        return 0.0;

    if (max(distA, distB) > learningParams().maxProximityDist)
        return 1000.0;

    const vec2i offsetDiff = diffB - diffA;

    double sum = 0.0;
    sum += math::abs(distA - distB);
    sum += math::abs(offsetDiff.x);
    sum += math::abs(offsetDiff.y);
    return sum;
}

float AtariUtil::bboxDist(const bbox2f &a, const bbox2f &b)
{
    const vec2f diff = a.getCenter() - b.getCenter();
    const vec2f variance = (a.getExtent() + b.getExtent()) / 2;
    const float distX = max(0.0f, abs(diff.x) - variance.x);
    const float distY = max(0.0f, abs(diff.y) - variance.y);
    return max(distX, distY);
}

bool AtariUtil::objectsInContactSingleton(const SegmentDatabase &segments, const Game::StateInst &state, const string &objectNameA, const string &objectNameB)
{
    const Game::ObjectInst *instA = findSingleton(state, objectNameA);
    const Game::ObjectInst *instB = findSingleton(state, objectNameB);

    if (instA == nullptr || instB == nullptr)
        return false;

    const bbox2f bboxA = instA->bbox(segments);
    const bbox2f bboxB = instB->bbox(segments);
    
    const float dist = bboxDist(bboxA, bboxB);
    
    return (dist <= learningParams().maxProximityDist);
}

double AtariUtil::compareContactDescriptorDistSingleton(const SegmentDatabase &segments, const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName1, const string &objectName2)
{
    const Game::ObjectInst *instA1 = findSingleton(statesA, baseFrameIndexA, objectName1);
    const Game::ObjectInst *instA2 = findSingleton(statesA, baseFrameIndexA, objectName2);

    const Game::ObjectInst *instB1 = findSingleton(statesB, baseFrameIndexB, objectName1);
    const Game::ObjectInst *instB2 = findSingleton(statesB, baseFrameIndexB, objectName2);

    if (instA1 == nullptr || instA2 == nullptr || instB1 == nullptr || instB2 == nullptr)
        return 1000.0;

    const bbox2f bboxA1 = instA1->bbox(segments);
    const bbox2f bboxA2 = instA2->bbox(segments);
    const bbox2f bboxB1 = instB1->bbox(segments);
    const bbox2f bboxB2 = instB2->bbox(segments);

    const float distA = bboxDist(bboxA1, bboxA2);
    const float distB = bboxDist(bboxB1, bboxB2);

    if (distA > learningParams().maxProximityDist && distB > learningParams().maxProximityDist)
        return 0.0;

    if (max(distA, distB) > learningParams().maxProximityDist)
        return 1.0;

    return 0.0;
}

double AtariUtil::compareLineConstraintsSingleton(const vector<Game::StateInst> &statesA, int frameIndexA, const vector<Game::StateInst> &statesB, int frameIndexB, const string &objectName, const vector<LineConstraint> &lines)
{
    const Game::ObjectInst *instA = findSingleton(statesA, frameIndexA, objectName);
    const Game::ObjectInst *instB = findSingleton(statesB, frameIndexB, objectName);

    if (instA == nullptr && instB == nullptr)
        return 0.0f;
    if (instA == nullptr || instB == nullptr)
        return 1000.0f;

    double sum = 0.0;
    for (const LineConstraint &l : lines)
    {
        const bool aOnLine = l.onLine(instA->origin);
        const bool bOnLine = l.onLine(instB->origin);
        if (aOnLine != bOnLine)
            sum += l.weight;
    }
    return sum;
}

double AtariUtil::compareVelocityDescriptorDistSingleton(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName, int historyDepth)
{
    auto getVelocity = [&](const vector<Game::StateInst> &states, int frame, bool &alive)
    {
        const Game::StateInst &state = states[max(0, frame)];
        const Game::StateInst &stateP = states[max(0, frame - 1)];
        const vector<Game::ObjectInst> &instances = state.getInstances(objectName);
        const vector<Game::ObjectInst> &instancesP = stateP.getInstances(objectName);

        if (instances.size() == 0 || instancesP.size() == 0)
        {
            alive = false;
            return vec2s(0, 0);
        }

        alive = true;
        return instances[0].origin - instancesP[0].origin;
    };

    int sum = 0;
    for (int history = 0; history < historyDepth; history++)
    {
        bool aliveA, aliveB;
        const vec2s velA = getVelocity(statesA, baseFrameIndexA - history, aliveA);
        const vec2s velB = getVelocity(statesB, baseFrameIndexB - history, aliveB);

        if (!aliveA && !aliveB)
            continue;

        if (!aliveA || !aliveB)
        {
            sum += 20;
            continue;
        }

        const vec2s diff = velB - velA;
        sum += math::abs(diff.x);
        sum += math::abs(diff.y);
    }
    return sum;
}

double AtariUtil::comparePositionDescriptorDistSingleton(const vector<Game::StateInst> &statesA, int baseFrameIndexA, const vector<Game::StateInst> &statesB, int baseFrameIndexB, const string &objectName, int historyDepth)
{
    int sum = 0;
    for (int history = 0; history < historyDepth; history++)
    {
        const Game::StateInst &stateA = statesA[max(0, baseFrameIndexA - history)];
        const Game::StateInst &stateB = statesB[max(0, baseFrameIndexB - history)];
        const vector<Game::ObjectInst> &instancesA = stateA.getInstances(objectName);
        const vector<Game::ObjectInst> &instancesB = stateB.getInstances(objectName);

        if (instancesA.size() == 0 && instancesB.size() == 0)
            continue;

        if ((instancesA.size() == 1 && instancesB.size() == 0) ||
            (instancesA.size() == 0 && instancesB.size() == 1))
        {
            sum += 20;
            continue;
        }

        vec2s diff = instancesA[0].origin - instancesB[0].origin;
        sum += min(10, math::abs((int)diff.x));
        sum += min(10, math::abs((int)diff.y));
    }
    return sum;
}

void AtariUtil::overlayModelFrame(AppState &state, const Game::StateInst &gameState, Bitmap &bmp)
{
    for (auto &o : gameState.objects)
    {
        for (auto &inst : o.second)
        {
            SegmentAnimation *animation = state.segmentDatabase.getSegment(inst.segmentHash);
            
            vec4uc objColor;
            
            set<vec2s> mask;
            if (animation == nullptr)
            {
                objColor = vec4uc(255, 0, 255, 255);
                mask.insert(vec2s(0, 0));
                mask.insert(vec2s(1, 0));
                mask.insert(vec2s(0, 1));
                mask.insert(vec2s(1, 1));
            }
            else
            {
                objColor = AtariUtil::getAtariColor(animation->color, state.getPalette());
                mask = animation->mask;
            }

            for (vec2s v : mask)
            {
                vec2s pos = inst.origin + v;
                if (bmp.isValidCoordinate(pos.x, pos.y))
                {
                    vec4uc &curColor = bmp(pos.x, pos.y);
                    if (curColor == objColor)
                    {
                        curColor = vec4uc(255, 128, 0, 255);
                    }
                    else
                    {
                        curColor = vec4uc((vec4f(curColor) + vec4f(objColor)) * 0.5f);
                    }
                }
            }
        }
    }
}

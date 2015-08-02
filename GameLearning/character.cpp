
#include "main.h"

void CharacterDatabase::init(SegmentAnalyzer &analyzer)
{
    characters.resize(analyzer.characterSegments.size());
    for (size_t characterIndex = 0; characterIndex < characters.size(); characterIndex++)
    {
        characters[characterIndex].init(analyzer.characterSegments[characterIndex], (int)characterIndex);
    }
}

void CharacterDatabase::recordAllFrames(const ReplayDatabase &frames)
{
    for (Character &c : characters)
    {
        c.recordAllFrames(frames);
    }
}

void Character::init(const vector<UINT64> &segments, int _characterIndex)
{
    characterIndex = _characterIndex;

    for (const UINT64 &segment : segments)
        allSegments.insert(segment);
}


void Character::recordAllFrames(const ReplayDatabase &frames)
{
    for (const ReplayDatabaseEntry &entry : frames.entries)
    {
        cout << "Recording animations for character " << characterIndex << " in " << entry.replay->sourceFilename << endl;
        for (const auto &frame : entry.processedFrames)
        {
            recordFrameAnimation(frame);
        }

        for (const auto &pair : entry.pairs)
        {
            recordFrameTransition(pair);
        }
    }

    cout << "Instance count = " << allInstances.size() << endl;

    ofstream fileA("logs/transitionTo.txt");
    for (auto &c : clusters)
    {
        fileA << c.index << " (" << c.observations << ")";
        for (auto &i : c.transitionsFrom)
        {
            fileA << '\t' << i.first << " (" << i.second.frameCount << ")";
        }
        fileA << endl;
    }

    ofstream fileB("logs/transitionFrom.txt");
    for (auto &c : clusters)
    {
        fileB << c.index << " (" << c.observations << ")";
        for (auto &i : c.transitionsTo)
        {
            fileB << '\t' << i.first << " (" << i.second.frameCount << ")";
        }
        fileB << endl;
    }
}

void Character::recordFrameAnimation(const ProcessedFrame &frame)
{
    CharacterFrameInstance frameInstance;
    
    vec3f sum = vec3f::origin;
    float sumCount = 0.0f;

    for (UINT64 signature : allSegments)
    {
        if (frame.signatureMap.count(signature) > 0)
        {
            CharacterSegmentInstance &segmentInstance = frameInstance.segments[signature];
            segmentInstance.signature = signature;
            segmentInstance.worldCentroid = frame.signatureMap.find(signature)->second.front()->centroid;
            sum += segmentInstance.worldCentroid;
            sumCount += 1.0f;
        }
    }

    if (sumCount > 0.0f)
    {
        sum /= sumCount;
        for (auto &segmentInstance : frameInstance.segments)
        {
            segmentInstance.second.centeredCentroid = segmentInstance.second.worldCentroid - sum;
        }
        updateClusters(frameInstance);
        allInstances[frame.frameID] = frameInstance;
    }
}

void Character::updateClusters(CharacterFrameInstance &newInstance)
{
    //
    // check if the instance belongs to an existing animation cluster
    //
    for (AnimationCluster &cluster : clusters)
    {
        const double dist = frameInstanceDistSqAvg(cluster.seedInstance, newInstance);
        if (dist < learningParams().animationClusterDistThreshold)
        {
            cluster.observations++;
            newInstance.animationClusterIndex = cluster.index;
            return;
        }
    }

    //
    // no valid cluster found; create a new one
    //
    AnimationCluster newCluster;
    newCluster.index = (int)clusters.size();
    newInstance.animationClusterIndex = newCluster.index;
    newCluster.observations = 1;
    newCluster.seedInstance = newInstance;
    clusters.push_back(newCluster);
}

void Character::recordFrameTransition(const FramePair &pair)
{
    const CharacterFrameInstance* instance0 = findInstanceAtFrame(pair.f0->frameID);
    const CharacterFrameInstance* instance1 = findInstanceAtFrame(pair.f1->frameID);

    if (instance0 != nullptr && instance1 != nullptr)
    {
        int cluster0 = instance0->animationClusterIndex;
        int cluster1 = instance1->animationClusterIndex;

        clusters[cluster1].transitionsFrom[cluster0].frameCount++;
        clusters[cluster0].transitionsTo[cluster1].frameCount++;
    }
}

double Character::frameInstanceDistSqMax(const CharacterFrameInstance &a, const CharacterFrameInstance &b)
{
    if (a.segments.size() != b.segments.size()) return std::numeric_limits<double>::max();

    double result = 0.0;
    for (auto &segA : a.segments)
    {
        if (b.segments.count(segA.first) == 0) return std::numeric_limits<double>::max();
        
        const vec3f &aCentroid = segA.second.centeredCentroid;
        const vec3f &bCentroid = b.segments.find(segA.first)->second.centeredCentroid;
        const double diff = vec3f::distSq(aCentroid, bCentroid);
        result += diff;
    }
    return result;
}

double Character::frameInstanceDistSqAvg(const CharacterFrameInstance &a, const CharacterFrameInstance &b)
{
    if (a.segments.size() != b.segments.size()) return std::numeric_limits<double>::max();

    double result = 0.0;
    for (auto &segA : a.segments)
    {
        if (b.segments.count(segA.first) == 0) return std::numeric_limits<double>::max();

        const vec3f &aCentroid = segA.second.centeredCentroid;
        const vec3f &bCentroid = b.segments.find(segA.first)->second.centeredCentroid;
        const double diff = vec3f::distSq(aCentroid, bCentroid);
        result = max(result, diff);
    }
    return result / a.segments.size();
}

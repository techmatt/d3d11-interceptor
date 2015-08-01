
#include "main.h"

void Character::load(const FrameCollection &frames, const vector<UINT64> &segments, int _characterIndex)
{
    characterIndex = _characterIndex;
    cout << endl << "Processing all frames for character " << characterIndex << endl;
    
    for (const UINT64 &segment : segments)
        allSegments.insert(segment);

    for (const auto &frame : frames.frames)
    {
        map<UINT64, const LocalizedObjectData*> frameMap;
        for (auto &object : frame->objectData)
        {
            if (allSegments.count(object.signature) > 0)
            {
                frameMap[object.signature] = &object;
            }
        }

        CharacterFrameInstance frameInstance;
        frameInstance.frameIndex = frame->index;

        vec3f sum = vec3f::origin;
        float sumCount = 0.0f;

        for (UINT64 signature : allSegments)
        {
            if (frameMap.count(signature) > 0)
            {
                CharacterSegmentInstance &segmentInstance = frameInstance.segments[signature];
                segmentInstance.signature = signature;
                segmentInstance.worldCentroid = frameMap.find(signature)->second->centroid;
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
            allInstances[frame->index] = frameInstance;
        }
    }

    cout << "Instance count = " << allInstances.size() << endl;

    clusterAnimations();
    makeTransitionTables(frames);
}

void Character::makeTransitionTables(const FrameCollection &frames)
{
    cout << "generating transition tables..." << endl;
    for (int frameIndex = 0; frameIndex < frames.frames.size(); frameIndex++)
    {
        if (allInstances.count(frameIndex - 1) != 0 && allInstances.count(frameIndex) != 0 && allInstances.count(frameIndex + 1) != 0)
        {
            int prevClusterIndex = allInstances.find(frameIndex - 1)->second.animationClusterIndex;
            int curClusterIndex =  allInstances.find(frameIndex + 0)->second.animationClusterIndex;
            int nextClusterIndex = allInstances.find(frameIndex + 1)->second.animationClusterIndex;

            clusters[curClusterIndex].transitionsFrom[prevClusterIndex].frameCount++;
            clusters[curClusterIndex].transitionsTo[nextClusterIndex].frameCount++;
        }
    }

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

void Character::clusterAnimations()
{
    cout << "Clustering animations" << endl;

    set<CharacterFrameInstance*> unassignedInstances;

    for (auto &instance : allInstances)
        unassignedInstances.insert(&instance.second);

    while (unassignedInstances.size() > 0)
    {
        CharacterFrameInstance *seedInstance;
        seedInstance = *unassignedInstances.begin();

        vector<CharacterFrameInstance*> clusterInstances;
        for (auto &candidateInstance : unassignedInstances)
        {
            const double dist = frameInstanceDistSqAvg(*seedInstance, *candidateInstance);
            if (dist < learningParams().animationClusterDistThreshold)
            {
                candidateInstance->animationClusterIndex = (int)clusters.size();
                clusterInstances.push_back(candidateInstance);
            }
        }

        for (auto &instance : clusterInstances)
        {
            unassignedInstances.erase(instance);
        }

        AnimationCluster cluster;
        cluster.index = (int)clusters.size();
        cluster.seedFrameIndex = seedInstance->frameIndex;
        cluster.observations = (int)clusterInstances.size();
        clusters.push_back(cluster);
    }

    cout << "Animations cluster count: " << clusters.size() << endl;
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

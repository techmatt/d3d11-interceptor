
#include "main.h"

void Character::load(const FrameCollection &frames, const vector<CharacterSegment> &segments, int _characterIndex)
{
    characterIndex = _characterIndex;
    cout << endl << "Processing all frames for character " << characterIndex << endl;
    
    for (const CharacterSegment &segment : segments)
    {
        allSegments[segment.signature] = segment;
    }

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

        for (auto &segment : allSegments)
        {
            if (frameMap.count(segment.first) > 0)
            {
                CharacterSegmentInstance &segmentInstance = frameInstance.segments[segment.first];
                segmentInstance.signature = segment.first;
                segmentInstance.worldCentroid = frameMap.find(segment.first)->second->centroid;
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
            allInstances.push_back(frameInstance);
        }
    }

    cout << "Instance count = " << allInstances.size() << endl;

    clusterAnimations();
}

void Character::clusterAnimations()
{
    const double clusterDistThreshold = 1.0;

    cout << "Clustering animations" << endl;

    set<CharacterFrameInstance*> unassignedInstances;

    for (auto &instance : allInstances)
        unassignedInstances.insert(&instance);

    //ofstream file("clusters.txt");

    animationClusterCount = 0;
    while (unassignedInstances.size() > 0)
    {
        CharacterFrameInstance *seedInstance;
        seedInstance = *unassignedInstances.begin();

        for (auto &candidateInstance : unassignedInstances)
        {
            const double dist = frameInstanceDistL2(*seedInstance, *candidateInstance);
            if (dist < clusterDistThreshold)
            {
                candidateInstance->animationClusterIndex = animationClusterCount;
                unassignedInstances.erase(candidateInstance);
            }
        }

        animationClusterCount++;
    }

    cout << "Animations cluster count: " << animationClusterCount << endl;
}

double Character::frameInstanceDistL2(const CharacterFrameInstance &a, const CharacterFrameInstance &b)
{
    if (a.segments.size() != b.segments.size()) return std::numeric_limits<double>::max();

    double result = 0.0;
    for (auto &segA : a.segments)
    {
        if (b.segments.count(segA.first) == 0) return std::numeric_limits<double>::max();
        
        const vec3f &aCentroid = segA.second.centeredCentroid;
        const vec3f &bCentroid = b.segments.find(segA.first)->second.centeredCentroid;
        const double diff = vec3f::dist(aCentroid, bCentroid);
        result += diff;
    }
    return result;
}

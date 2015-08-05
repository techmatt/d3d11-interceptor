
#include "main.h"

void CharacterFrameInstance::makeRawPoseDescriptor(const vector<UINT64> &segmentList, float *result) const
{
    size_t resultIndex = 0;
    for (UINT64 signature : segmentList)
    {
        auto it = segments.find(signature);
        if (it == segments.end())
        {
            for (int i = 0; i < 3; i++)
                result[resultIndex++] = 0.0f;
        }
        else
        {
            result[resultIndex++] = it->second.centeredCentroid.x;
            result[resultIndex++] = it->second.centeredCentroid.y;
            result[resultIndex++] = it->second.centeredCentroid.z;
        }
    }
}

void Character::init(const vector<UINT64> &segments, int _characterIndex)
{
    characterIndex = _characterIndex;

    allSegmentsVec = segments;
    for (UINT64 segment : segments)
        allSegmentsSet.insert(segment);
}

void Character::recordAllFrames(const ReplayDatabase &frames)
{
    for (const ReplayDatabaseEntry &entry : frames.entries)
    {
        cout << "Recording poses for character " << characterIndex << " in " << entry.replay->sourceFilename << endl;
        for (const auto &frame : entry.processedFrames)
        {
            recordFramePoses(frame);
        }
    }

    computePosePCA();

    computeAnimationSequences();
}

void Character::computePosePCA()
{
    const int posePCASamples = 4000;
    const int poseFeatureCount = 3 * (int)allSegmentsVec.size();

    vector<CharacterFrameInstance*> instancesVec;
    for (auto &instance : allInstances)
        instancesVec.push_back(&instance.second);

    DenseMatrixf M(posePCASamples, poseFeatureCount);

    for (int sampleIndex = 0; sampleIndex < posePCASamples; sampleIndex++)
    {
        const CharacterFrameInstance &instance = *util::randomElement(instancesVec);
        instance.makeRawPoseDescriptor(allSegmentsVec, &M(sampleIndex, 0));
    }

    posePCA.init(M);
}

void Character::recordFramePoses(const ProcessedFrame &frame)
{
    CharacterFrameInstance frameInstance;
    
    vec3f sum = vec3f::origin;
    float sumCount = 0.0f;

    for (UINT64 signature : allSegmentsVec)
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
        frameInstance.frameID = frame.frameID;
        sum /= sumCount;
        for (auto &segmentInstance : frameInstance.segments)
        {
            segmentInstance.second.centeredCentroid = segmentInstance.second.worldCentroid - sum;
        }
        allInstances[frame.frameID] = frameInstance;
    }
}

void Character::computeAnimationSequences()
{
    cout << "*** Computing animation sequences" << endl;


    cout << "Animation count: " << sequences.size() << endl;
    
    //updateFirstPoseMap();
}

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

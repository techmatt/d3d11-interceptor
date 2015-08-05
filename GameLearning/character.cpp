
#include "main.h"

void CharacterFrameInstance::makePoseDescriptor(const vector<UINT64> &segmentList, vector<float> &result)
{
    PCAf test;
    vector<const float*> values;
    test.init(values, 10);
    test.reducedDimension(0.5);

    const size_t segmentCount = segmentList.size();
    if (result.size() != segmentCount * 3)
        result.resize(segmentCount);

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

    allSegmentsVec = segments;
    for (UINT64 segment : segments)
        allSegmentsSet.insert(segment);
}

void Character::labelAnimationInstances()
{
    /*cout << "Labeling animation instances..." << endl;
    for (auto &startInstance : allInstances)
    {
        for (PoseCluster *pose : startInstance.second.poseClusters)
        {
            if (sequencesByFirstPose.count(pose->index) > 0)
            {
                for (auto &sequenceCandidate : sequencesByFirstPose.find(pose->index)->second)
                {
                    if (animationAtFrame(*sequenceCandidate, startInstance.first))
                    {
                        sequenceCandidate->instances.push_back(startInstance.first);
                        for (int frameOffset = 0; frameOffset < sequenceCandidate->poses.size(); frameOffset++)
                        {
                            CharacterFrameInstance* frameInstance = findInstanceAtFrame(FrameID(startInstance.first.replayIndex, startInstance.first.frameIndex + frameOffset));
                            frameInstance->sequences.push_back(sequenceCandidate);
                        }
                        break;
                    }
                }
            }
        }
    }

    for (auto &sequences : sequences)
    {
        cout << "Sequence frames = " << sequences.poses.size() << ", instances = " << sequences.instances.size() << endl;
    }*/
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

    cout << "Assigning instances to clusters..." << endl;
    for (auto &instance : allInstances)
    {
        assignClusters(instance.second);
    }

    for (const ReplayDatabaseEntry &entry : frames.entries)
    {
        cout << "Recording transitions for character " << characterIndex << " in " << entry.replay->sourceFilename << endl;
        for (const auto &pair : entry.pairs)
        {
            recordFrameTransition(pair);
        }
    }

    cout << "Instance count = " << allInstances.size() << endl;
    cout << "Cluster count = " << poseClusters.size() << endl;

    computeAnimationSequences();
    //labelAnimationInstances();

    ofstream fileA("logs/transitionFrom.txt");
    for (auto &c : poseClusters)
    {
        fileA << c.index << " (" << c.observations.size() << ")";
        for (auto &i : c.transitionsFrom)
        {
            fileA << '\t' << i.first << " (" << i.second.frameCount << ")";
        }
        fileA << endl;
    }

    ofstream fileB("logs/transitionTo.txt");
    for (auto &c : poseClusters)
    {
        fileB << c.index << " (" << c.observations.size() << ")";
        for (auto &i : c.transitionsTo)
        {
            fileB << '\t' << i.first << " (" << i.second.frameCount << ")";
        }
        fileB << endl;
    }

    ofstream fileSequences("logs/sequences.txt");
    for (auto &sequence : sequences)
    {
        fileSequences << endl << "Animation index: " << sequence.index << ", length=" << sequence.poses.size() << endl;
        for (const FrameID &frameID : sequence.instances)
        {
            fileSequences << "  frame=" << frameID.frameIndex << endl;
        }
    }

    /*ofstream fileC("logs/animationStrengths.txt");
    for (auto &instance : allInstances)
    {
        fileC << instance.first.frameIndex;
        for (int instanceCount : computeAnimationInstanceCounts(instance.second))
        {
            fileC << '\t' << instanceCount;
        }
        fileC << endl;
    }*/
    
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
        addNewCluster(frameInstance);
        allInstances[frame.frameID] = frameInstance;
    }
}

void Character::addNewCluster(CharacterFrameInstance &newInstance)
{
    //
    // check if the instance belongs to an existing pose cluster
    //
    for (PoseCluster &cluster : poseClusters)
    {
        const double dist = frameInstanceDistSqAvg(cluster.seedInstance, newInstance);
        if (dist < learningParams().poseClusterDistThreshold)
            return;
    }

    //
    // no valid cluster found; create a new one
    //
    PoseCluster newCluster;
    newCluster.index = (int)poseClusters.size();
    newCluster.seedInstance = newInstance;
    poseClusters.push_back(newCluster);
}

void Character::assignClusters(CharacterFrameInstance &newInstance)
{
    newInstance.poseClusters.clear();
    for (PoseCluster &cluster : poseClusters)
    {
        const double dist = frameInstanceDistSqAvg(cluster.seedInstance, newInstance);
        if (dist < learningParams().poseClusterSoftAssignmentThreshold)
        {
            cluster.observations.push_back(newInstance.frameID);
            newInstance.poseClusters.push_back(&cluster);
        }
    }
}

void Character::recordFrameTransition(const FramePair &pair)
{
    const CharacterFrameInstance* instance0 = findInstanceAtFrame(pair.f0->frameID);
    const CharacterFrameInstance* instance1 = findInstanceAtFrame(pair.f1->frameID);

    if (instance0 != nullptr && instance1 != nullptr)
    {
        for (auto &cluster0 : instance0->poseClusters)
        {
            for (auto &cluster1 : instance1->poseClusters)
            {
                poseClusters[cluster1->index].transitionsFrom[cluster0->index].frameCount++;
                poseClusters[cluster0->index].transitionsTo[cluster1->index].frameCount++;
            }
        }
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

void Character::updateFirstPoseMap()
{
    sequencesByFirstPose.clear();
    for (auto &animation : sequences)
    {
        for (int firstPose : animation.poses[0])
        {
            sequencesByFirstPose[firstPose].push_back(&animation);
        }
    }
}

void Character::computeAnimationSequences()
{
    cout << "*** Computing animation sequences" << endl;

    auto findOptimalAnimationLength = [](const vector<int> &v)
    {
        if (v.size() <= learningParams().minAnimationLength)
        {
            return 0;
        };
        
        const int startValue = v[learningParams().animationStartOffset];
        for (int length = learningParams().animationStartOffset; length < v.size(); length++)
        {
            const double ratio = 1.0 - fabs(double(v[length] - startValue)) / (double)startValue;
            if (ratio < learningParams().animationConsistencyRatio || v[length] < learningParams().minAnimationInstances)
            {
                if (length < learningParams().minAnimationLength)
                    return 0;
                return length;
            }
        }
        return (int)v.size();
    };

    cout << "Filling priority queue" << endl;
    priority_queue<CharacterFrameInstance*, vector<CharacterFrameInstance*>, CharacterFrameInstanceCompare> animationLengthsQueue;
    for (auto &instance : allInstances)
    {
        vector<int> animationCounts = computeAnimationInstanceCounts(instance.second);
        int optimalLength = findOptimalAnimationLength(animationCounts);
        if (optimalLength != 0)
        {
            instance.second.candidateAnimationLength = optimalLength;
            animationLengthsQueue.push(&instance.second);
        }
    }

    cout << "Emptying priority queue" << endl;
    while (!animationLengthsQueue.empty())
    {
        CharacterFrameInstance &instance = *animationLengthsQueue.top();
        animationLengthsQueue.pop();

        if (instance.sequences.size() == 0)
        {
            vector< set<PoseCluster*> > posesA, posesB, posesC;
            vector<FrameID> startingFramesA, startingFramesB, startingFramesC;

            const int instanceCountA = computeAnimationInstances(instance, learningParams().animationStartOffset, posesA, startingFramesA, true);
            const int instanceCountB = computeAnimationInstances(instance, instance.candidateAnimationLength - 1, posesB, startingFramesB, true);
            const int instanceCountC = computeAnimationInstances(instance, instance.candidateAnimationLength, posesC, startingFramesC, true);

            cout << "Animation length=" << instance.candidateAnimationLength << ", frameIndex=" << instance.frameID.frameIndex << ", instanceCounts=" << instanceCountA << "-" << instanceCountB << "-" << instanceCountC << endl;

            AnimationSequence sequence;
            sequence.index = (int)sequences.size();

            sequence.color = vec3f::origin;
            while(sequence.color.length() < 0.5f)
                sequence.color = vec3f((float)util::randomUniform(), (float)util::randomUniform(), (float)util::randomUniform());

            for (const FrameID &frameID : startingFramesA)
            {
                sequence.instances.push_back(frameID);
                for (int frameOffset = 0; frameOffset < instance.candidateAnimationLength; frameOffset++)
                {
                    CharacterFrameInstance *otherInstance = findInstanceAtFrame(frameID.next(frameOffset));

                    if (otherInstance == nullptr)
                    {
                        cout << "otherInstance == nullptr?" << endl;
                    }
                    else
                    {
                        InstanceAnimationEntry entry;
                        entry.sequenceIndex = sequence.index;
                        entry.sequenceOffset = frameOffset;
                        entry.weight = 1.0f;
                        otherInstance->sequences.push_back(entry);
                    }
                }
            }

            //sequence.poses = posesB;
            sequences.push_back(sequence);
        }
    }

    cout << "Animation count: " << sequences.size() << endl;
    
    //updateFirstPoseMap();
}

int Character::computeAnimationOverlap(const CharacterFrameInstance &instanceAStart, const CharacterFrameInstance &instanceBStart, int animationLength) const
{
    auto hasIntersection = [](const vector<PoseCluster*> &v0, const vector<PoseCluster*> &v1)
    {
        for (PoseCluster* x0 : v0)
            if (util::contains(v1, x0))
                return true;
        return false;
    };

    int overlap = 0;
    for (int frameOffset = 0; frameOffset < animationLength; frameOffset++)
    {
        const CharacterFrameInstance *instanceA = findInstanceAtFrame(instanceAStart.frameID.next(frameOffset));
        const CharacterFrameInstance *instanceB = findInstanceAtFrame(instanceBStart.frameID.next(frameOffset));

        if (instanceA == nullptr || instanceB == nullptr)
            return 0;

        if (hasIntersection(instanceA->poseClusters, instanceB->poseClusters))
            overlap++;
    }
    return overlap;
}

int Character::computeAnimationInstances(const CharacterFrameInstance &startInstance, int animationLength, vector< set<PoseCluster*> > &poses, vector<FrameID> &startingFrames, bool emitPoses) const
{
    int instanceCount = 0;
    set<FrameID> includedFrames;

    startingFrames.clear();

    if (emitPoses)
    {
        poses.resize(animationLength);
    }

    for (PoseCluster *startCluster : startInstance.poseClusters)
    {
        for (const FrameID &observedFrameID : startCluster->observations)
        {
            const CharacterFrameInstance *candidateInstance = findInstanceAtFrame(observedFrameID);

            bool valid = true;
            for (int frameOffset = 0; valid && frameOffset < animationLength; frameOffset++)
            {
                if (includedFrames.count(observedFrameID.next(frameOffset)) > 0)
                    valid = false;
            }
            if (valid)
            {
                const int overlap = computeAnimationOverlap(startInstance, *candidateInstance, animationLength);
                const double overlapPercentage = (double)overlap / (double)animationLength;
                if (overlapPercentage >= learningParams().requiredOverlapPercentage)
                {
                    if (emitPoses)
                    {
                        startingFrames.push_back(observedFrameID);
                        for (int frameOffset = 0; frameOffset < animationLength; frameOffset++)
                        {
                            const CharacterFrameInstance *instance = findInstanceAtFrame(candidateInstance->frameID.next(frameOffset));
                            for (PoseCluster *cluster : instance->poseClusters)
                                poses[frameOffset].insert(cluster);
                        }
                    }
                    for (int frameOffset = 0; frameOffset < animationLength; frameOffset++)
                        includedFrames.insert(observedFrameID.next(frameOffset));
                    instanceCount++;
                }
            }
        }
    }
    return instanceCount;
}

vector<int> Character::computeAnimationInstanceCounts(const CharacterFrameInstance &startInstance) const
{
    vector<int> result;

    vector< set<PoseCluster*> > poses;
    vector<FrameID> startingFrames;

    for (int animationLength = 1; animationLength < learningParams().maxAnimationLength; animationLength++)
    {
        const int instanceCount = computeAnimationInstances(startInstance, animationLength, poses, startingFrames, false);
        result.push_back(instanceCount);
        if (instanceCount <= learningParams().minAnimationInstances || instanceCount >= 100)
            break;
    }
    return result;
}

bool Character::animationAtFrame(const AnimationSequence &sequence, const FrameID &startFrame) const
{
    FrameID curFrame = startFrame;
    for (int sequenceIndex = 0; sequenceIndex < sequence.poses.size(); sequenceIndex++)
    {
        auto &poseChoices = sequence.poses[sequenceIndex];
        
        const CharacterFrameInstance* instance = findInstanceAtFrame(curFrame);
        if (instance == nullptr)
            return false;

        bool acceptable = false;
        for (int poseIndex : poseChoices)
        {
            for (auto &instancePose : instance->poseClusters)
                if (instancePose->index == poseIndex)
                    acceptable = true;
        }

        if (!acceptable)
            return false;

        curFrame = curFrame.next();
    }
    return true;
}

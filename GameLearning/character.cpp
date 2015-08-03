
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

void Character::labelAnimationInstances()
{
    cout << "Labeling animation instances..." << endl;
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
    }
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
    labelAnimationInstances();

    ofstream fileA("logs/transitionFrom.txt");
    for (auto &c : poseClusters)
    {
        fileA << c.index << " (" << c.observations << ")";
        for (auto &i : c.transitionsFrom)
        {
            fileA << '\t' << i.first << " (" << i.second.frameCount << ")";
        }
        fileA << endl;
    }

    ofstream fileB("logs/transitionTo.txt");
    for (auto &c : poseClusters)
    {
        fileB << c.index << " (" << c.observations << ")";
        for (auto &i : c.transitionsTo)
        {
            fileB << '\t' << i.first << " (" << i.second.frameCount << ")";
        }
        fileB << endl;
    }
}

void Character::recordFramePoses(const ProcessedFrame &frame)
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
            newInstance.poseClusters.push_back(&cluster);
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
    cout << "Computing animation sequences" << endl;

    UndirectedGraph<PoseCluster*, float> animationGraph;
    
    for (auto &pose : poseClusters)
    {
        animationGraph.addNode(&pose);
    }

    for (auto &pose : poseClusters)
    {
        float bestSaliency = (float)learningParams().requiredSaliency;
        int bestTransition = -1;
        for (auto &tr : pose.transitionsTo)
        {
            const int poseCandidate = tr.first;
            const float saliencyA = PoseCluster::transitionSaliency(pose.transitionsTo, pose.index, poseCandidate);
            const float saliencyB = PoseCluster::transitionSaliency(poseClusters[poseCandidate].transitionsFrom, poseCandidate, pose.index);
            const float totalSaliency = min(saliencyA, saliencyB);
            if (totalSaliency > bestSaliency)
            {
                bestSaliency = totalSaliency;
                bestTransition = poseCandidate;
            }
        }

        if (bestTransition != -1)
        {
            animationGraph.addEdge(pose.index, bestTransition, bestSaliency);
        }
    }

    auto components = animationGraph.computeConnectedComponents();
    for (auto &component : components)
    {
        if (component.size() >= learningParams().minAnimationLength)
        {
            AnimationSequence sequence;
            sequence.index = (int)sequences.size();

            for (auto &node : component)
            {
                vector<int> v;
                v.push_back(node->data->index);
                sequence.poses.push_back(v);
            }
            sequences.push_back(sequence);
        }
    }
    cout << "Animation count: " << sequences.size() << endl;

    updateFirstPoseMap();
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

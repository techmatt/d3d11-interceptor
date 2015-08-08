
#include "main.h"

void CharacterInstance::makeRawPoseDescriptor(const vector<UINT64> &segmentList, float *result) const
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

    for (auto &instance : allInstances)
        allInstancesVec.push_back(&instance.second);

    computePosePCA();
    computePoseDescriptors();

    computeAnimationPCA();
    computeAnimationDescriptors();

    makeAnimationSearch();

    computeAnimationSequences();
}

void Character::computePoseDescriptors()
{
    cout << "Computing pose descriptors..." << endl;

    const int poseFeatureCount = 3 * (int)allSegmentsVec.size();
    vector<float> rawPoseDescriptor(poseFeatureCount);

    for (auto &instance : allInstances)
    {
        instance.second.makeRawPoseDescriptor(allSegmentsVec, rawPoseDescriptor.data());
        posePCA.transform(rawPoseDescriptor, posePCADimension, instance.second.reducedPoseDescriptor);
    }
}

void Character::computeAnimationDescriptors()
{
    cout << "Computing animation descriptors..." << endl;

    const int animationFeatureCount = posePCADimension * Constants::animationWindowTotalSize;
    vector<float> rawAnimationDescriptor(animationFeatureCount);

    for (auto &instance : allInstances)
    {
        computeAnimationDescriptor(instance.first, rawAnimationDescriptor.data());
        animationPCA.transform(rawAnimationDescriptor, animationPCADimension, instance.second.reducedAnimationDescriptor);
    }
}

bool Character::computeAnimationDescriptor(const FrameID &centerFrame, float *result)
{
    bool valid = true;
    int animationFeatureIndex = 0;
    for (int windowOffset = -Constants::animationWindowRadius; windowOffset <= Constants::animationWindowRadius; windowOffset++)
    {
        const FrameID curFrame = centerFrame.delta(windowOffset);
        const CharacterInstance *instance = findInstanceAtFrame(curFrame);
        if (instance == nullptr)
        {
            valid = false;
            for (int poseFeatureIndex = 0; poseFeatureIndex < posePCADimension; poseFeatureIndex++)
                result[animationFeatureIndex++] = 0.0f;
        }
        else
        {
            for (int poseFeatureIndex = 0; poseFeatureIndex < posePCADimension; poseFeatureIndex++)
                result[animationFeatureIndex++] = instance->reducedPoseDescriptor[poseFeatureIndex];
        }
    }
    return valid;
}

void Character::computeAnimationPCA()
{
    const string PCADir = learningParams().datasetDir + "animationPCA/";
    const string PCAFile = PCADir + "character" + to_string(characterIndex) + ".dat";

    if (util::fileExists(PCAFile))
    {
        cout << "Loading animation PCA from " << PCAFile << endl;
        animationPCA.load(PCAFile);
    }
    else
    {
        const int animationFeatureCount = posePCADimension * Constants::animationWindowTotalSize;

        DenseMatrixf M(Constants::animationPCASamples, animationFeatureCount);

        for (int sampleIndex = 0; sampleIndex < Constants::animationPCASamples; sampleIndex++)
        {
            bool valid = false;
            while (!valid)
            {
                const CharacterInstance &randomInstance = *util::randomElement(allInstancesVec);
                valid = computeAnimationDescriptor(randomInstance.frameID, &M(sampleIndex, 0));
            }
        }

        animationPCA.init(M);

        util::makeDirectory(PCADir);
        animationPCA.save(PCAFile);
    }

    animationPCADimension = (int)animationPCA.reducedDimension(learningParams().PCAEnergy);
    cout << "Animation PCA dimension: " << animationPCADimension << endl;
}

void Character::computePosePCA()
{
    const string PCADir = learningParams().datasetDir + "posePCA/";
    const string PCAFile = PCADir + "character" + to_string(characterIndex) + ".dat";

    if (util::fileExists(PCAFile))
    {
        cout << "Loading pose PCA from " << PCAFile << endl;
        posePCA.load(PCAFile);
    }
    else
    {
        const int poseFeatureCount = 3 * (int)allSegmentsVec.size();

        DenseMatrixf M(Constants::posePCASamples, poseFeatureCount);

        for (int sampleIndex = 0; sampleIndex < Constants::posePCASamples; sampleIndex++)
        {
            const CharacterInstance &instance = *util::randomElement(allInstancesVec);
            instance.makeRawPoseDescriptor(allSegmentsVec, &M(sampleIndex, 0));

            //vector<float> descriptor = instance.makeRawPoseDescriptor(allSegmentsVec);
            //for(int featureIndex = 0; featureIndex < poseFeatureCount; featureIndex++)
            //   M(sampleIndex, featureIndex) = descriptor[featureIndex];
        }

        posePCA.init(M);

        util::makeDirectory(PCADir);
        posePCA.save(PCAFile);
    }

    posePCADimension = (int)posePCA.reducedDimension(learningParams().PCAEnergy);
    cout << "Pose PCA dimension: " << posePCADimension << endl;
}

void Character::recordFramePoses(const ProcessedFrame &frame)
{
    CharacterInstance frameInstance;
    
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

bool Character::animationsShouldMergeDeviation(AnimationSequence *a, AnimationSequence *b, float deviation)
{
    auto deviationAcceptable = [](float dA, float dB)
    {
        float diffRatio = max(dA, dB) / (min(dA, dB) + 1.0f);
        //cout << "  dA=" << dA << ", dB=" << dB << ", ratio=" << diffRatio << endl;
        return (diffRatio < 1.0f);
    };

    if (a->deviations.size() == 0 || b->deviations.size() == 0)
        return true;
    else if (a->deviations.size() == 0)
        return animationsShouldMergeDeviation(b, a, deviation);
    else if (b->deviations.size() == 0)
        return deviationAcceptable(a->maxDeviation, deviation);
    else
        return deviationAcceptable(a->maxDeviation, b->maxDeviation) &&
               deviationAcceptable(deviation, a->maxDeviation) &&
               deviationAcceptable(deviation, b->maxDeviation);
}

bool Character::animationsShouldMergeSeed(AnimationSequence *a, AnimationSequence *b, float seedDistSq)
{
    auto deviationAcceptable = [](float dA, float dB)
    {
        float diffRatio = max(dA, dB) / (min(dA, dB) + 1.0f);
        return (diffRatio < 1.0f);
    };

    if (a->minSeedDistSq == 0.0f && b->minSeedDistSq == 0.0f)
        return true;
    else if (a->minSeedDistSq == 0.0f)
        return animationsShouldMergeSeed(b, a, seedDistSq);
    else if (b->minSeedDistSq == 0.0f)
        return deviationAcceptable(a->minSeedDistSq, seedDistSq);
    else
        return deviationAcceptable(a->minSeedDistSq, b->minSeedDistSq) &&
               deviationAcceptable(seedDistSq, a->minSeedDistSq) &&
               deviationAcceptable(seedDistSq, b->minSeedDistSq);
}

void Character::computeAnimationSequences()
{
    return;
    cout << "*** Computing animation sequences" << endl;

    // this is just so I can focus on Marth for now
    if (characterIndex == 0)
        return;

    cout << "Adding all instances" << endl;
    for (CharacterInstance *instance : allInstancesVec)
    {
        AnimationSequence *newAnimation = new AnimationSequence(instance->frameID);
        instance->sequence = newAnimation;
        sequences.insert(newAnimation);
    }

    mergeNearestSequences();
    mergeAdjacentFrames();
}

void Character::mergeNearestSequences()
{
    cout << "Merging kNearest sequences" << endl;
    priority_queue<AnimationGraphSimilarEdge> queue;
    for (CharacterInstance *instance : allInstancesVec)
    {
        auto similarInstances = findKNearestInstances(*instance, 3);
        for (CharacterInstance *similarInstance : similarInstances)
        {
            AnimationGraphSimilarEdge edge;
            edge.instA = instance;
            edge.instB = similarInstance;
            edge.distSq = math::distSq(edge.instA->reducedAnimationDescriptor, edge.instB->reducedAnimationDescriptor);
            
            if (edge.distSq < 50.0f)
                queue.push(edge);
        }
    }

    cout << "Emptying queue, sequences=" << sequences.size() << endl;
    while (!queue.empty())
    {
        AnimationGraphSimilarEdge edge = queue.top();
        queue.pop();

        if (edge.instA->sequence == edge.instB->sequence)
            continue;

        //cout << "edge distSq: " << edge.distSq << endl;
        if (animationsShouldMergeSeed(edge.instA->sequence, edge.instB->sequence, edge.distSq))
        {
            //cout << "  merging A=" << edge.instA->sequence->instances.size() << " B=" << edge.instB->sequence->instances.size() << " total=" << sequences.size() << endl;
            mergeAnimations(edge.instA->sequence, edge.instB->sequence, edge.distSq, -1.0f);
        }
    }
    cout << "Queue empty, sequences=" << sequences.size() << endl;
}

void Character::mergeAdjacentFrames()
{
    //ofstream file("logs/edgeDeviations" + to_string(characterIndex) + ".txt");

    cout << "Adding neighbor edges" << endl;
    priority_queue<AnimationGraphNeighborEdge> queue;
    for (CharacterInstance *instance : allInstancesVec)
    {
        CharacterInstance *nextInstance = findInstanceAtFrame(instance->frameID.delta(1));
        if (nextInstance == nullptr)
            continue;
        AnimationGraphNeighborEdge edge;
        edge.instA = instance;
        edge.instB = nextInstance;
        edge.deviationAToB = computeTransitionDeviation(*edge.instA, 1);
        edge.deviationBToA = computeTransitionDeviation(*edge.instB, -1);
        //file << instance->frameID.toString() << '\t' << edge.deviationAToB << '\t' << edge.deviationBToA << endl;

        if (edge.totalDeviation() <= 500.0f)
            queue.push(edge);
    }

    int i = 0;
    cout << "Merging adjacent frames sequences=" << sequences.size() << endl;
    while (!queue.empty())
    {
        AnimationGraphNeighborEdge edge = queue.top();
        queue.pop();

        if (edge.instA->sequence == edge.instB->sequence)
            continue;

        //cout << "deviation=" << edge.totalDeviation() << endl;
        if (animationsShouldMergeDeviation(edge.instA->sequence, edge.instB->sequence, edge.totalDeviation()))
        {
            //cout << "  merging A=" << edge.instA->sequence->instances.size() << " B=" << edge.instB->sequence->instances.size() << " total=" << sequences.size() << endl;
            mergeAnimations(edge.instA->sequence, edge.instB->sequence, -1.0f, edge.totalDeviation());
        }
        else
        {
            //cout << "  animations not merging" << endl;
            //cin.get();
        }

        i++;
        //if (i % 1000 == 0)
        //    cin.get();
    }
    cout << "Queue empty, sequences=" << sequences.size() << endl;
}

void Character::testAnimationSearch(float pNorm, UINT miniHashFunctionCount, UINT macroTableCount)
{
    cout << "Testing LSH " << pNorm << ", " << miniHashFunctionCount << ", " << macroTableCount << endl;
    LSHEuclidean<CharacterInstance*> search;
    search.init(animationPCADimension, pNorm, miniHashFunctionCount, macroTableCount);
    for (auto &instance : allInstances)
        search.insert(instance.second.reducedAnimationDescriptor, &instance.second);

    const UINT testInstanceCount = 500;
    const float targetDistSq = 10.0f;
    size_t missedSamples = 0;
    size_t wastedSamples = 0;
    
    size_t totalValidSamples = 0;

    ofstream fileDebug("debug.txt");
    for (int test = 0; test < testInstanceCount; test++)
    {
        const CharacterInstance &randomInstance = *util::randomElement(allInstancesVec);

        auto results = search.findSimilar(randomInstance.reducedAnimationDescriptor);
        for (CharacterInstance *candidate : results)
        {
            const float distSq = math::distSq(candidate->reducedAnimationDescriptor, randomInstance.reducedAnimationDescriptor);
            if (distSq > targetDistSq)
                wastedSamples++;
        }

        for (auto &instance : allInstancesVec)
        {
            const float distSq = math::distSq(instance->reducedAnimationDescriptor, randomInstance.reducedAnimationDescriptor);
            if (test == 0)
                fileDebug << distSq << endl;
            if (distSq < targetDistSq)
            {
                if (results.count(instance) == 0)
                    missedSamples++;
                totalValidSamples++;
            }   
        }
    }

    const string filename = "logs/animationSearchTest.txt";
    bool emitHeader = !util::fileExists(filename);
    ofstream file(filename, ios::app);
    if (emitHeader)
    {
        file << "pNorm\tminiHashCount\tmacroTableCount\ttargetDistSq\tavg valid count\tavg missed samples\tavg wasted samples" << endl;
    }
    file << pNorm << '\t' << miniHashFunctionCount << '\t' << macroTableCount << '\t' << targetDistSq << '\t';
    file << (double)totalValidSamples / (double)testInstanceCount << '\t';
    file << (double)missedSamples / (double)testInstanceCount << '\t';
    file << (double)wastedSamples / (double)testInstanceCount << endl;
}

void Character::makeAnimationSearch()
{
    const bool LSHParameterSearch = false;
    if (LSHParameterSearch)
    {
        // best values found for targetDistSq = 5.0:
        // pNorm=0.0135, k=30, L=20
        for (float dist = 0.004f; dist <= 0.3f; dist *= 1.5f)
            for (int k = 10; k <= 30; k += 5)
                for (int L = 5; L <= 20; L += 5)
                    testAnimationSearch(dist, k, L);
    }

    cout << "Building LSH tables" << endl;
    auto &params = learningParams();
    animationSearch.init(animationPCADimension, (float)params.LSHpNorm, params.LSHminiHashCount, params.LSHmacroTableCount);
    for (auto &instance : allInstances)
    {
        animationSearch.insert(instance.second.reducedAnimationDescriptor, &instance.second);
    }
}

vector<CharacterInstance*> Character::findKNearestInstances(const CharacterInstance &instance, int k)
{
    auto candidates = animationSearch.findSimilar(instance.reducedAnimationDescriptor);
    KNearestNeighborQueue<float, CharacterInstance*> queue;
    queue.init(k, std::numeric_limits<float>::max());
    for (auto &candidateInstance : candidates)
    {
        const float distSq = math::distSq(instance.reducedAnimationDescriptor, candidateInstance->reducedAnimationDescriptor);
        queue.insert(candidateInstance, distSq);
    }

    vector<CharacterInstance*> result;
    for (auto &entry : queue.queue())
        if (entry.value != nullptr)
            result.push_back(entry.value);
    return result;
}

float Character::computeTransitionDeviation(const CharacterInstance &instStart, int delta)
{
    CharacterInstance *instEnd = findInstanceAtFrame(instStart.frameID.delta(delta));
    if (instEnd == nullptr)
        return std::numeric_limits<float>::max();

    float distSqSum = 0.0f;
    int count = 0;

    auto endKNearest = findKNearestInstances(*instEnd, learningParams().transitionKNearest);
    for (CharacterInstance *similarEnd : endKNearest)
    {
        CharacterInstance *reprojectedInstance = findInstanceAtFrame(similarEnd->frameID.delta(-delta));
        if (reprojectedInstance == nullptr || reprojectedInstance == &instStart)
            continue;

        const float distSq = math::distSq(instStart.reducedAnimationDescriptor, reprojectedInstance->reducedAnimationDescriptor);
        distSqSum += distSq;
        count++;
    }

    if (count == 0)
        return std::numeric_limits<float>::max();

    return distSqSum / (float)count;
}

void Character::mergeAnimations(AnimationSequence *a, AnimationSequence *b, float seedDistSq, float neighborDeviation)
{
    if (b->instances.size() > a->instances.size())
    {
        mergeAnimations(b, a, seedDistSq, neighborDeviation);
        return;
    }
    for (FrameID bInstFrame : b->instances)
    {
        CharacterInstance *bInst = findInstanceAtFrame(bInstFrame);
        if (bInst == nullptr)
        {
            cout << "bInst == nullptr?" << endl;
            continue;
        }

        bInst->sequence = a;
        a->instances.push_back(bInstFrame);
    }
    for (float bDeviation : b->deviations)
        a->addDeviation(bDeviation);

    if (neighborDeviation > 0.0f)
        a->addDeviation(neighborDeviation);

    a->minSeedDistSq = min(a->minSeedDistSq, b->minSeedDistSq);

    if (seedDistSq > 0.0f)
        a->minSeedDistSq = min(a->minSeedDistSq, seedDistSq);

    sequences.erase(b);
    delete b;
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


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

void Character::computeAnimationSequences()
{
    cout << "*** Computing animation sequences" << endl;

    ofstream file("logs/animationInfo.txt");
    int minWindowSize = 5;
    for (float acceptanceScale = 1.0f; acceptanceScale <= 10.0f; acceptanceScale *= 1.5f)
    {
        addAnimationSequences(acceptanceScale, minWindowSize);

        if (minWindowSize > 0) minWindowSize--;

        file << endl << "*** acceptanceScale = " << acceptanceScale << endl;
        for (auto &instance : allInstancesVec)
        {
            string animationName = "(none)";
            if (instance->sequences.size() == 1)
                animationName = to_string(instance->sequences[0].sequenceIndex) + "-" + to_string(instance->sequences[0].sequenceOffset);
            if (instance->sequences.size() >= 2)
                animationName = "(multiple)";
            file << instance->frameID.toString() << '\t' << animationName << endl;
        }
    }
}

void Character::addAnimationSequences(float acceptanceScale, int minWindowSize)
{
    cout << "Filling priority queue acceptanceScale=" << acceptanceScale << endl;
    priority_queue<CharacterInstance*, vector<CharacterInstance*>, CharacterInstanceCompare> seedQueue;
    for (CharacterInstance *instance : allInstancesVec)
    {
        if (instance->sequences.size() == 0)
        {
            instance->optimalAnimationWindowSize = evaluateBestWindowSize(*instance, acceptanceScale, instance->estimatedAnimationInstanceCount);
            if (instance->estimatedAnimationInstanceCount >= learningParams().minAnimationInstances &&
                instance->optimalAnimationWindowSize >= minWindowSize)
            {
                seedQueue.push(instance);
            }
        }
    }
    
    vector<FrameID> animationCenterFrames;
    cout << "Emptying priority queue" << endl;
    while (!seedQueue.empty())
    {
        CharacterInstance &seedInstance = *seedQueue.top();
        seedQueue.pop();

        if (seedInstance.sequences.size() == 0)
        {
            AnimationSequence sequence;
            sequence.index = (int)sequences.size();

            sequence.color = vec3f::origin;
            while (sequence.color.length() < 0.5f)
                sequence.color = vec3f((float)util::randomUniform(), (float)util::randomUniform(), (float)util::randomUniform());
            
            const int windowSize = seedInstance.optimalAnimationWindowSize;
            const int animationInstances = evaluateAnimationInstances(seedInstance, windowSize, acceptanceScale, &animationCenterFrames);

            cout << "New sequence: windowSize=" << windowSize << ", instances=" << animationInstances << endl;

            for (const FrameID &frameID : animationCenterFrames)
            {
                for (int window = -windowSize; window <= windowSize; window++)
                {
                    FrameID curFrame = frameID.delta(window);
                    CharacterInstance *otherInstance = findInstanceAtFrame(curFrame);
                    if (otherInstance->sequences.size() == 0)
                    {
                        if (otherInstance == nullptr)
                        {
                            cout << "otherInstance == nullptr?: " << curFrame.frameIndex << endl;
                        }
                        else
                        {
                            InstanceAnimationEntry entry;
                            entry.sequenceIndex = sequence.index;
                            entry.sequenceOffset = window;
                            entry.weight = 1.0f;
                            sequence.instances.push_back(curFrame);
                            otherInstance->sequences.push_back(entry);
                        }
                    }
                }
            }

            //sequence.poses = posesB;
            sequences.push_back(sequence);
        }
    }

    /*ofstream file("logs/animationStrength" + to_string(characterIndex) + ".txt");
    for (int i = 0; i < allInstancesVec.size(); i += 10)
    {
        CharacterInstance &instance = *allInstancesVec[i];
        file << instance.frameID.frameIndex;
        file << '\t' << evaluateBestWindowSize(instance);
        for (int windowSize = 0; windowSize <= 20; windowSize++)
        {
            file << '\t' << evaluateAnimationInstances(instance, windowSize);
        }
        file << endl;
    }*/

    cout << "Animation count: " << sequences.size() << endl;
    
    //updateFirstPoseMap();
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

int Character::evaluateAnimationMatchingFrames(const CharacterInstance &frameA, const CharacterInstance &frameB, int windowSize, float acceptanceScale)
{
    int matchingFrames = 0;
    for (int window = -windowSize; window <= windowSize; window++)
    {
        CharacterInstance *instanceA = findInstanceAtFrame(frameA.frameID.delta(window));
        CharacterInstance *instanceB = findInstanceAtFrame(frameB.frameID.delta(window));

        if (instanceA == nullptr || instanceB == nullptr)
            return 0;

        const float distSq = math::distSq(instanceA->reducedAnimationDescriptor, instanceB->reducedAnimationDescriptor);
        if (distSq < learningParams().baseAnimationFeatureDistSq * acceptanceScale)
            matchingFrames++;
    }
    return matchingFrames;
}

int Character::evaluateAnimationInstances(const CharacterInstance &seed, int windowSize, float acceptanceScale, vector<FrameID> *instanceFrames)
{
    if (instanceFrames != nullptr)
        instanceFrames->clear();

    int matchingSequences = 0;
    set<FrameID> invalidFrames;

    auto candidatesUnsorted = animationSearch.findSimilar(seed.reducedAnimationDescriptor);

    vector< pair<CharacterInstance*, float> > candidatesSorted;
    for (CharacterInstance *candidate : candidatesUnsorted)
    {
        const float distSq = math::distSq(candidate->reducedAnimationDescriptor, seed.reducedAnimationDescriptor);
        if (distSq >= learningParams().baseAnimationFeatureDistSq * acceptanceScale)
            continue;
        candidatesSorted.push_back(make_pair(candidate, distSq));
    }
    sort(candidatesSorted.begin(), candidatesSorted.end(),
        [](const pair<CharacterInstance*, float> &a, const pair<CharacterInstance*, float> &b) { return a.second < b.second; });

    for (auto &c : candidatesSorted)
    {
        auto &candidate = *c.first;
        const int matchingFrames = evaluateAnimationMatchingFrames(seed, candidate, windowSize, acceptanceScale);

        const float matchingPercent = (float)matchingFrames / (float)(windowSize * 2 + 1);
        if (matchingPercent >= learningParams().requiredOverlapPercentage)
        {
            bool valid = true;
            for (int window = -windowSize; window <= windowSize; window++)
                if (invalidFrames.count(candidate.frameID.delta(window)) > 0)
                    valid = false;

            if (valid)
            {
                for (int window = -windowSize; window <= windowSize; window++)
                    invalidFrames.insert(candidate.frameID.delta(window));
                matchingSequences++;
                if (instanceFrames != nullptr)
                    instanceFrames->push_back(candidate.frameID);
            }
        }
    }

    return matchingSequences;
}

int Character::evaluateBestWindowSize(const CharacterInstance &seed, float acceptanceScale, int &instanceCount)
{
    const int startInstanceCount = evaluateAnimationInstances(seed, 0, acceptanceScale);
    const int minInstances = max(learningParams().minAnimationInstances, (int)ceil((double)startInstanceCount * learningParams().maxAnimationDropOffPercentage));

    instanceCount = startInstanceCount;

    for (int windowSize = 1; windowSize < learningParams().maxAnimationLength / 2; windowSize++)
    {
        const int curInstanceCount = evaluateAnimationInstances(seed, windowSize, acceptanceScale);
        if (curInstanceCount < minInstances)
            return windowSize - 1;
        instanceCount = curInstanceCount;
    }
    return learningParams().maxAnimationLength / 2;
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

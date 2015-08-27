
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

    saveAnimationCurve();

    makeLSHSearch();

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

    const int animationFeatureCount = posePCADimension * Constants::animationPoseCount;
    vector<float> rawAnimationDescriptor(animationFeatureCount);

    for (auto &instance : allInstances)
    {
        computeAnimationDescriptor(instance.first, rawAnimationDescriptor.data());
        poseChainPCA.transform(rawAnimationDescriptor, poseChainPCADimension, instance.second.reducedPoseChainDescriptor);
    }
}

bool Character::computeAnimationDescriptor(const FrameID &centerFrame, float *result)
{
    bool valid = true;
    int animationFeatureIndex = 0;
    for (int windowOffset = 0; windowOffset < Constants::animationPoseCount; windowOffset++)
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
    const string PCADir = learningParams().datasetDir + "poseChainPCA/";
    const string PCAFile = PCADir + "character" + to_string(characterIndex) + ".dat";

    if (util::fileExists(PCAFile))
    {
        cout << "Loading animation PCA from " << PCAFile << endl;
        poseChainPCA.load(PCAFile);
    }
    else
    {
        const int animationFeatureCount = posePCADimension * Constants::animationPoseCount;

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

        poseChainPCA.init(M);

        util::makeDirectory(PCADir);
        poseChainPCA.save(PCAFile);
    }

    poseChainPCADimension = (int)poseChainPCA.reducedDimension(learningParams().PCAEnergy);
    cout << "Animation PCA dimension: " << poseChainPCADimension << endl;
}

void Character::saveAnimationCurve()
{
    const string curveDir = learningParams().datasetDir + "curves/";
    const string curveFile = curveDir + "character" + to_string(characterIndex) + ".csv";

    if (util::fileExists(curveFile))
    {
        cout << "Animation curve already exported" << endl;
    }
    else
    {
        util::makeDirectory(curveDir);
        ofstream file(curveFile);

        file << "pose0,pose1,animation0,animation1" << endl;

        for (int sampleIndex = 0; sampleIndex < allInstancesVec.size(); sampleIndex++)
        {
            const CharacterInstance &instance = *allInstancesVec[sampleIndex];
            file << instance.reducedPoseDescriptor[0] << "," << instance.reducedPoseDescriptor[1] << ",";
            file << instance.reducedPoseChainDescriptor[0] << "," << instance.reducedPoseChainDescriptor[1];
            file << endl;
        }
    }

    posePCADimension = (int)posePCA.reducedDimension(learningParams().PCAEnergy);
    cout << "Pose PCA dimension: " << posePCADimension << endl;
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

    // this is just so I can focus on Marth for now
    if (characterIndex == 0)
        return;

    //ofstream file("logs/animationInfo.txt");
    int minAnimationLength = 5;
    for (float acceptanceScale = 1.0f; acceptanceScale <= 10.0f; acceptanceScale *= 1.5f)
    //float acceptanceScale = 1.0f;
    {
        addAnimationSequences(acceptanceScale, minAnimationLength);

        if (minAnimationLength > 0) minAnimationLength--;

        /*file << endl << "*** acceptanceScale = " << acceptanceScale << endl;
        for (auto &instance : allInstancesVec)
        {
            string animationName = "(none)";
            if (instance->animations.size() > 0)
                animationName = to_string(instance->animations[0].animation->index) + "-" + to_string(instance->animations[0].offset);
            file << instance->frameID.toString() << '\t' << animationName << endl;
        }*/
    }

    for (CharacterInstance *inst : allInstancesVec)
    {
        if (inst->animation.animation != nullptr)
        {
            inst->animation.animation->instances.push_back(inst->frameID);
        }
    }
}

int Character::computeAnimationDuration(const CharacterInstance &seed, const CharacterInstance &instance, int animationLength, float acceptanceScale)
{
    vector<int> matches = animationMatchingFrames(seed, instance, animationLength, acceptanceScale);
    if (matches.size() == 0) return 0;

    double sum = 0.0;
    for (int i = 0; i < animationLength; i++)
    {
        sum += matches[i];
        const double matchingRatio = sum / (double)(i + 1);
        if (matchingRatio < learningParams().requiredOverlapPercentage)
        {
            return i;
        }
    }
    return animationLength;
}

void Character::addNewAnimation(const CharacterInstance &seed, int animationLength)
{
    animationLength = max(animationLength, learningParams().minAnimationLength);
    for (int offset = 0; offset < animationLength; offset++)
    {
        FrameID animationFrame = seed.frameID.delta(offset);
        CharacterInstance *animationInstance = findInstanceAtFrame(animationFrame);
        if (animationInstance == nullptr)
        {
            cout << "Animation rejected" << endl;
            return;
        }
    }

    AnimationSequence *animation = new AnimationSequence(seed.frameID, animationLength, (int)animations.size());

    int frameCount = 0;
    for (int offset = 0; offset < animationLength; offset++)
    {
        FrameID animationFrame = seed.frameID.delta(offset);
        CharacterInstance *animationInstance = findInstanceAtFrame(animationFrame);

        for (auto pose : findPosesRadius(*animationInstance, (float)learningParams().maxPoseDistSq))
        {
            CharacterInstance *otherInstance = pose.first;
            const float poseDistSq = pose.second;
            if (otherInstance->animation.poseDistSq > poseDistSq)
            {
                if (otherInstance->animation.animation != animation)
                    frameCount++;
                otherInstance->animation.animation = animation;
                otherInstance->animation.offset = offset;
                otherInstance->animation.poseDistSq = poseDistSq;
            }
        }
    }

    cout << "New animation: length=" << animationLength << ", estFrameCount=" << frameCount << endl;

    animations.push_back(animation);
}

void Character::addAnimationSequences(float acceptanceScale, int minAnimationLength)
{
    cout << "Filling priority queue acceptanceScale=" << acceptanceScale << endl;
    priority_queue<CharacterInstance*, vector<CharacterInstance*>, CharacterInstanceCompare> seedQueue;
    for (CharacterInstance *instance : allInstancesVec)
    {
        if (instance->animation.poseDistSq > learningParams().poseDistSqThreshold)
        {
            instance->optimalAnimationLength = evaluateBestAnimationLength(*instance, acceptanceScale, instance->estimatedAnimationInstanceCount);
            if (minAnimationLength == 1 ||
                (instance->estimatedAnimationInstanceCount >= learningParams().minAnimationInstances &&
                 instance->optimalAnimationLength >= minAnimationLength))
            {
                seedQueue.push(instance);
            }
        }
    }

    vector<FrameID> animationStartFrames;
    cout << "Emptying priority queue" << endl;
    while (!seedQueue.empty())
    {
        CharacterInstance &seedInstance = *seedQueue.top();
        seedQueue.pop();

        if (seedInstance.animation.poseDistSq > learningParams().poseDistSqThreshold)
        {
            addNewAnimation(seedInstance, seedInstance.optimalAnimationLength);
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

    cout << "Animation count: " << animations.size() << endl;

    //updateFirstPoseMap();
}

vector<int> Character::animationMatchingFrames(const CharacterInstance &frameA, const CharacterInstance &frameB, int animationLength, float acceptanceScale)
{
    vector<int> result(animationLength, 0);
    for (int offset = 0; offset < animationLength; offset++)
    {
        CharacterInstance *instanceA = findInstanceAtFrame(frameA.frameID.delta(offset));
        CharacterInstance *instanceB = findInstanceAtFrame(frameB.frameID.delta(offset));

        if (instanceA == nullptr || instanceB == nullptr)
        {
            result.clear();
            return result;
        }

        const float distSq = math::distSq(instanceA->reducedPoseChainDescriptor, instanceB->reducedPoseChainDescriptor);
        if (distSq < learningParams().animationDistSqBase * acceptanceScale)
            result[offset] = 1;
    }
    return result;
}

int Character::countAnimationInstances(const CharacterInstance &seed, int animationLength, float acceptanceScale)
{
    int matchingSequences = 0;
    set<FrameID> invalidFrames;

    auto candidatesUnsorted = poseChainSearch.findSimilar(seed.reducedPoseChainDescriptor);

    vector< pair<CharacterInstance*, float> > candidatesSorted;
    for (CharacterInstance *candidate : candidatesUnsorted)
    {
        const float distSq = math::distSq(candidate->reducedPoseChainDescriptor, seed.reducedPoseChainDescriptor);
        if (distSq <= learningParams().animationDistSqBase * acceptanceScale)
            candidatesSorted.push_back(make_pair(candidate, distSq));
    }
    sort(candidatesSorted.begin(), candidatesSorted.end(),
        [](const pair<CharacterInstance*, float> &a, const pair<CharacterInstance*, float> &b) { return a.second < b.second; });

    for (auto &c : candidatesSorted)
    {
        auto &candidate = *c.first;
        const int animationDuration = computeAnimationDuration(seed, candidate, animationLength, acceptanceScale);

        if (animationDuration >= animationLength)
        {
            bool valid = true;
            for (int offset = 0; offset < animationLength; offset++)
                if (invalidFrames.count(candidate.frameID.delta(offset)) > 0)
                    valid = false;

            if (valid)
            {
                for (int offset = 0; offset < animationLength; offset++)
                    invalidFrames.insert(candidate.frameID.delta(offset));
                matchingSequences++;
            }
        }
    }

    return matchingSequences;
}

int Character::evaluateBestAnimationLength(const CharacterInstance &seed, float acceptanceScale, int &instanceCount)
{
    const int startInstanceCount = countAnimationInstances(seed, 0, acceptanceScale);
    const int minInstances = max(learningParams().minAnimationInstances, (int)ceil((double)startInstanceCount * learningParams().maxAnimationDropOffPercentage));

    instanceCount = startInstanceCount;

    for (int animationLength = 1; animationLength < learningParams().maxAnimationLength; animationLength++)
    {
        const int curInstanceCount = countAnimationInstances(seed, animationLength, acceptanceScale);
        if (curInstanceCount < minInstances)
            return animationLength - 1;
        instanceCount = curInstanceCount;
    }
    return learningParams().maxAnimationLength;
}

void Character::testAnimationSearch(float pNorm, UINT miniHashFunctionCount, UINT macroTableCount)
{
    cout << "Testing LSH " << pNorm << ", " << miniHashFunctionCount << ", " << macroTableCount << endl;
    LSHEuclidean<CharacterInstance*> search;
    search.init(poseChainPCADimension, pNorm, miniHashFunctionCount, macroTableCount);
    for (auto &instance : allInstances)
        search.insert(instance.second.reducedPoseChainDescriptor, &instance.second);

    const UINT testInstanceCount = 500;
    const float targetDistSq = 10.0f;
    size_t missedSamples = 0;
    size_t wastedSamples = 0;
    
    size_t totalValidSamples = 0;

    ofstream fileDebug("debug.txt");
    for (int test = 0; test < testInstanceCount; test++)
    {
        const CharacterInstance &randomInstance = *util::randomElement(allInstancesVec);

        auto results = search.findSimilar(randomInstance.reducedPoseChainDescriptor);
        for (CharacterInstance *candidate : results)
        {
            const float distSq = math::distSq(candidate->reducedPoseChainDescriptor, randomInstance.reducedPoseChainDescriptor);
            if (distSq > targetDistSq)
                wastedSamples++;
        }

        for (auto &instance : allInstancesVec)
        {
            const float distSq = math::distSq(instance->reducedPoseChainDescriptor, randomInstance.reducedPoseChainDescriptor);
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

void Character::makeLSHSearch()
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
    poseChainSearch.init(poseChainPCADimension, (float)params.LSHpNorm, params.LSHminiHashCount, params.LSHmacroTableCount);
    for (auto &instance : allInstances)
    {
        poseChainSearch.insert(instance.second.reducedPoseChainDescriptor, &instance.second);
    }

    poseSearch.init(posePCADimension, (float)params.LSHpNorm, params.LSHminiHashCount, params.LSHmacroTableCount);
    for (auto &instance : allInstances)
    {
        poseSearch.insert(instance.second.reducedPoseDescriptor, &instance.second);
    }
}

/*vector<CharacterInstance*> Character::findAnimationsKNearest(const CharacterInstance &instance, int k)
{
    auto candidates = poseChainSearch.findSimilar(instance.reducedPoseChainDescriptor);
    KNearestNeighborQueue<float, CharacterInstance*> queue;
    queue.init(k, std::numeric_limits<float>::max());
    for (auto &candidateInstance : candidates)
    {
        const float distSq = math::distSq(instance.reducedPoseChainDescriptor, candidateInstance->reducedPoseChainDescriptor);
        queue.insert(candidateInstance, distSq);
    }

    vector<CharacterInstance*> result;
    for (auto &entry : queue.queue())
        if (entry.value != nullptr)
            result.push_back(entry.value);
    return result;
}*/

vector< pair<CharacterInstance*, float> > Character::findAnimationsRadius(const CharacterInstance &instance, float maxDistSq)
{
    auto candidates = poseChainSearch.findSimilar(instance.reducedPoseChainDescriptor);
    vector< pair<CharacterInstance*, float> > result;
    for (auto &candidateInstance : candidates)
    {
        const float distSq = math::distSq(instance.reducedPoseChainDescriptor, candidateInstance->reducedPoseChainDescriptor);
        if (distSq <= maxDistSq)
            result.push_back(make_pair(candidateInstance, distSq));
    }
    return result;
}

vector< pair<CharacterInstance*, float> > Character::findPosesRadius(const CharacterInstance &instance, float maxDistSq)
{
    auto candidates = poseSearch.findSimilar(instance.reducedPoseDescriptor);
    vector< pair<CharacterInstance*, float> > result;
    for (auto &candidateInstance : candidates)
    {
        const float distSq = math::distSq(instance.reducedPoseDescriptor, candidateInstance->reducedPoseDescriptor);
        if (distSq <= maxDistSq)
            result.push_back(make_pair(candidateInstance, distSq));
    }
    return result;
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

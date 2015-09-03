
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

    computePoseChainForwardPCA();
    computePoseChainForwardDescriptors();

    computePoseChainReversePCA();
    computePoseChainReverseDescriptors();

    saveAnimationCurve();

    makePoseLSHSearch();

    makePoseClusters();

    //computeAnimationSequences();
}

void Character::computePoseDescriptors()
{
    cout << "Computing pose descriptors..." << endl;

    const int poseFeatureCount = 3 * (int)allSegmentsVec.size();
    vector<float> rawPoseDescriptor(poseFeatureCount);

    for (auto &instance : allInstances)
    {
        instance.second.makeRawPoseDescriptor(allSegmentsVec, rawPoseDescriptor.data());
        posePCA.transform(rawPoseDescriptor, posePCADimension, instance.second.poseDescriptor);
    }
}

void Character::computePoseChainForwardDescriptors()
{
    cout << "Computing pose chain forward descriptors..." << endl;

    const int poseChainFeatureCount = min(Constants::poseChainForwardMaxDim, posePCADimension) * Constants::poseChainForwardLength;
    vector<float> rawPoseChainDescriptor(poseChainFeatureCount);

    for (auto &instance : allInstances)
    {
        computePoseChainForwardDescriptor(instance.first, rawPoseChainDescriptor.data());
        poseChainForwardPCA.transform(rawPoseChainDescriptor, poseChainForwardPCADimension, instance.second.poseChainForwardDescriptor);
    }
}

int Character::poseChainReverseFeatureCount() const
{
    return min(Constants::poseChainReverseMaxDim, posePCADimension) * Constants::poseChainReverseLength;;
}

void Character::computePoseChainReverseDescriptors()
{
    cout << "Computing pose chain reverse descriptors..." << endl;

    vector<float> rawPoseChainReverseDescriptor(poseChainReverseFeatureCount());

    for (auto &instance : allInstances)
    {
        computePoseChainReverseDescriptor(instance.first, rawPoseChainReverseDescriptor.data());
        poseChainReversePCA.transform(rawPoseChainReverseDescriptor, poseChainReversePCADimension, instance.second.poseChainReverseDescriptor);
    }
}

bool Character::computePoseChainForwardDescriptor(const FrameID &startFrame, float *result) const
{
    const int poseDims = min(Constants::poseChainForwardMaxDim, posePCADimension);

    bool valid = true;
    int animationFeatureIndex = 0;
    for (int windowOffset = 0; windowOffset < Constants::poseChainForwardLength; windowOffset++)
    {
        const FrameID curFrame = startFrame.delta(windowOffset);
        const CharacterInstance *instance = findInstanceAtFrame(curFrame);
        if (instance == nullptr)
        {
            valid = false;
            for (int poseFeatureIndex = 0; poseFeatureIndex < poseDims; poseFeatureIndex++)
                result[animationFeatureIndex++] = 0.0f;
        }
        else
        {
            for (int poseFeatureIndex = 0; poseFeatureIndex < poseDims; poseFeatureIndex++)
                result[animationFeatureIndex++] = instance->poseDescriptor[poseFeatureIndex];
        }
    }
    return valid;
}

void Character::computePoseChainReverseDescriptor(const deque<const PoseCluster *> &clusterHistory, float *result) const
{
    const int poseDims = min(Constants::poseChainReverseMaxDim, posePCADimension);

    int animationFeatureIndex = 0;
    for (int windowOffset = 0; windowOffset < Constants::poseChainReverseLength; windowOffset++)
    {
        const PoseCluster &pose = *clusterHistory[windowOffset];
        const CharacterInstance *instance = findInstanceAtFrame(pose.seedFrame);
        for (int poseFeatureIndex = 0; poseFeatureIndex < poseDims; poseFeatureIndex++)
            result[animationFeatureIndex++] = instance->poseDescriptor[poseFeatureIndex];
    }
}

bool Character::computePoseChainReverseDescriptor(const FrameID &startFrame, float *result) const
{
    const int poseDims = min(Constants::poseChainReverseMaxDim, posePCADimension);

    bool valid = true;
    int animationFeatureIndex = 0;
    for (int windowOffset = 0; windowOffset < Constants::poseChainReverseLength; windowOffset++)
    {
        const FrameID curFrame = startFrame.delta(-windowOffset);
        const CharacterInstance *instance = findInstanceAtFrame(curFrame);
        if (instance == nullptr)
        {
            valid = false;
            for (int poseFeatureIndex = 0; poseFeatureIndex < poseDims; poseFeatureIndex++)
                result[animationFeatureIndex++] = 0.0f;
        }
        else
        {
            for (int poseFeatureIndex = 0; poseFeatureIndex < poseDims; poseFeatureIndex++)
                result[animationFeatureIndex++] = instance->poseDescriptor[poseFeatureIndex];
        }
    }
    return valid;
}

void Character::computePoseChainForwardPCA()
{
    const string PCADir = learningParams().datasetDir + "poseChainForwardPCA/";
    const string PCAFile = PCADir + "character" + to_string(characterIndex) + ".dat";

    if (util::fileExists(PCAFile))
    {
        cout << "Loading animation PCA from " << PCAFile << endl;
        poseChainForwardPCA.load(PCAFile);
    }
    else
    {
        const int poseChainFeatureCount = min(Constants::poseChainForwardMaxDim, posePCADimension) * Constants::poseChainForwardLength;

        DenseMatrixf M(Constants::poseChainPCASamples, poseChainFeatureCount);

        for (int sampleIndex = 0; sampleIndex < Constants::poseChainPCASamples; sampleIndex++)
        {
            bool valid = false;
            while (!valid)
            {
                const CharacterInstance &randomInstance = *util::randomElement(allInstancesVec);
                valid = computePoseChainForwardDescriptor(randomInstance.frameID, &M(sampleIndex, 0));
            }
        }

        poseChainForwardPCA.init(M);

        util::makeDirectory(PCADir);
        poseChainForwardPCA.save(PCAFile);
    }

    poseChainForwardPCADimension = (int)poseChainForwardPCA.reducedDimension(learningParams().PCAEnergy);
    cout << "Pose chain forward PCA dimension: " << poseChainForwardPCADimension << endl;
}

void Character::computePoseChainReversePCA()
{
    const string PCADir = learningParams().datasetDir + "poseChainReversePCA/";
    const string PCAFile = PCADir + "character" + to_string(characterIndex) + ".dat";

    if (util::fileExists(PCAFile))
    {
        cout << "Loading animation PCA from " << PCAFile << endl;
        poseChainReversePCA.load(PCAFile);
    }
    else
    {
        const int poseChainFeatureCount = min(Constants::poseChainReverseMaxDim, posePCADimension) * Constants::poseChainReverseLength;

        DenseMatrixf M(Constants::poseChainPCASamples, poseChainFeatureCount);

        for (int sampleIndex = 0; sampleIndex < Constants::poseChainPCASamples; sampleIndex++)
        {
            bool valid = false;
            while (!valid)
            {
                const CharacterInstance &randomInstance = *util::randomElement(allInstancesVec);
                valid = computePoseChainReverseDescriptor(randomInstance.frameID, &M(sampleIndex, 0));
            }
        }

        poseChainReversePCA.init(M);

        util::makeDirectory(PCADir);
        poseChainReversePCA.save(PCAFile);
    }

    poseChainReversePCADimension = (int)poseChainReversePCA.reducedDimension(learningParams().PCAEnergy);
    cout << "Pose chain reverse PCA dimension: " << poseChainReversePCADimension << endl;
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
            file << instance.poseDescriptor[0] << "," << instance.poseDescriptor[1] << ",";
            //file << instance.reducedPoseChainDescriptor[0] << "," << instance.reducedPoseChainDescriptor[1];
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

/*bool Character::makeInstance(const ProcessedFrame &frame, CharacterInstance &result) const
{
    vec3f sum = vec3f::origin;
    float sumCount = 0.0f;

    for (UINT64 signature : allSegmentsVec)
    {
        if (frame.signatureMap.count(signature) > 0)
        {
            CharacterSegmentInstance &segmentInstance = result.segments[signature];
            segmentInstance.signature = signature;
            segmentInstance.worldCentroid = frame.signatureMap.find(signature)->second.front()->centroid;
            sum += segmentInstance.worldCentroid;
            sumCount += 1.0f;
        }
    }

    if (sumCount <= 0.0f)
        return false;

    result.frameID = frame.frameID;
    sum /= sumCount;
    for (auto &segmentInstance : result.segments)
    {
        segmentInstance.second.centeredCentroid = segmentInstance.second.worldCentroid - sum;
    }

    const int poseFeatureCount = 3 * (int)allSegmentsVec.size();
    vector<float> rawPoseDescriptor(poseFeatureCount);

    result.makeRawPoseDescriptor(allSegmentsVec, rawPoseDescriptor.data());
    posePCA.transform(rawPoseDescriptor, posePCADimension, result.poseDescriptor);

    const int poseChainReverseFeatureCount = min(Constants::poseChainReverseMaxDim, posePCADimension) * Constants::poseChainReverseLength;
    vector<float> rawPoseChainReverseDescriptor(poseChainReverseFeatureCount);

    for (auto &instance : allInstances)
    {
        computePoseChainReverseDescriptor(instance.first, rawPoseChainDescriptor.data());
        poseChainReversePCA.transform(rawPoseChainDescriptor, poseChainReversePCADimension, instance.second.poseChainReverseDescriptor);
    }

    return true;
}*/

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
        frameInstance.worldCentroid = sum;
        allInstances[frame.frameID] = frameInstance;
    }
}

void Character::computeAnimationSequences()
{
    cout << "*** Computing animation sequences" << endl;

    // this is just so I can focus on Marth for now
    if (characterIndex == 0)
        return;

    cout << "Filling priority queue" << endl;

    priority_queue<CharacterInstance*, vector<CharacterInstance*>, CharacterInstanceCompare> seedQueue;
    for (CharacterInstance *instance : allInstancesVec)
    {
        vector< pair<CharacterInstance*, float> > candidates = findPoseChainsForwardRadius(*instance, (float)learningParams().poseChainDistSq);
        instance->similarPoseChainCount = (int)candidates.size();
        if (instance->similarPoseChainCount >= learningParams().minAnimationInstances)
        {
            seedQueue.push(instance);
        }
    }

    cout << "Emptying priority queue" << endl;
    while (!seedQueue.empty())
    {
        CharacterInstance &seedInstance = *seedQueue.top();
        seedQueue.pop();

        if (seedInstance.animation.sequence == nullptr)
        {
            addNewAnimation(seedInstance);
        }
    }

    cout << "Animation count: " << animations.size() << endl;

    for (CharacterInstance *instance : allInstancesVec)
    {
        if (instance->animation.sequence != nullptr)
            instance->animation.sequence->instances.push_back(instance->frameID);
    }
}

void Character::addNewAnimation(const CharacterInstance &seed)
{
    const int animationLength = Constants::poseChainForwardLength;
    for (int offset = 0; offset < animationLength; offset++)
    {
        FrameID animationFrame = seed.frameID.delta(offset);
        CharacterInstance *animationInstance = findInstanceAtFrame(animationFrame);
        if (animationInstance == nullptr || animationInstance->animation.sequence != nullptr)
        {
            cout << "Animation rejected" << endl;
            return;
        }
    }

    AnimationSequence *animation = new AnimationSequence(seed.frameID, animationLength, (int)animations.size());

    int frameCount = 0;
    
    vector< pair<CharacterInstance*, float> > candidates = findPoseChainsForwardRadius(seed, (float)learningParams().poseChainDistSq);
    for (auto &candidate : candidates)
    {
        for (int offset = 0; offset < animationLength; offset++)
        {
            FrameID candidateFrame = candidate.first->frameID.delta(offset);
            CharacterInstance *otherInstance = findInstanceAtFrame(candidateFrame);

            if (otherInstance != nullptr && otherInstance->animation.sequence == nullptr)
            {
                frameCount++;
                otherInstance->animation.sequence = animation;
                otherInstance->animation.offset = offset;
                otherInstance->animation.poseDistSq = 0.0f;
            }
        }
    }
    /*for (int offset = 0; offset < animationLength; offset++)
    {
        FrameID animationFrame = seed.frameID.delta(offset);
        CharacterInstance *animationInstance = findInstanceAtFrame(animationFrame);

        for (auto pose : findPosesRadius(*animationInstance, (float)learningParams().poseDistSqThreshold))
        {
            CharacterInstance *otherInstance = pose.first;
            const float poseDistSq = pose.second;
            if (otherInstance->animation.poseDistSq > poseDistSq)
            {
                if (otherInstance->animation.sequence != animation)
                    frameCount++;
                otherInstance->animation.sequence = animation;
                otherInstance->animation.offset = offset;
                otherInstance->animation.poseDistSq = poseDistSq;
            }
        }
    }*/

    cout << "New animation: length=" << animationLength << ", estFrameCount=" << frameCount << endl;

    animations.push_back(animation);
}

void Character::makePoseClusters()
{
    cout << "Making pose clusters" << endl;

    auto &params = learningParams();
    poseClusterSearch.init(posePCADimension, (float)params.LSHpNorm, params.LSHminiHashCount, params.LSHmacroTableCount);
    for (CharacterInstance *instance : allInstancesVec)
    {
        PoseCluster *cluster = findBestPoseCluster(*instance, (float)learningParams().poseDistSqThreshold);
        if (cluster == nullptr)
        {
            cluster = new PoseCluster();
            cluster->index = (int)poseClusters.size();
            cluster->seedFrame = instance->frameID;
            poseClusters.push_back(cluster);
            poseClusterSearch.insert(instance->poseDescriptor, cluster);
        }
        instance->poseCluster = cluster;
        //cout << "poseCluster=" << instance->poseClusterIndex << endl;
        cluster->allInstances.push_back(instance->frameID);
    }
    cout << "Pose cluster count: " << poseClusters.size() << endl;
}

void Character::testAnimationSearch(float pNorm, UINT miniHashFunctionCount, UINT macroTableCount)
{
    cout << "Testing LSH " << pNorm << ", " << miniHashFunctionCount << ", " << macroTableCount << endl;
    LSHEuclidean<CharacterInstance*> search;
    search.init(poseChainForwardPCADimension, pNorm, miniHashFunctionCount, macroTableCount);
    for (auto &instance : allInstances)
        search.insert(instance.second.poseChainForwardDescriptor, &instance.second);

    const UINT testInstanceCount = 500;
    const float targetDistSq = 10.0f;
    size_t missedSamples = 0;
    size_t wastedSamples = 0;
    
    size_t totalValidSamples = 0;

    ofstream fileDebug("debug.txt");
    for (int test = 0; test < testInstanceCount; test++)
    {
        const CharacterInstance &randomInstance = *util::randomElement(allInstancesVec);

        auto results = search.findSimilar(randomInstance.poseChainForwardDescriptor);
        for (CharacterInstance *candidate : results)
        {
            const float distSq = math::distSq(candidate->poseChainForwardDescriptor, randomInstance.poseChainForwardDescriptor);
            if (distSq > targetDistSq)
                wastedSamples++;
        }

        for (auto &instance : allInstancesVec)
        {
            const float distSq = math::distSq(instance->poseChainForwardDescriptor, randomInstance.poseChainForwardDescriptor);
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

void Character::makePoseLSHSearch()
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

    cout << "Building pose LSH tables" << endl;
    auto &params = learningParams();

    poseSearch.init(posePCADimension, (float)params.LSHpNorm, params.LSHminiHashCount, params.LSHmacroTableCount);
    for (auto &instance : allInstances)
    {
        poseSearch.insert(instance.second.poseDescriptor, &instance.second);
    }

    poseChainForwardSearch.init(poseChainForwardPCADimension, (float)params.LSHpNorm, params.LSHminiHashCount, params.LSHmacroTableCount);
    for (auto &instance : allInstances)
    {
        poseChainForwardSearch.insert(instance.second.poseChainForwardDescriptor, &instance.second);
    }

    poseChainReverseSearch.init(poseChainForwardPCADimension, (float)params.LSHpNorm, params.LSHminiHashCount, params.LSHmacroTableCount);
    for (auto &instance : allInstances)
    {
        poseChainReverseSearch.insert(instance.second.poseChainReverseDescriptor, &instance.second);
    }
}

PoseCluster* Character::findBestPoseCluster(const CharacterInstance &instance, float maxDistSq) const
{
    auto candidates = poseClusterSearch.findSimilar(instance.poseDescriptor);
    
    PoseCluster *bestCluster = nullptr;
    float bestClusterDistSq = maxDistSq;

    for (auto &candidateCluster : candidates)
    {
        const CharacterInstance *candidateInstance = findInstanceAtFrame(candidateCluster->seedFrame);
        const float distSq = math::distSq(instance.poseDescriptor, candidateInstance->poseDescriptor);
        if (distSq < bestClusterDistSq)
        {
            bestCluster = candidateCluster;
            bestClusterDistSq = distSq;
        }
    }

    return bestCluster;
}

vector< pair<PoseCluster*, float> > Character::findPoseClustersRadius(const CharacterInstance &instance, float maxDistSq) const
{
    auto candidates = poseClusterSearch.findSimilar(instance.poseDescriptor);
    vector< pair<PoseCluster*, float> > result;
    for (auto &candidateCluster : candidates)
    {
        const CharacterInstance *candidateInstance = findInstanceAtFrame(candidateCluster->seedFrame);
        const float distSq = math::distSq(instance.poseDescriptor, candidateInstance->poseDescriptor);
        if (distSq <= maxDistSq)
            result.push_back(make_pair(candidateCluster, distSq));
    }
    return result;
}

vector< pair<CharacterInstance*, float> > Character::findPosesRadius(const CharacterInstance &instance, float maxDistSq) const
{
    auto candidates = poseSearch.findSimilar(instance.poseDescriptor);
    vector< pair<CharacterInstance*, float> > result;
    for (auto &candidateInstance : candidates)
    {
        const float distSq = math::distSq(instance.poseDescriptor, candidateInstance->poseDescriptor);
        if (distSq <= maxDistSq)
            result.push_back(make_pair(candidateInstance, distSq));
    }
    return result;
}

vector< pair<CharacterInstance*, float> > Character::findPoseChainsForwardRadius(const CharacterInstance &instance, float maxDistSq) const
{
    auto candidates = poseChainForwardSearch.findSimilar(instance.poseChainForwardDescriptor);
    vector< pair<CharacterInstance*, float> > result;
    for (auto &candidateInstance : candidates)
    {
        const float distSq = math::distSq(instance.poseChainForwardDescriptor, candidateInstance->poseChainForwardDescriptor);
        if (distSq <= maxDistSq)
            result.push_back(make_pair(candidateInstance, distSq));
    }
    return result;
}

vector< pair<CharacterInstance*, float> > Character::findSimilarClusterHistoryInstances(const deque<const PoseCluster *> &clusterHistory, float maxDistSq) const
{
    vector<float> rawPoseChainReverseDescriptor(poseChainReverseFeatureCount());
    vector<float> reducedDescriptor;

    computePoseChainReverseDescriptor(clusterHistory, rawPoseChainReverseDescriptor.data());

    poseChainReversePCA.transform(rawPoseChainReverseDescriptor, poseChainReversePCADimension, reducedDescriptor);

    auto candidates = poseChainReverseSearch.findSimilar(reducedDescriptor);
    vector< pair<CharacterInstance*, float> > result;
    for (auto &candidateInstance : candidates)
    {
        const float distSq = math::distSq(reducedDescriptor, candidateInstance->poseChainReverseDescriptor);
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

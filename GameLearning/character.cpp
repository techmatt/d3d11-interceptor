
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

void Character::computeAnimationDescriptor(const FrameID &centerFrame, float *result)
{
    int animationFeatureIndex = 0;
    for (int windowOffset = -Constants::animationWindowRadius; windowOffset <= Constants::animationWindowRadius; windowOffset++)
    {
        const FrameID curFrame = centerFrame.delta(windowOffset);
        const CharacterFrameInstance *instance = findInstanceAtFrame(curFrame);
        if (instance == nullptr)
        {
            for (int poseFeatureIndex = 0; poseFeatureIndex < posePCADimension; poseFeatureIndex++)
                result[animationFeatureIndex++] = 0.0f;
        }
        else
        {
            for (int poseFeatureIndex = 0; poseFeatureIndex < posePCADimension; poseFeatureIndex++)
                result[animationFeatureIndex++] = instance->reducedPoseDescriptor[poseFeatureIndex];
        }
    }
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
            const CharacterFrameInstance &randomInstance = *util::randomElement(allInstancesVec);
            computeAnimationDescriptor(randomInstance.frameID, &M(sampleIndex, 0));
        }

        animationPCA.init(M);

        util::makeDirectory(PCADir);
        animationPCA.save(PCAFile);
    }

    animationPCADimension = (int)animationPCA.reducedDimension(0.99);
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
        const int posePCASamples = 10000;
        const int poseFeatureCount = 3 * (int)allSegmentsVec.size();

        DenseMatrixf M(posePCASamples, poseFeatureCount);

        for (int sampleIndex = 0; sampleIndex < posePCASamples; sampleIndex++)
        {
            const CharacterFrameInstance &instance = *util::randomElement(allInstancesVec);
            instance.makeRawPoseDescriptor(allSegmentsVec, &M(sampleIndex, 0));

            //vector<float> descriptor = instance.makeRawPoseDescriptor(allSegmentsVec);
            //for(int featureIndex = 0; featureIndex < poseFeatureCount; featureIndex++)
            //   M(sampleIndex, featureIndex) = descriptor[featureIndex];
        }

        posePCA.init(M);

        util::makeDirectory(PCADir);
        posePCA.save(PCAFile);
    }

    posePCADimension = (int)posePCA.reducedDimension(0.99);
    cout << "Pose PCA dimension: " << posePCADimension << endl;
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

void Character::testAnimationSearch(float pNorm, UINT miniHashFunctionCount, UINT macroTableCount)
{
    cout << "Testing LSH" << endl;
    LSHEuclidean<CharacterFrameInstance*> search;
    search.init(animationPCADimension, pNorm, miniHashFunctionCount, macroTableCount);
    for (auto &instance : allInstances)
        search.insert(instance.second.reducedAnimationDescriptor, &instance.second);

    const UINT testInstanceCount = 1000;
    const float targetDistSq = 5.0f;
    size_t missedSamples = 0;
    size_t wastedSamples = 0;
    
    size_t totalValidSamples = 0;

    ofstream fileDebug("debug.txt");
    for (int test = 0; test < testInstanceCount; test++)
    {
        const CharacterFrameInstance &randomInstance = *util::randomElement(allInstancesVec);

        auto results = search.findSimilar(randomInstance.reducedAnimationDescriptor);
        for (CharacterFrameInstance *candidate : results)
        {
            const float distSq = L2DistSq(candidate->reducedAnimationDescriptor, randomInstance.reducedAnimationDescriptor);
            if (distSq > targetDistSq)
                wastedSamples++;
        }

        for (auto &instance : allInstancesVec)
        {
            const float distSq = L2DistSq(instance->reducedAnimationDescriptor, randomInstance.reducedAnimationDescriptor);
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
    for (float dist = 0.001f; dist <= 0.3f; dist *= 2.0f)
        for (int k = 10; k <= 50; k += 10)
            for (int L = 3; L <= 7; L += 4)
                testAnimationSearch(dist, k, L);
    /*animationSearch.init(animationPCADimension, 7.0f, 5, 5);
    for (auto &instance : allInstances)
    {
        animationSearch.insert(instance.second.reducedAnimationDescriptor, &instance.second);
    }*/

    
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


namespace ml {

void FASTDetector::ProcessFrame(FASTFrame &frame, int threshold)
{
    frame.corners = DetectCornersDownsample(frame.bmp, threshold);
}

vector<vec2i> FASTDetector::DetectCorners(const Bitmap &bmp, UINT threshold)
{
    vector<BYTE> image(bmp.size());
    for(UINT y = 0; y < bmp.dimY(); y++)
        for(UINT x = 0; x < bmp.dimX(); x++)
        {
            RGBColor c = bmp(y, x);
            image[y * bmp.dimY() + x] = ((int)c.r + (int)c.g + (int)c.b) / 3;
        }

    int cornerCount;
    vec2i* corners = fast::fast12_detect_nonmax(&image[0], bmp.dimX(), bmp.dimY(), bmp.dimX(), threshold, &cornerCount, _fastStorage);
    
    vector<vec2i> result(cornerCount);
    memcpy(&result[0], corners, sizeof(vec2i) * cornerCount);

    return result;
}

vector<vec2i> FASTDetector::DetectCornersDownsample(const Bitmap &bmp, UINT threshold)
{
    for(UINT y = 0; y < downsampleHeight; y++)
        for(UINT x = 0; x < downsampleWidth; x++)
        {
            int sum = 0;
            
            RGBColor c0 = bmp(y * 2 + 0, x * 2 + 0); sum += (int)c0.r + (int)c0.g + (int)c0.b;
            RGBColor c1 = bmp(y * 2 + 0, x * 2 + 1); sum += (int)c1.r + (int)c1.g + (int)c1.b;
            RGBColor c2 = bmp(y * 2 + 1, x * 2 + 0); sum += (int)c2.r + (int)c2.g + (int)c2.b;
            RGBColor c3 = bmp(y * 2 + 1, x * 2 + 1); sum += (int)c3.r + (int)c3.g + (int)c3.b;

            _downsampleStorage[y * downsampleWidth + x] = sum / 12;
        }

    int cornerCount;
    vec2i* corners = fast::fast12_detect_nonmax(&_downsampleStorage[0], downsampleWidth, downsampleHeight, downsampleWidth, threshold, &cornerCount, _fastStorage);

    vector<vec2i> result;
    result.reserve(cornerCount);
    for(int cornerIndex = 0; cornerIndex < cornerCount; cornerIndex++)
    {
        const vec2i &corner = corners[cornerIndex];
        result.push_back(vec2i(corner.x * 2, corner.y * 2));
    }

    return result;
}

Bitmap FASTDetector::VisualizeCorners( const Bitmap &bmp, const vector<vec2i> &corners, const vector<RGBColor> &cornerColors )
{
    Bitmap result = bmp;
    for(UINT cornerIndex = 0; cornerIndex < corners.size(); cornerIndex++)
    {
        RGBColor color = cornerColors[cornerIndex];
        if(color != RGBColor::Magenta) result(corners[cornerIndex].y, corners[cornerIndex].x) = color;
    }
    return result;
}

float FASTDetector::ComputeDistance(UINT cornerA, UINT cornerB, const vector<const float*> &featuresA, const vector<const float*> &featuresB)
{
    return FeatureGenerator3Channel::Diff(featuresA[cornerA], featuresB[cornerB]);
}

vector<FASTCorrespondence> FASTDetector::ComputeCorrespondences(const FASTFrame &startFrame, const FASTFrame &endFrame)
{
    static const float maxCorrespondenceDistance = 10.0f;
    static const float maxDistSq = maxCorrespondenceDistance * maxCorrespondenceDistance;
    static const UINT k = 20;
    static const float minRatio = 3.0f;

    const auto &cornersA = startFrame.corners;
    const auto &cornersB = endFrame.corners;

    const UINT cornerACount = (UINT)cornersA.size();
    const UINT cornerBCount = (UINT)cornersB.size();

    if (_featureStorageA.size() < cornerACount * FeatureGenerator3Channel::featureCount) _featureStorageA.resize(cornerACount * FeatureGenerator3Channel::featureCount);
    if (_featureStorageB.size() < cornerBCount * FeatureGenerator3Channel::featureCount) _featureStorageB.resize(cornerBCount * FeatureGenerator3Channel::featureCount);

    vector<const float*> featuresA(cornerACount);
    for(UINT cornerIndexA = 0; cornerIndexA < cornerACount; cornerIndexA++)
    {
        float *f = &_featureStorageA[0] + cornerIndexA * FeatureGenerator3Channel::featureCount;
        featuresA[cornerIndexA] = f;
        FeatureGenerator3Channel::Generate(startFrame.bmp, cornersA[cornerIndexA], f);
    }

    vector<const float*> featuresB(cornerBCount);
    for(UINT cornerIndexB = 0; cornerIndexB < cornerBCount; cornerIndexB++)
    {
        float *f = &_featureStorageB[0] + cornerIndexB * FeatureGenerator3Channel::featureCount;
        featuresB[cornerIndexB] = f;
        FeatureGenerator3Channel::Generate(endFrame.bmp, cornersB[cornerIndexB], f);
    }

    FASTKDTree2 treeA, treeB;
    treeA.BuildTree(cornersA);
    treeB.BuildTree(cornersB);
    vector<int> cornersAMap(cornerACount);
    vector<BYTE> cornersBMask(cornerBCount, 0);
    vector<UINT> neighbors;
    KNearestNeighborQueue<float> NNQueue;
    NNQueue.init(2, 1e10f);

    for(UINT cornerIndexA = 0; cornerIndexA < cornerACount; cornerIndexA++)
    {
        neighbors.clear();
        treeB.KNearest(vec2f((float)cornersA[cornerIndexA].x, (float)cornersA[cornerIndexA].y), std::min(k, cornerBCount), neighbors, 0.0f);
        NNQueue.clear(1e10f);
        for(int cornerIndexB : neighbors)
        {
            NNQueue.insert(cornerIndexB, ComputeDistance(cornerIndexA, cornerIndexB, featuresA, featuresB));
        }
        if(NNQueue.queue().size() >= 2 && NNQueue.queue()[1].dist / NNQueue.queue()[0].dist >= minRatio)
        {
            int bestIndex = NNQueue.queue()[0].index;
            cornersAMap[cornerIndexA] = bestIndex;
            cornersBMask[bestIndex] = 1;
        }
    }
    
    vector<FASTCorrespondence> correspondences;
    for(UINT cornerIndexB = 0; cornerIndexB < cornerBCount; cornerIndexB++)
    {
        if(cornersBMask[cornerIndexB] == 1)
        {
            neighbors.clear();
            treeA.KNearest(vec2f((float)cornersB[cornerIndexB].x, (float)cornersB[cornerIndexB].y), std::min(k, cornerACount), neighbors, 0.0f);
            NNQueue.clear(1e10f);
            for(int cornerIndexA : neighbors)
            {
                NNQueue.insert(cornerIndexA, ComputeDistance(cornerIndexA, cornerIndexB, featuresA, featuresB));
            }
            if(NNQueue.queue().size() >= 2)
            {
                int bestIndex = NNQueue.queue()[0].index;
                float ratio = NNQueue.queue()[1].dist / NNQueue.queue()[0].dist;
                if(cornersAMap[bestIndex] == cornerIndexB && ratio > 2.0f && dist(cornersA[bestIndex], cornersB[cornerIndexB]) <= maxCorrespondenceDistance)
                {
                    correspondences.push_back(FASTCorrespondence(cornersA[bestIndex], cornersB[cornerIndexB]));
                }
            }
        }
    }
    return correspondences;
}

}
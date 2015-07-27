
#include "main.h"

FrameAlignmentCluster FrameProcessing::alignFrames(const FrameObjectData &source, const FrameObjectData &dest)
{
    const int correspondenceGroupSize = 3;
    const float clusterThreshold = 0.1f;
    const bool outputClusters = true;

    map<UINT64, const LocalizedObjectData*> destMap = dest.makeSignatureMap();
    
    ofstream file("clusters.txt");

    auto attemptAlignment = [&](int startSourceIndex)
    {
        vector<vec3f> sourcePoints(correspondenceGroupSize), destPoints(correspondenceGroupSize);
        for (int i = 0; i < correspondenceGroupSize; i++)
        {
            const auto &o = source.objects[startSourceIndex + i];
            if (destMap.count(o.signature) == 0)
                return make_pair(false, mat4f::identity());

            sourcePoints[i] = o.centroid;
            destPoints[i] = destMap[o.signature]->centroid;
        }
        const mat4f result = EigenWrapperf::kabsch(sourcePoints, destPoints);

        return make_pair(true, result);
    };

    auto compareMatrices = [](const mat4f &a, const mat4f &b)
    {
        float result = 0.0f;
        for (int i = 0; i < 16; i++)
            result = max(result, fabs(a.matrix[i] - b.matrix[i]));
        return result;
    };

    vector<mat4f> candidateAlignments;
    for (int sourceIndex = 0; sourceIndex < source.objects.size() - correspondenceGroupSize; sourceIndex++)
    {
        auto result = attemptAlignment(sourceIndex);
        if (result.first)
            candidateAlignments.push_back(result.second);
    }

    if (candidateAlignments.size() == 0)
    {
        FrameAlignmentCluster result;
        result.size = 0;
        return result;
    }

    vector<FrameAlignmentCluster> clusters;
    while (candidateAlignments.size() > 0)
    {
        vector<mat4f> unmatchedAlignments;

        FrameAlignmentCluster cluster;
        cluster.transform = candidateAlignments[0];
        cluster.size = 1;

        for (int i = 1; i < candidateAlignments.size(); i++)
        {
            const float dist = compareMatrices(cluster.transform, candidateAlignments[i]);
            file << dist << endl;
            if (dist < clusterThreshold)
            {
                cluster.size++;
            }
            else
            {
                unmatchedAlignments.push_back(candidateAlignments[i]);
            }
        }

        clusters.push_back(cluster);
        candidateAlignments = unmatchedAlignments;
    }

    sort(clusters.begin(), clusters.end(), [](const FrameAlignmentCluster &a, const FrameAlignmentCluster &b) { return a.size > b.size; });

    if (outputClusters)
    {
        for (auto &c : clusters)
        {
            file << "size = " << c.size << endl;
            file << c.transform << endl << endl;
        }
    }

    return clusters[0];
}

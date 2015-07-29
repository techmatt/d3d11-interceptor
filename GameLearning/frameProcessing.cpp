
#include "main.h"

void FrameProcessing::alignAllFrames(const FrameCollection &frames)
{
    ofstream file("alignment.txt");

    const int minClusterSize = 1;

    for (int frameIndex = 0; frameIndex < frames.frames.size() - 1; frameIndex++)
    {
        FrameObjectData &frameA = *frames.frames[frameIndex];
        FrameObjectData &frameB = *frames.frames[frameIndex + 1];

        FrameAlignmentCluster alignment = alignFrames(frameA, frameB);

        file << "align " << frameIndex + 1 << " to " << frameIndex << ", size=" << alignment.size << endl;
        file << "transform:" << endl << alignment.transform << endl << endl;
        
        if (alignment.size >= minClusterSize)
        {
            frameB.transform(alignment.transform.getInverse());
        }
    }
}

vector<FrameAlignmentCorrespondence> FrameProcessing::getCorrespondences(const FrameObjectData &source, const FrameObjectData &dest)
{
    vector<FrameAlignmentCorrespondence> result;

    const float minObjectSize = 20.0f;
    
    auto getMaxDim = [](const vec3f &v)
    {
        return max(max(v.x, v.y), v.z);
    };

    map<UINT64, const LocalizedObjectData*> destMap = dest.makeUniqueSignatureMap();

    for (const LocalizedObjectData &o : source.objects)
    {
        if (destMap.count(o.signature) != 0 && destMap[o.signature] != nullptr &&
            getMaxDim(destMap[o.signature]->bbox.getExtent()) >= minObjectSize)
        {
            FrameAlignmentCorrespondence correspondence;
            correspondence.source = &o;
            correspondence.dest = destMap[o.signature];
            result.push_back(correspondence);
        }
    }

    return result;
}

FrameAlignmentCluster FrameProcessing::alignFrames(const FrameObjectData &source, const FrameObjectData &dest)
{
    const int correspondenceGroupSize = 6;
    const float clusterThreshold = 0.1f;
    const bool outputClusters = true;

    ofstream file;
    if(outputClusters)
        file.open("clusters.txt");

    const vector<FrameAlignmentCorrespondence> correspondences = getCorrespondences(source, dest);

    auto attemptAlignment = [&](int correspondenceStartIndex)
    {
        vector<vec3f> sourcePoints(correspondenceGroupSize), destPoints(correspondenceGroupSize);
        for (int i = 0; i < correspondenceGroupSize; i++)
        {
            const auto &correspondence = correspondences[correspondenceStartIndex + i];
            sourcePoints[i] = correspondence.source->centroid;
            destPoints[i] = correspondence.dest->centroid;
        }

        mat3f m(sourcePoints[0], sourcePoints[1], sourcePoints[2]);
        mat3f m2 = m.getTranspose() * m;
        auto system = m2.eigenSystem();

        mat4f alignment = EigenWrapperf::kabsch(sourcePoints, destPoints);
        return alignment;
    };

    auto compareMatrices = [](const mat4f &a, const mat4f &b)
    {
        float result = 0.0f;
        for (int i = 0; i < 16; i++)
            result = max(result, fabs(a.matrix[i] - b.matrix[i]));
        return result;
    };

    vector<mat4f> candidateAlignments;
    for (int sourceIndex = 0; sourceIndex < correspondences.size() - correspondenceGroupSize; sourceIndex++)
    {
        const mat4f alignment = attemptAlignment(sourceIndex);
        candidateAlignments.push_back(alignment);

        if (fabs(alignment(0, 3) - -30.9165f) < 0.01f)
        {
            const mat4f alignment2 = attemptAlignment(sourceIndex);
        }
    }

    if (outputClusters)
    {
        file << candidateAlignments.size() << " candidates" << endl << endl;
        for (const mat4f &m : candidateAlignments)
        {
            file << m << endl << endl;
        }
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

        if (outputClusters) file << endl << "new cluster" << endl;
        for (int i = 1; i < candidateAlignments.size(); i++)
        {
            const float dist = compareMatrices(cluster.transform, candidateAlignments[i]);

            if (outputClusters) file << "dist vs " << i << ": " << dist << endl;
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

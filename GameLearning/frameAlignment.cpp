
#include "main.h"

mat4f FrameAlignment::alignObjects(const LocalizedObjectData &source, const LocalizedObjectData &dest)
{
    int vertexCount = source.getVertexCount();
    
    //MLIB_ASSERT_STR(vertexCount == dest.getVertexCount(), "inconsistent vertices");

    if (vertexCount <= 2 || vertexCount != dest.getVertexCount())
        return mat4f::identity();
    
    vector<vec3f> sourcePoints(vertexCount), destPoints(vertexCount);
    for (int i = 0; i < vertexCount; i++)
    {
        sourcePoints[i] = source.vertices[i];
        destPoints[i] = dest.vertices[i];
    }

    vec3f eigenvalues;
    mat4f alignment = EigenWrapperf::kabsch(sourcePoints, destPoints, eigenvalues);
    return alignment;
}

void FrameAlignment::alignAllFrames(const GameReplay &frames)
{
    //ofstream file("logs/alignment.txt");

    const int minClusterSize = 4;

    for (int frameIndex = 0; frameIndex < frames.frames.size() - 1; frameIndex++)
    {
        FrameObjectData &frameA = *frames.frames[frameIndex];
        FrameObjectData &frameB = *frames.frames[frameIndex + 1];

        FrameAlignmentCluster alignment = alignFrames(frameA, frameB);

        //file << "align " << frameIndex + 1 << " to " << frameIndex << ", size=" << alignment.size << endl;
        //file << "transform:" << endl << alignment.transform << endl << endl;
        
        if (alignment.size >= minClusterSize)
        {
            frameB.transform(alignment.transform.getInverse());
        }
    }
}

vector<FrameAlignmentCorrespondence> FrameAlignment::getCorrespondences(const FrameObjectData &source, const FrameObjectData &dest)
{
    vector<FrameAlignmentCorrespondence> result;

    const float minObjectSize = 20.0f;
    
    auto getMaxDim = [](const vec3f &v)
    {
        return max(max(v.x, v.y), v.z);
    };

    auto destMap = dest.makeSignatureMap();

    for (const LocalizedObjectData &o : source.objectData)
    {
        if (destMap.count(o.signature) != 0 && destMap[o.signature].size() == 1 &&
            getMaxDim(destMap[o.signature].front()->bbox.getExtent()) >= minObjectSize)
        {
            FrameAlignmentCorrespondence correspondence;
            correspondence.source = &o;
            correspondence.dest = destMap[o.signature].front();
            result.push_back(correspondence);
        }
    }

    return result;
}

FrameAlignmentCluster FrameAlignment::alignFrames(const FrameObjectData &source, const FrameObjectData &dest)
{
    const int correspondenceGroupSize = 6;
    const float clusterThreshold = 0.1f;
    const bool outputClusters = false;

    ofstream file;
    if(outputClusters)
        file.open("logs/alignmentClusters.txt");

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

        vec3f eigenvalues;
        mat4f alignment = EigenWrapperf::kabsch(sourcePoints, destPoints, eigenvalues);
        return alignment;
    };

    auto compareMatrices = [](const mat4f &a, const mat4f &b)
    {
        float result = 0.0f;
        for (int i = 0; i < 16; i++)
            result = max(result, fabs(a.matrix[i] - b.matrix[i]));
        return result;
    };

    if (correspondences.size() < correspondenceGroupSize)
    {
        FrameAlignmentCluster result;
        result.size = 0;
        result.transform = mat4f::identity();
        return result;
    }

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

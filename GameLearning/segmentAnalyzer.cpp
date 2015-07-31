
#include "main.h"

void SegmentAnalyzer::analyze(const FrameCollection &frames)
{
    cout << "Analyzing " << frames.frames.size() << " frames" << endl;
    for (int frameIndex = 1; frameIndex < frames.frames.size(); frameIndex++)
    {
        const auto &prevSigMap = frames.frames[frameIndex - 1]->makeCompleteSignatureMap();
        const auto &sigMap = frames.frames[frameIndex]->makeCompleteSignatureMap();

        for (const auto &sig : sigMap)
        {
            auto &segment = segments[sig.first];
            segment.signature = sig.first;
            segment.frameCount++;
            segment.instancesPerFrame.record((double)sig.second.size());

            for (const auto &instance : sig.second)
            {
                segment.dimensions.record(instance->bbox.getExtent().length());

                if (sig.second.size() == 1 &&
                    prevSigMap.count(sig.first) > 0 &&
                    prevSigMap.find(sig.first)->second.size() == 1)
                {
                    const vec3f delta = instance->centroid - prevSigMap.find(sig.first)->second[0]->centroid;
                    segment.distMoved.record(delta.length());
                }
            }
        }
    }
    
    cout << "Making segment graph" << endl;
    makeSegmentGraph(frames);

    cout << "Making character labels" << endl;
    assignCharacterLabels();
}

void SegmentAnalyzer::dump(const string &filename)
{
    ofstream file(filename);
    file << "Signature\tFrames\tInstances per frame\tDimensions\tDist Moved" << endl;
    for (const auto &segment : segments)
    {
        file << segment.first << '\t';
        file << segment.second.frameCount << '\t';
        file << segment.second.instancesPerFrame.toString() << '\t';
        file << segment.second.dimensions.toString() << '\t';
        file << segment.second.distMoved.toString() << endl;
    }
}

void SegmentAnalyzer::makeSegmentGraph(const FrameCollection &frames)
{
    map<UINT64, size_t> signatureToNode;

    for (auto &segment : segments)
    {
        if (segment.second.trackableSegment())
        {
            signatureToNode[segment.first] = segmentGraph.nodes().size();
            segmentGraph.addNode(&segment.second);
        }
    }

    const int n = (int)segmentGraph.nodes().size();
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            segmentGraph.addEdge(i, j);
        }
    }
    cout << "Segment graph nodes=" << n << ", edges=" << segmentGraph.edges().size() << endl;

    for (int frameIndex = 0; frameIndex < frames.frames.size(); frameIndex++)
    {
        const auto &sigMap = frames.frames[frameIndex]->makeCompleteSignatureMap();

        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                auto &nodeA = segmentGraph.nodes()[i];
                auto &nodeB = segmentGraph.nodes()[j];
                const bool aPresent = (sigMap.count(nodeA.data->signature) > 0);
                const bool bPresent = (sigMap.count(nodeB.data->signature) > 0);

                if (aPresent || bPresent)
                {
                    const int cooccurrence = aPresent && bPresent ? 1 : 0;

                    auto &edge = segmentGraph.getEdge(i, j);
                    edge.data.cooccurenceFrequency.record(cooccurrence);

                    if (aPresent && bPresent)
                    {
                        const vec3f &centroidA = sigMap.find(nodeA.data->signature)->second[0]->centroid;
                        const vec3f &centroidB = sigMap.find(nodeB.data->signature)->second[0]->centroid;
                        float dist = vec3f::dist(centroidA, centroidB);
                        edge.data.cooccurenceDist.record(dist);
                    }
                }
            }
        }
    }
    cout << "Done processing segment cooccurrence" << endl;
}

void SegmentAnalyzer::assignCharacterLabels()
{
    const int minCharacterComponents = 5;

    auto edgeTest = [](const SegmentEdge &edge)
    {
        if (edge.cooccurenceDist.mean() > 7.0f)
            return false;
        if (edge.cooccurenceFrequency.mean() < 0.9f)
            return false;
        return true;
    };

    cout << "Computing connected components" << endl;
    const auto components = segmentGraph.computeConnectedComponents(edgeTest);
    int characterIndex = 0;
    for (int componentIndex = 0; componentIndex < components.size(); componentIndex++)
    {
        const vector<UINT> &component = components[componentIndex];
        cout << "Component " << componentIndex << " has " << component.size() << " vertices";

        if (component.size() < minCharacterComponents)
        {
            cout << " (discarded)" << endl;
            continue;
        }

        characterSegments.push_back(vector<CharacterSegment>());
        auto &character = characterSegments.back();
        for (UINT nodeIndex : component)
        {
            SegmentStats &segment = *segmentGraph.nodes()[nodeIndex].data;
            segment.characterLabel = characterIndex;
            character.push_back(CharacterSegment(segment.signature));
        }
        characterIndex++;
        cout << endl;
    }
}

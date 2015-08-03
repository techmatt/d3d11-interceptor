
#include "main.h"

void SegmentAnalyzer::save(const string &filename) const
{
    BinaryDataStreamFile file(filename, true);

    file << segments << characterSegments;
    
    file.closeStream();
}

void SegmentAnalyzer::load(const string &filename)
{
    BinaryDataStreamFile file(filename, false);

    file >> segments >> characterSegments;
    
    file.closeStream();
}

void SegmentAnalyzer::analyze(const ReplayDatabase &database)
{
    const string &cacheFilename = learningParams().datasetDir + "segments.dat";
    if (util::fileExists(cacheFilename))
    {
        cout << "Loading segments from " << cacheFilename << endl;
        load(cacheFilename);
        return;
    }

    cout << "*** Computing segment statistics" << endl;
    for (const ReplayDatabaseEntry &entry : database.entries)
    {
        cout << "Recording frames for " << entry.replay->sourceFilename << endl;
        for (auto &pair : entry.pairs)
        {
            recordSegmentTracking(pair);
        }
    }

    cout << "*** Making segment graph" << endl;
    makeTrackableSegmentGraph(database);

    cout << "*** Making character labels" << endl;
    assignCharacterLabels();

    save(cacheFilename);
}

void SegmentAnalyzer::recordSegmentTracking(const FramePair &pair)
{
    for (const auto &sig : pair.f1->signatureMap)
    {
        auto &segment = segments[sig.first];
        segment.signature = sig.first;
        segment.frameCount++;
        segment.instancesPerFrame.record((double)sig.second.size());

        for (const auto &instance : sig.second)
        {
            segment.dimensions.record(instance->bbox.getExtent().length());

            if (sig.second.size() == 1 &&
                pair.f0->signatureMap.count(sig.first) > 0 &&
                pair.f0->signatureMap.find(sig.first)->second.size() == 1)
            {
                const vec3f delta = instance->centroid - pair.f0->signatureMap.find(sig.first)->second[0]->centroid;
                segment.distMoved.record(delta.length());
            }
        }
    }
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

void SegmentAnalyzer::makeTrackableSegmentGraph(const ReplayDatabase &database)
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

    for (const ReplayDatabaseEntry &entry : database.entries)
    {
        cout << "Recording trackable segments for " << entry.replay->sourceFilename << endl;
        for (auto &frame : entry.processedFrames)
        {
            recordTrackableSegmentObservations(frame);
        }
    }

    cout << "Done constructing segment graph" << endl;
}

void SegmentAnalyzer::recordTrackableSegmentObservations(const ProcessedFrame& frame)
{
    const int n = (int)segmentGraph.nodes().size();
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            auto &nodeA = segmentGraph.nodes()[i];
            auto &nodeB = segmentGraph.nodes()[j];
            const bool aPresent = (frame.signatureMap.count(nodeA.data->signature) > 0);
            const bool bPresent = (frame.signatureMap.count(nodeB.data->signature) > 0);

            if (aPresent || bPresent)
            {
                const int cooccurrence = aPresent && bPresent ? 1 : 0;

                auto &edge = segmentGraph.getEdge(i, j);
                edge.data.cooccurenceFrequency.record(cooccurrence);

                if (aPresent && bPresent)
                {
                    const vec3f &centroidA = frame.signatureMap.find(nodeA.data->signature)->second[0]->centroid;
                    const vec3f &centroidB = frame.signatureMap.find(nodeB.data->signature)->second[0]->centroid;
                    float dist = vec3f::dist(centroidA, centroidB);
                    edge.data.cooccurenceDist.record(dist);
                }
            }
        }
    }
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
        const auto &component = components[componentIndex];
        cout << "Component " << componentIndex << " has " << component.size() << " vertices";

        if (component.size() < minCharacterComponents)
        {
            cout << " (discarded)" << endl;
            continue;
        }

        characterSegments.push_back(vector<UINT64>());
        auto &character = characterSegments.back();
        for (auto &node : component)
        {
            SegmentStats &segment = *node->data;
            segment.characterLabel = characterIndex;
            character.push_back(segment.signature);
        }
        characterIndex++;
        cout << endl;
    }
}

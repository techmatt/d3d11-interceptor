
#include "main.h"

bool SegmentAnalyzer::segmentsSoftMatch(const SegmentAnimation &segmentA, const SegmentAnimation &segmentB)
{
    return (segmentA.color == segmentB.color);
}

void SegmentAnalyzer::init(const SegmentManager &segments)
{
    for (auto &p : segments.segmentsByHash)
    {
        graph.addNode(p.second);
    }
}

void SegmentAnalyzer::recordFramePair(const SegmentManager &segments, const ReplayFrame &frameA, const ReplayFrame &frameB)
{
    for (const ReplayAnnotation &frameAInst : frameA.annotations)
    {
        recordObjectMatch(segments, frameAInst, frameB);
    }
}

void SegmentAnalyzer::recordObjectMatch(const SegmentManager &segments, const ReplayAnnotation &frameAInst, const ReplayFrame &frameB)
{
    const SegmentAnimation &segmentA = *segments.getSegment(frameAInst.segmentHash);
    
    for (const ReplayAnnotation &frameBInst : frameB.annotations)
    {
        int xDiff = abs(frameBInst.origin.x - frameAInst.origin.x);
        int yDiff = abs(frameBInst.origin.y - frameAInst.origin.y);
        if (xDiff > learningParams().objectMotionRadius || yDiff > learningParams().objectMotionRadius)
            continue;

        const SegmentAnimation &segmentB = *segments.getSegment(frameBInst.segmentHash);

        if (segmentsSoftMatch(segmentA, segmentB))
        {
            if (!graph.hasEdge(segmentA.index, segmentB.index))
                graph.addEdge(segmentA.index, segmentB.index);

            graph.getEdge(segmentA.index, segmentB.index).data.count++;
        }
    }
}

vector<GameObject*> SegmentAnalyzer::extractObjects()
{
    vector< vector<const SegmentGraph::Node*> > components = graph.computeConnectedComponents();

    cout << "Component count: " << components.size() << endl;

    vector<GameObject*> result;
    for (const vector<const SegmentGraph::Node*> &component : components)
    {
        cout << "Component size: " << component.size() << endl;
        GameObject *newObject = new GameObject();

        for (const SegmentGraph::Node *segment : component)
        {
            newObject->segments.push_back(segment->data);
        }
        newObject->index = (int)result.size();

        result.push_back(newObject);
    }
    return result;
}

#include "main.h"

bool SegmentAnalyzer::segmentsSoftMatch(const SegmentAnimation &segmentA, const SegmentAnimation &segmentB)
{
    const UINT64 hashA = AtariUtil::animationHash(segmentA.mask, 0);
    const UINT64 hashB = AtariUtil::animationHash(segmentB.mask, 0);
    
    const bool horzLineA = (segmentA.dimensions.y == 1);
    const bool horzLineB = (segmentB.dimensions.y == 1);
    //if (horzLineA != horzLineB)
    //    return false;

    return (segmentA.color == segmentB.color ||
            (hashA == hashB && segmentA.mask.size() >= 10));
}

void SegmentAnalyzer::init(const SegmentDatabase &segments)
{
    for (auto &p : segments.segmentsByHash)
    {
        graph.addNode(p.second);
    }
}

void SegmentAnalyzer::recordFramePair(const SegmentDatabase &segments, const ReplayFrame &frameA, const ReplayFrame &frameB)
{
    for (const SegmentAnnotation &frameAInst : frameA.segmentAnnotations)
    {
        recordObjectMatch(segments, frameAInst, frameB);
    }
}

void SegmentAnalyzer::recordObjectMatch(const SegmentDatabase &segments, const SegmentAnnotation &frameAInst, const ReplayFrame &frameB)
{
    const SegmentAnimation &segmentA = *segments.getSegment(frameAInst.segmentHash);
    
    for (const SegmentAnnotation &frameBInst : frameB.segmentAnnotations)
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
            segment->data->object = newObject;
        }
        newObject->index = (int)result.size();

        result.push_back(newObject);
    }
    return result;
}

void SegmentAnalyzer::annotateObjects(ReplayFrame &frame)
{
    frame.objectAnnotations.clear();
    int segmentIndex = 0;
    for (const SegmentAnnotation &s : frame.segmentAnnotations)
    {
        ObjectAnnotation newObject(s.origin);
        newObject.segments.push_back(segmentIndex);
        frame.objectAnnotations.push_back(newObject);
        segmentIndex++;
    }
}

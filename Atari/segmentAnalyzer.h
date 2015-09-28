
struct SegmentEdge
{
    SegmentEdge()
    {
        count = 0;
    }
    int count;
};

typedef UndirectedGraph<SegmentAnimation*, SegmentEdge> SegmentGraph;

struct SegmentAnalyzer
{
    void init(const SegmentManager &segments);
    void recordFramePair(const SegmentManager &segments, const ReplayFrame &frameA, const ReplayFrame &frameB);
    
    vector<GameObject*> extractObjects();
    void annotateObjects(ReplayFrame &frame);

    SegmentGraph graph;

private:
    void recordObjectMatch(const SegmentManager &segments, const SegmentAnnotation &frameAObject, const ReplayFrame &frameB);

    static bool segmentsSoftMatch(const SegmentAnimation &segmentA, const SegmentAnimation &segmentB);
};

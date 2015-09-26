
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

    SegmentGraph graph;

private:
    void recordObjectMatch(const SegmentManager &segments, const ReplayAnnotation &frameAObject, const ReplayFrame &frameB);

    static bool segmentsSoftMatch(const SegmentAnimation &segmentA, const SegmentAnimation &segmentB);
};

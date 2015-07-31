
struct StatTracker
{
    StatTracker()
    {
        sum = 0.0;
        sumSquared = 0.0;
        count = 0;
    }

    void record(double value)
    {
        sum += value;
        sumSquared += value * value;
        count++;
    }

    double mean() const
    {
        if (count == 0) return 0.0;
        return sum / (double)count;
    }

    double stdDev() const
    {
        if (count == 0) return 0.0;
        return sumSquared / (double)count;
    }

    string toString() const
    {
        return to_string(mean()) + " (" + to_string(count) + ")";
    }

private:
    double sum;
    double sumSquared;
    int count;
};

struct SegmentStats
{
    SegmentStats()
    {
        signature = (UINT64)-1;
        characterLabel = -1;
        frameCount = 0;
    }
    bool trackableSegment() const
    {
        return (frameCount > 0 &&
            fabs(instancesPerFrame.mean() - 1.0) < 1e-4 &&
            dimensions.mean() < 30.0f &&
            distMoved.mean() > 0.1f);
    }

    UINT64 signature;
    int frameCount;
    int characterLabel;

    StatTracker instancesPerFrame;
    StatTracker triCount;
    StatTracker dimensions;
    StatTracker distMoved;
};

struct SegmentEdge
{
    StatTracker cooccurenceFrequency;
    StatTracker cooccurenceDist;
};

typedef UndirectedGraph<SegmentStats*, SegmentEdge> SegmentGraph;

struct SegmentAnalyzer
{
    void analyze(const FrameCollection &frames);
    void dump(const string &filename);

    map<UINT64, SegmentStats> segments;
    SegmentGraph segmentGraph;
    vector< vector<CharacterSegment> > characterSegments;

private:
    void makeSegmentGraph(const FrameCollection &frames);
    void assignCharacterLabels();
};
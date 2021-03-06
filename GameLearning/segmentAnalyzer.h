
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

struct SegmentStats : public BinaryDataSerialize< SegmentStats >
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
    void analyze(const ReplayDatabase &database);

    void dump(const string &filename);

    unordered_map<UINT64, SegmentStats> segments;
    SegmentGraph segmentGraph;
    vector< vector<UINT64> > characterSegments;

private:
    void save(const string &filename) const;
    void load(const string &filename);

    void recordSegmentTracking(const FramePair &frames);
    void recordTrackableSegmentObservations(const ProcessedFrame& frame);

    void makeTrackableSegmentGraph(const ReplayDatabase &database);
    void assignCharacterLabels();
};
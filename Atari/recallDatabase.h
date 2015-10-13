
struct ObjectHistory
{
    void computeHash();

    bool alive;
    vec2s velocity;
    vector<BYTE> contactStates;
    vector<BYTE> lineConstraints;

    UINT64 hash;
};

struct HistorySlotInfo
{
    vector<LineConstraint> lines;
    vector<string> objectNames;
};

struct ObjectTransition
{
    ObjectTransition()
    {
        nextAnimation = 0;
        nextAlive = 0;
    }
    UINT64 hash()
    {
        return ml::util::hash64(*this);
    }
    UINT64 nextAnimation;
    UINT nextAlive;
    vec2s velocity;
};

struct ObjectSample
{
    string toString() const;

    FrameID frame;
    ObjectHistory history;
    ObjectTransition transition;
    int nextAction;
};

struct HistoryMetricWeights
{
    HistoryMetricWeights()
    {
        velocity = 3.0f;
        action = 1.0f;
        animation = 0.0f;
        position = 0.0f;

        lines.push_back(LineConstraint(33, false, 100.0f));
        lines.push_back(LineConstraint(34, false, 100.0f));
        lines.push_back(LineConstraint(191, false, 100.0f));
        lines.push_back(LineConstraint(192, false, 100.0f));

        lines.push_back(LineConstraint(3, true, 100.0f));
        lines.push_back(LineConstraint(156, true, 100.0f));
    }
    float velocity;
    float action;
    float animation;
    float position;
    vector<LineConstraint> lines;
};

struct ObjectSampleDataset
{
    ObjectSampleDataset(const string &_objectName)
    {
        objectName = _objectName;
    }
    void dumpToFile(const string &filename) const;

    vector<ObjectSample*> getTransitionCandidates(const ObjectHistory &history) const;
    ObjectTransition predictTransitionSingleton(AppState &state, const ReplayDatabase &replays, int testReplayIndex, const vector<Game::StateInst> &states, int baseFrameIndex, int action, const HistoryMetricWeights &metric) const;

    string objectName;
    map< UINT64, vector<ObjectSample*> > samplesByHash;
    vector<ObjectSample*> allSamples;
};

struct RecallDatabase
{
    void init(AppState &state);

    static ObjectTransition computeObjectTransitionSingleton(const vector<Game::StateInst> &states, int baseFrameIndex, const string &objectName);
    static ObjectHistory computeObjectHistorySingleton(AppState &appState, const vector<Game::StateInst> &states, int baseFrameIndex, const string &objectName, const HistorySlotInfo &slotInfo);

    void predictAllTransitions(AppState &state, const ReplayDatabase &replays, int testReplayIndex, const vector<Game::StateInst> &states, const string &objectName, const string &filename);

    map<string, ObjectSampleDataset*> objectSamples;

private:
    ObjectSampleDataset* makeObjectSampleDataset(AppState &state, const string &objectName);
};

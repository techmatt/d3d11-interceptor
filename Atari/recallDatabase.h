
struct ObjectHistory
{
    struct Entry
    {
        vec2s origin;
        vec2s velocity;
        UINT64 animation;
        UINT alive;
    };
    
    void computeHashes();
    
    UINT64 combinedHash() const
    {
        return velocityHash + animationHash;
    }
    UINT64 velocityHash, animationHash;

    // history 0 = current frame
    vector<Entry> history;

private:
    UINT64 computeVelocityAliveHash() const;
    UINT64 computeAnimationHash() const;
};

struct ObjectTransition
{
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
    FrameID frame;
    ObjectHistory history;
    ObjectTransition transition;
};

struct HistoryMetricWeights
{
    float action;
    float animation;
};

struct ObjectSampleDataset
{
    vector<ObjectSample*> getTransitionCandidates(const ObjectHistory &history) const;
    ObjectTransition predictTransitionSingleton(const ReplayDatabase &replays, const vector<Game::StateInst> &states, int baseFrameIndex, const string &objectName, const HistoryMetricWeights &metric) const;

    map< UINT64, vector<ObjectSample*> > historyByCombinedHash;
    map< UINT64, vector<ObjectSample*> > historyByVelocityHash;
};

struct RecallDatabase
{
    void init(AppState &state);

    static ObjectTransition computeObjectTransitionSingleton(const vector<Game::StateInst> &states, int baseFrameIndex, const string &objectName);
    static ObjectHistory computeObjectHistorySingleton(const vector<Game::StateInst> &states, int baseFrameIndex, const string &objectName);

    void predictAllTransitions(const ReplayDatabase &replays, const vector<Game::StateInst> &states, const string &objectName, const string &filename);

    map<string, ObjectSampleDataset*> objectSamples;

private:
    static float compareVelocityHistory(const ObjectHistory &a, const ObjectHistory &b);
    ObjectSampleDataset* makeObjectSampleDataset(AppState &state, const string &objectName);
};


struct MCTSNode
{
    MCTSNode()
    {
        parentAction = (Action)-1;
        rewardSum = 0.0;
        visitCount = 1;

        index = -1;
        parentIndex = -1;
        childrenStart = -1;
    }

    bool isLeaf() const
    {
        return (childrenStart == -1);
    }

    double rewardEstimate() const
    {
        return rewardSum / visitCount;
    }

    Action parentAction;
    double rewardSum;
    double visitCount;

    // indices into allNodes
    INT64 index;
    INT64 parentIndex;
    INT64 childrenStart;
};

struct MCTSParams
{
    MCTSParams()
    {
        explorationConstant = 1.0f;
        actionChunkSize = 4;
        maxActionDepth = 200;
        iterations = 1000;
    }
    int actionCount;
    int actionChunkSize;
    int maxActionDepth;
    int iterations;
    vector<Action> actions;

    float explorationConstant;
};

struct MCTSMutableState
{
    virtual void act(Action action) = 0;
    virtual void resetState() = 0;
    virtual bool gameOver() const = 0;

    int rewardSum;
    int actionsTaken;
};

struct MCTSMutableStateALE : public MCTSMutableState
{
    MCTSMutableStateALE(ALEInterface *_ale)
    {
        ale = _ale;
        savedState = ale->cloneState();
    }
    void act(Action action)
    {
        if (ale->game_over())
            return;
        rewardSum += ale->act(action);
        actionsTaken++;
    }
    bool gameOver() const
    {
        return ale->game_over();
    }
    void resetState()
    {
        ale->restoreState(savedState);
        rewardSum = 0;
        actionsTaken = 0;
    }

    ALEState savedState;
    ALEInterface *ale;
};

struct MCTSMutableStateRecall : public MCTSMutableState
{
    MCTSMutableStateRecall(AppState *_appState, const vector<Game::StateInst> &stateHistory)
    {
        appState = _appState;
        savedStateHistory = stateHistory;
        curStateHistory = stateHistory;
    }
    void act(Action action);
    bool gameOver() const
    {
        return false;
    }
    void resetState()
    {
        curStateHistory = savedStateHistory;
        rewardSum = 0;
        actionsTaken = 0;
    }

    vector<Game::StateInst> savedStateHistory;
    vector<Game::StateInst> curStateHistory;
    AppState *appState;
};

struct MCTS
{
    void init(const MCTSParams &_params, MCTSMutableState *_mutableState);
    void go();
    void iterate();
    Action bestAction();
    void describeActions();

private:
    MCTSNode* select(MCTSNode *startNode);
    MCTSNode* expand(MCTSNode *node);
    void act(Action action);
    void simulate();
    void backpropagate(MCTSNode *leafNode, int rewardSum);

    MCTSParams params;
    MCTSMutableState *mutableState;

    MCTSNode* root()
    {
        return &allNodes[0];
    }

    vector< MCTSNode > allNodes;
};

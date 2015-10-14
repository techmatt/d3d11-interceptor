
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
    }
    int actionCount;
    int actionChunkSize;
    int maxActionDepth;
    vector<Action> actions;

    float explorationConstant;
};

interface MCTSBaseState
{
public:
    virtual ~MCTSBaseState() {}
};

struct MCTSBaseStateALE : public MCTSBaseState
{
    ALEState state;
};

struct MCTSMutableState
{
    virtual void act(Action a);
    virtual void reset(const MCTSBaseState *baseState);
    virtual bool gameOver() const;

    int rewardSum;
    int actionsTaken;
};

struct MCTSMutableStateALE : public MCTSMutableState
{
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
    void reset(const MCTSBaseState *baseState)
    {
        const MCTSBaseStateALE *state = dynamic_cast<const MCTSBaseStateALE*>(baseState);
        ale->restoreState(state->state);
        rewardSum = 0;
        actionsTaken = 0;
    }
    
    ALEInterface *ale;
};

struct MCTS
{
    void init(const MCTSParams &_params, const MCTSBaseState *_baseState, MCTSMutableState *_mutableState);
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
    const MCTSBaseState *baseState;
    MCTSMutableState *mutableState;

    MCTSNode* root()
    {
        return &allNodes[0];
    }

    vector< MCTSNode > allNodes;
};

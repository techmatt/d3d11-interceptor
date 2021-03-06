
#include "main.h"

void MCTSMutableStateRecall::act(Action action)
{
    Game::StateInst nextState;
    appState->model.advance(*appState, -1, curStateHistory, action, nextState);
    curStateHistory.push_back(nextState);

    rewardSum += nextState.reward;
    actionsTaken++;
}

void MCTS::init(const MCTSParams &_params, MCTSMutableState *_mutableState)
{
    params = _params;
    mutableState = _mutableState;
    mutableState->resetState();

    allNodes.clear();

    MCTSNode rootNode;
    rootNode.index = 0;
    rootNode.parentIndex = -1;
    allNodes.push_back(rootNode);
}

void MCTS::go()
{
    for (int i = 0; i < params.iterations; i++)
    {
        if (i % 10 == 0)
            cout << "MCTS iteration " << i + 1 << " / " << params.iterations << endl;
        iterate();
    }
}

void MCTS::iterate()
{
    MCTSNode *node = root();
    while (!node->isLeaf())
    {
        node = select(node);
        act(node->parentAction);
    }
    MCTSNode *child = expand(node);
    act(child->parentAction);

    simulate();

    backpropagate(child, mutableState->rewardSum);

    mutableState->resetState();
}

void MCTS::backpropagate(MCTSNode *leafNode, int rewardSum)
{
    if (leafNode->parentIndex == -1)
        return;
    MCTSNode *parentNode = &allNodes[leafNode->parentIndex];
    parentNode->rewardSum += rewardSum;
    parentNode->visitCount++;
    backpropagate(parentNode, rewardSum);
}

void MCTS::simulate()
{
    int rewardSum = 0;
    while (!mutableState->gameOver() && mutableState->actionsTaken < params.maxActionDepth)
    {
        const Action randomAction = params.actions[rand() % params.actionCount];
        mutableState->act(randomAction);
    }
}

void MCTS::act(Action action)
{
    for (int i = 0; i < params.actionChunkSize; i++)
        mutableState->act(action);
}

MCTSNode* MCTS::select(MCTSNode *startNode)
{
    //MLIB_ASSERT_STR(!startNode->isLeaf(), "select called on leaf");
    
    MCTSNode *bestChild = nullptr;
    double bestChildScore = -numeric_limits<double>::max();
    for (int childIndex = 0; childIndex < params.actionCount; childIndex++)
    {
        MCTSNode *child = &allNodes[startNode->childrenStart + childIndex];
        
        const double childScore = child->rewardEstimate() + params.explorationConstant * sqrt(log(startNode->visitCount) / child->visitCount);
        if (childScore > bestChildScore)
        {
            bestChild = child;
            bestChildScore = childScore;
        }
    }
    return bestChild;
}

MCTSNode* MCTS::expand(MCTSNode *node)
{
    //MLIB_ASSERT_STR(node->isLeaf(), "expand called on non-leaf");
    node->childrenStart = allNodes.size();
    INT64 parentIndex = node->index;
    for (int childIndex = 0; childIndex < params.actionCount; childIndex++)
    {
        MCTSNode childNode;
        childNode.index = allNodes.size();
        childNode.parentIndex = parentIndex;
        childNode.parentAction = params.actions[childIndex];
        allNodes.push_back(childNode);
    }
    const INT64 randomChildIndex = allNodes[parentIndex].childrenStart + rand() % params.actionCount;
    return &allNodes[randomChildIndex];
}

void MCTS::describeActions()
{
    for (int childIndex = 0; childIndex < params.actionCount; childIndex++)
    {
        MCTSNode *child = &allNodes[root()->childrenStart + childIndex];
        cout << "child " << childIndex <<
                " " << AtariUtil::toString(params.actions[childIndex]) <<
                " score=" << child->rewardEstimate() <<
                " visits=" << child->visitCount << endl;
    }
}

Action MCTS::bestAction()
{
    MCTSNode *child = select(root());
    return child->parentAction;
}

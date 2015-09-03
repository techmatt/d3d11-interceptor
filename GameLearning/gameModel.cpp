
#include "main.h"

void GameModel::advanceGameState(GameState &state, const StateTransition &transition, const ControllerState &nextController)
{
    for (int characterIndex = 0; characterIndex < GameState::CharacterCount; characterIndex++)
    {
        CharacterState &characterState = state.characters[characterIndex];
        
        characterState.valid = true;

        characterState.poseHistory.push_front(transition.character[characterIndex].newCluster);
        characterState.poseHistory.pop_back();

        characterState.worldDerivativeHistory.push_front(transition.character[characterIndex].worldPosDelta);
        characterState.worldDerivativeHistory.pop_back();

        characterState.worldPos += characterState.worldDerivativeHistory[0];
    }

    state.controllerHistory.push_front(nextController);
    state.controllerHistory.pop_back();
}

void GameModel::predictTransition(const CharacterDatabase &characterDatabase, const GameState &state, StateTransition &transition)
{
    const Character &characterEntry = characterDatabase.characters[1];
    auto candidates = characterEntry.findSimilarClusterHistoryInstances(state.characters[0].poseHistory, (float)learningParams().poseChainDistSq);

    transition.character[0].newCluster = characterEntry.poseClusters[0];
    transition.character[0].worldPosDelta = vec3f::origin;

    for (auto &candidate : candidates)
    {
        const CharacterInstance *nextInstance = characterEntry.findInstanceAtFrame(candidate.first->frameID.delta(1));
        if (nextInstance != nullptr)
        {
            transition.character[0].newCluster = nextInstance->poseCluster;
            transition.character[0].worldPosDelta = nextInstance->worldCentroid - candidate.first->worldCentroid;
        }
    }

    
}
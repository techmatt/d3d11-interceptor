
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
    for (int characterIndex = 0; characterIndex < GameState::CharacterCount; characterIndex++)
    {
        if (characterIndex == 0) continue;

        const Character &characterEntry = characterDatabase.characters[characterIndex];

        auto candidates = characterEntry.findSimilarClusterHistoryInstances(state.characters[characterIndex].poseHistory, (float)learningParams().poseChainDistSq);

        transition.character[characterIndex].newCluster = characterEntry.poseClusters[0];
        transition.character[characterIndex].worldPosDelta = vec3f::origin;

        cout << "Prediction candidates: " << candidates.size() << endl;

        for (auto &candidate : candidates)
        {
            const CharacterInstance *nextInstance = characterEntry.findInstanceAtFrame(candidate.first->frameID.delta(1));
            if (nextInstance != nullptr)
            {
                transition.character[characterIndex].newCluster = nextInstance->poseCluster;
                transition.character[characterIndex].worldPosDelta = nextInstance->worldCentroid - candidate.first->worldCentroid;
            }
        }

        cout << "Predicted cluster index: " << transition.character[characterIndex].newCluster->index << endl;
        cout << "Predicted world pos delta: " << transition.character[characterIndex].worldPosDelta << endl;
    }
}
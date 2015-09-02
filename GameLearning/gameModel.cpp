
#include "main.h"

void GameModel::advanceGameState(GameState &state, const StateTransitionData &transition, const ControllerState &nextController)
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
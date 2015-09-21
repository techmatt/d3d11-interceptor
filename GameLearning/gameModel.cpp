
#include "main.h"

void GameModel::advanceGameState(GameState &state, const StateTransition &transition, const ControllerState &nextController)
{
    for (int characterIndex = 0; characterIndex < GameState::CharacterCount; characterIndex++)
    {
        CharacterState &characterState = state.characters[characterIndex];
        
        characterState.valid = true;

        characterState.poseHistory.push_front(transition.character[characterIndex].newCluster);
        characterState.poseHistory.pop_back();

        characterState.velocity.history.push_front(transition.character[characterIndex].worldPosDelta);
        characterState.velocity.history.pop_back();

        characterState.worldPos += characterState.velocity.history[0];
    }

    state.controllers.history.push_front(nextController);
    state.controllers.history.pop_back();
}

vector<PredictionEntry> GameModel::candidateTransitions(const ReplayDatabase &replays, const CharacterDatabase &characterDatabase, const GameState &state, int characterIndex)
{
    const Character &characterEntry = characterDatabase.characters[characterIndex];

    const auto candidates = characterEntry.findSimilarClusterHistoryInstances(
        state.characters[characterIndex].poseHistory,
        learningParams().predictionReverseChainDistSq);

    cout << "Prediction candidates: " << candidates.size() << endl;

    vector<PredictionEntry> entries;

    for (auto &candidate : candidates)
    {
        const CharacterInstance *nextInstance = characterEntry.findInstanceAtFrame(candidate.first->frameID.delta(1));
        if (nextInstance != nullptr && nextInstance->frameID.replayIndex != replays.entries.size() - 1)
        {
            PredictionEntry entry;
            entry.baseFrameID = candidate.first->frameID;
            entry.controllers = ControllerHistory(entry.baseFrameID, replays);
            entry.velocity = VelocityHistory(entry.baseFrameID, replays, characterEntry);
            entry.poseChainDistSq = candidate.second;

            // TODO: learn mapping from controller to player
            entry.controllerDist = compareControllerHistory(entry.controllers, state.controllers, 0);

            entry.velocityDist = compareVelocityHistory(entry.velocity, state.characters[characterIndex].velocity);

            entry.transition.newCluster = nextInstance->poseCluster;
            entry.transition.worldPosDelta = nextInstance->worldCentroid - candidate.first->worldCentroid;
            entries.push_back(entry);
        }
    }

    return entries;
}

PredictionEntry GameModel::predictTransition(const ReplayDatabase &replays, const CharacterDatabase &characterDatabase, const GameState &state, StateTransition &transition, int predictionIndex)
{
    PredictionEntry chosenEntry;
    for (int characterIndex = 0; characterIndex < GameState::CharacterCount; characterIndex++)
    {
        if (characterIndex == 0) continue;

        const Character &characterEntry = characterDatabase.characters[characterIndex];

        transition.character[characterIndex].newCluster = characterEntry.poseClusters[0];
        transition.character[characterIndex].worldPosDelta = vec3f::origin;

        vector<PredictionEntry> entries = candidateTransitions(replays, characterDatabase, state, characterIndex);
        
        if (entries.size() == 0)
        {
            cout << "No valid entries" << endl;
            continue;
        }

        stable_sort(entries.begin(), entries.end());

        const PredictionEntry &entry = entries[math::clamp(predictionIndex, 0, (int)entries.size() - 1)];
        transition.character[characterIndex] = entry.transition;

        chosenEntry = entry;


        cout << "Predicted cluster index: " << transition.character[characterIndex].newCluster->index << endl;
        cout << "Predicted world pos delta: " << transition.character[characterIndex].worldPosDelta << endl;
    }

    return chosenEntry;
}

float GameModel::compareControllerState(const ControllerState &a, const ControllerState &b, int controllerIndex)
{
    float sum = 0.0f;
    auto &aCtrl = a.controllers[controllerIndex];
    auto &bCtrl = b.controllers[controllerIndex];

    for (int buttonIndex = 0; buttonIndex < ControllerState::ControllerButtonCount; buttonIndex++)
        if (aCtrl.buttons[buttonIndex] != bCtrl.buttons[buttonIndex])
            sum += 1.0f;
    sum += fabs(aCtrl.stick.x - bCtrl.stick.x);
    sum += fabs(aCtrl.stick.y - bCtrl.stick.y);
    return sum;
}

float GameModel::compareControllerHistory(const ControllerHistory &a, const ControllerHistory &b, int controllerIndex)
{
    float sum = 0.0f;
    for (int historyIndex = 0; historyIndex < a.history.size(); historyIndex++)
    {
        sum += compareControllerState(a.history[historyIndex], b.history[historyIndex], controllerIndex);
    }
    return sum;
}

float GameModel::compareVelocityHistory(const VelocityHistory &a, const VelocityHistory &b)
{
    float sum = 0.0f;
    for (int historyIndex = 0; historyIndex < a.history.size(); historyIndex++)
    {
        sum += vec3f::dist(a.history[historyIndex], b.history[historyIndex]);
    }
    return sum;
}


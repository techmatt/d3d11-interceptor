
struct PredictionEntry
{
    // this is the current frame, which is judged to be similar to the current game state.
    // baseFrameID + 1 is the next, predicted frame.
    FrameID baseFrameID;

    double combinedDist() const
    {
        //return controllerDist * 100.0 + velocityDist;
        return controllerDist * 100.0;
    }

    float poseChainDistSq;
    float controllerDist;
    float velocityDist;

    ControllerHistory controllers;
    VelocityHistory velocity;

    CharacterStateTransition transition;
};

inline bool operator < (const PredictionEntry &a, const PredictionEntry &b)
{
    return a.combinedDist() < b.combinedDist();
}

struct GameModel
{
    static void advanceGameState(GameState &state, const StateTransition &transition, const ControllerState &nextController);

    vector<PredictionEntry> candidateTransitions(const ReplayDatabase &replays, const CharacterDatabase &characterDatabase, const GameState &state, int characterIndex);
    PredictionEntry predictTransition(const ReplayDatabase &replays, const CharacterDatabase &characterDatabase, const GameState &state, StateTransition &transition, int transitionIndex);

private:
    float compareControllerHistory(const ControllerHistory &a, const ControllerHistory &b, int controllerIndex);
    float compareControllerState(const ControllerState &a, const ControllerState &b, int controllerIndex);
    
    float compareVelocityHistory(const VelocityHistory &a, const VelocityHistory &b);
};

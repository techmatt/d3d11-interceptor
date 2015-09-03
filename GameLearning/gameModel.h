
struct PredictionEntry
{
    // this is the current frame, which is judged to be similar to the current game state.
    // baseFrameID + 1 is the next, predicted frame.
    FrameID baseFrameID;

    float poseChainDistSq;

    ControllerHistory controllers;
};

struct GameModel
{
    static void advanceGameState(GameState &state, const StateTransition &transition, const ControllerState &nextController);

    void predictTransition(const CharacterDatabase &characterDatabase, const GameState &state, StateTransition &transition);
};

struct GameModel
{
    static void advanceGameState(GameState &state, const StateTransition &transition, const ControllerState &nextController);

    void predictTransition(const CharacterDatabase &characterDatabase, const GameState &state, StateTransition &transition);
};
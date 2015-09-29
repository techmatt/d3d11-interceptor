
namespace Game
{

enum VariablePattern
{
    // variable holds steady
    VariablePatternStationary,

    // variable increments every N frames
    VariablePatternPeriodicChange,

    // variable pulses from 0 to M every N frames
    VariablePatternPeriodicPulse,

    // variable instantaneously changes by M
    VariablePatternInstantaneousDelta,

    // variable instantaneously changes from 0 to M then back to 0
    VariablePatternInstantaneousBurst,

    // variable jumps to M
    VariablePatternJumpTo,

    // variable blinks between 0 and M every N frames
    VariablePatternBlink,
};

struct VariablePatternData
{
    VariablePattern type;

    int magnitude;

    int frequency;
};

struct ModelLearner
{
    static void loadReplayStates(AppState &state, const Replay &replay, const Model &model, vector<StateInst> &states);
    static void annotateVariablePatterns(AppState &state, vector<StateInst> &states);
};

}
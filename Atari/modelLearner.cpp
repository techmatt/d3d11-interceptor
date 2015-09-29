
#include "main.h"

namespace Game
{

    void ModelLearner::loadReplayStates(AppState &state, const Replay &replay, const Model &model, vector<StateInst> &states)
{
    const int frameCount = (int)replay.frames.size();
    states.resize(frameCount);

    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        model.loadObjects(state, state.objectAnalyzer, *replay.frames[frameIndex], states[frameIndex]);
        model.readVariables(state.segmentDatabase, states[frameIndex]);
    }
}

}
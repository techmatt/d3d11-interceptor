
#include "main.h"

namespace Game
{

void ModelLearner::loadReplayStates(AppState &state, const Replay &replay, const Model &model, vector<StateInst> &states)
{
    const int frameCount = (int)replay.frames.size();
    states.resize(frameCount);

    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        const ReplayFrame &prevFrame = *replay.frames[max(frameIndex - 1, 0)];
        const ReplayFrame &frame = *replay.frames[frameIndex];
        model.loadObjects(state, state.objectAnalyzer, frame, states[frameIndex]);
        model.readVariables(state.segmentDatabase, states[frameIndex]);

        states[frameIndex].action = prevFrame.action;
        states[frameIndex].reward = frame.reward;
    }
}

}

#include "main.h"

void ReplayDatabase::init()
{
    
}

ReplayFrame& ReplayDatabase::getFrame(FrameID id)
{
    return *replays[id.replayIndex]->replay->frames[id.frameIndex];
}

const ReplayFrame& ReplayDatabase::getFrame(FrameID id) const
{
    return *replays[id.replayIndex]->replay->frames[id.frameIndex];
}

void ReplayDatabase::loadAnnotatedReplays()
{
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    for (const string &filename : Directory::enumerateFilesWithPath(replayAnnotatedDir, ".dat"))
    {
        cout << "Loading " << filename << endl;
        
        AnnotatedReplay *replay = new AnnotatedReplay();
        replay->replay = new Replay();
        replay->replay->load(filename);
        replay->replay->index = (int)replays.size();
        replays.push_back(replay);
    }
}

void ReplayDatabase::loadGameStates(AppState &state)
{
    for (auto &replay : replays)
    {
        Game::ModelLearner::loadReplayStates(state, *replay->replay, state.model, replay->states);
    }
}

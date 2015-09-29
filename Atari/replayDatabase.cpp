
#include "main.h"

void ReplayDatabase::init()
{
    
}

ReplayFrame& ReplayDatabase::getFrame(FrameID id)
{
    return *replays[id.replayIndex]->frames[id.frameIndex];
}

const ReplayFrame& ReplayDatabase::getFrame(FrameID id) const
{
    return *replays[id.replayIndex]->frames[id.frameIndex];
}

void ReplayDatabase::loadAnnotatedReplays()
{
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    for (const string &filename : Directory::enumerateFilesWithPath(replayAnnotatedDir, ".dat"))
    {
        cout << "Loading " << filename << endl;
        
        Replay *replay = new Replay();
        replay->load(filename);
        replay->index = (int)replays.size();
        replays.push_back(replay);
    }
}

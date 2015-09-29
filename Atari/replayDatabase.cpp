
#include "main.h"

void ReplayDatabase::init()
{
    
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

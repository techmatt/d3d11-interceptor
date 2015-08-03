
#include "main.h"

void ReplayDatabase::addEntry(const string &alignedFrameFilename)
{
    cout << "Loading " << alignedFrameFilename << endl;
    
    ReplayDatabaseEntry *newEntry = new ReplayDatabaseEntry;

    newEntry->replayIndex = (int)entries.size();
    newEntry->replay = new GameReplay;
    newEntry->replay->load(alignedFrameFilename);
    
    const size_t frameCount = newEntry->replay->frames.size();
    newEntry->processedFrames.resize(frameCount);
    newEntry->pairs.resize(frameCount - 1);

    string fileTag = util::fileNameFromPath(util::removeExtensions(alignedFrameFilename));

    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        FrameObjectData *frame = newEntry->replay->frames[frameIndex];
        newEntry->processedFrames[frameIndex] = ProcessedFrame(frame, FrameID(newEntry->replayIndex, frameIndex));
    }

    for (int frameIndex = 0; frameIndex < frameCount - 1; frameIndex++)
    {
        FramePair pair;
        pair.f0 = &newEntry->processedFrames[frameIndex];
        pair.f1 = &newEntry->processedFrames[frameIndex + 1];
        newEntry->pairs[frameIndex] = pair;
    }

    entries.push_back(*newEntry);
}

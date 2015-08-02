
#include "main.h"

void FrameDatabase::addEntry(const string &alignedFrameFilename)
{
    cout << "Loading " << alignedFrameFilename << endl;
    
    FrameDatabaseEntry *newEntry = new FrameDatabaseEntry;

    newEntry->collection = new FrameCollection;
    newEntry->collection->load(alignedFrameFilename);
    
    const size_t frameCount = newEntry->collection->frames.size();
    newEntry->processedFrames.resize(frameCount);
    newEntry->pairs.resize(frameCount - 1);

    string fileTag = util::fileNameFromPath(util::removeExtensions(alignedFrameFilename));

    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        FrameObjectData *frame = newEntry->collection->frames[frameIndex];
        const string frameID = fileTag + "-" + to_string(frameIndex);
        newEntry->processedFrames[frameIndex] = ProcessedFrame(frame, frameID);
    }

    for (int frameIndex = 0; frameIndex < frameCount - 1; frameIndex++)
    {
        FramePair pair;
        pair.f0 = &newEntry->processedFrames[frameIndex];
        pair.f1 = &newEntry->processedFrames[frameIndex + 1];
        newEntry->pairs[frameIndex] = pair;
    }
}
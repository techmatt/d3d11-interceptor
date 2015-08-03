
#include "main.h"

void GameReplay::save(const string &filename) const
{
    BinaryDataStreamFile file(filename, true);

    file << frames.size();
    for (FrameObjectData *frame : frames)
    {
        file << frame->objectData << frame->objectMeshes;
    }
    file.closeStream();
}

void GameReplay::load(const string &filename)
{
    sourceFilename = filename;

    BinaryDataStreamFile file(filename, false);

    size_t frameCount = -1;
    file >> frameCount;

    frames.resize(frameCount);
    for (int i = 0; i < frameCount; i++)
    {
        FrameObjectData *frame = new FrameObjectData();
        file >> frame->objectData >> frame->objectMeshes;
        frame->index = i;
        frames[i] = frame;
    }

    file.closeStream();
}

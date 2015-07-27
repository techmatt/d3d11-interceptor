
#include "main.h"

void ObjectCollection::save(const string &filename) const
{
    BinaryDataStreamFile file(filename, true);
    file << objects;
    file.closeStream();
}

void ObjectCollection::load(const string &filename)
{
    BinaryDataStreamFile file(filename, false);
    file >> objects;
    file.closeStream();
}

void FrameCollection::save(const string &filename) const
{
    BinaryDataStreamFile file(filename, true);

    file << frames.size();
    for (FrameObjectData *frame : frames)
    {
        file << frame->objects << frame->objectMeshes;
    }
    file.closeStream();
}

void FrameCollection::load(const string &filename)
{
    BinaryDataStreamFile file(filename, false);

    size_t frameCount = -1;
    file >> frameCount;

    frames.resize(frameCount);
    for (int i = 0; i < frameCount; i++)
    {
        FrameObjectData *frame = new FrameObjectData();
        file >> frame->objects >> frame->objectMeshes;
        frames[i] = frame;
    }

    file.closeStream();
}

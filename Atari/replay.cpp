
#include "main.h"

void Replay::clearImages()
{
    for (ReplayFrame *frame : frames)
    {
        frame->image.data.free();
    }
}

void Replay::save(const string &filename) const
{
    BinaryDataStreamVector stream;

    stream << id << romName << frames.size();
    for (ReplayFrame *frame : frames)
    {
        int action = frame->action;
        int reward = frame->reward;
        stream << action;
        stream << reward;

        stream.writePrimitiveContainer(frame->image.data);
        stream.writePrimitiveVector(frame->segmentAnnotations);
        stream << frame->objectAnnotations;
    }
    
    const vector<BYTE> &data = stream.getBuffer().getData();
    ZLibWrapper::CompressStreamToFile(data.data(), data.size(), filename);
}

void Replay::load(const string &filename)
{
    vector<BYTE> decompressedStream = ZLibWrapper::DecompressStreamFromFile(filename);

    BinaryDataStreamVector stream;
    stream.writePrimitiveVector(decompressedStream);

    UINT64 sizeHeader;
    stream >> sizeHeader;

    size_t frameCount = -1;
    stream >> id >> romName >> frameCount;

    frames.resize(frameCount);
    for (int i = 0; i < frameCount; i++)
    {
        ReplayFrame *frame = new ReplayFrame();
        
        int action, reward;
        stream >> action;
        stream >> reward;
        frame->action = (Action)action;
        frame->reward = reward;

        stream.readPrimitiveContainer(frame->image.data);
        stream.readPrimitiveVector(frame->segmentAnnotations);
        stream >> frame->objectAnnotations;

        frame->index = i;
        frames[i] = frame;
    }
}

void ReplayFrame::updateObjectIDs(const SegmentManager &database)
{
    for (ObjectAnnotation &o : objectAnnotations)
    {
        const UINT64 segmentHash = segmentAnnotations[o.segments[0]].segmentHash;
        SegmentAnimation *segment = database.getSegment(segmentHash);
        o.objectID = (segment == nullptr || segment->object == nullptr) ? -1 : segment->object->index;
    }
}

void Replay::updateObjectIDs(const SegmentManager &database)
{
    for (ReplayFrame *frame : frames)
    {
        frame->updateObjectIDs(database);
    }
}

void AtariImage::fromScreen(const ALEScreen &screen)
{
    const int width = (int)screen.width();
    const int height = (int)screen.height();
    data.allocate(width, height);
    for (auto &d : data)
    {
        d.value = screen.get((int)d.y, (int)d.x);
    }
}

Bitmap AtariImage::toBmp(const ColourPalette &palette) const
{
    Bitmap result;
    toBmp(palette, result);
    return result;
}

void AtariImage::toBmp(const ColourPalette &palette, Bitmap &bmpOut) const
{
    if (bmpOut.getWidth() != data.getDimX() || bmpOut.getHeight() != data.getDimY())
        bmpOut.allocate((int)data.getDimX(), (int)data.getDimY());

    for (auto &p : bmpOut)
    {
        p.value = AtariUtil::getAtariColor(data(p.x, p.y), palette);

        //if (p.x == 0 || p.y == 0 || p.x == state.aleScreen.getWidth() - 1 || p.y == state.aleScreen.getHeight() - 1)
        //    p.value = vec4uc(255, 255, 255, 255);
    }
}

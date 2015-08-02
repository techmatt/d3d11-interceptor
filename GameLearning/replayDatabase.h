
struct ProcessedFrame
{
    ProcessedFrame() {}
    ProcessedFrame(FrameObjectData *_frame, const string &_frameID)
    {
        frame = _frame;
        frameID = _frameID;
        signatureMap = frame->makeSignatureMap();
    }

    string frameID;
    FrameObjectData *frame;
    map<UINT64, vector<const LocalizedObjectData*> > signatureMap;
};

struct FramePair
{
    const ProcessedFrame *f0;
    const ProcessedFrame *f1;
};

struct FrameDatabaseEntry
{
    vector<FramePair> pairs;
    vector<ProcessedFrame> processedFrames;

    FrameCollection *collection;
};

struct FrameDatabase
{
    void addEntry(const string &alignedFrameFilename);

    vector< reference_wrapper<FrameDatabaseEntry> > entries;
};

struct FrameAlignmentCluster
{
    mat4f transform;
    int size;
};

struct FrameAlignmentCorrespondence
{
    const LocalizedObjectData *source;
    const LocalizedObjectData *dest;
};

class FrameAlignment
{
public:
    static void alignAllFrames(const GameReplay &frames);

    static mat4f alignObjects(const LocalizedObjectData &source, const LocalizedObjectData &dest);

    static vector<FrameAlignmentCorrespondence> getCorrespondences(const FrameObjectData &source, const FrameObjectData &dest);
    static FrameAlignmentCluster alignFrames(const FrameObjectData &source, const FrameObjectData &dest);
};

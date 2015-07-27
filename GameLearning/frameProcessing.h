
struct FrameAlignmentCluster
{
    mat4f transform;
    int size;
};

class FrameProcessing
{
public:
    static FrameAlignmentCluster alignFrames(const FrameObjectData &source, const FrameObjectData &dest);
};

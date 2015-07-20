
class GameAI : public GameAIInterface
{
public:
    void newFrameStart(long _frameIndex);

    void drawIndexed(const GameAIConstantBuffer &VSConstants, const GameAIConstantBuffer &PSConstants, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation);

    long frameIndex;
};

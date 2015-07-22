
struct GameAIConstantBuffer
{
    float *data;
    UINT floatCount;
};

class GameAIInterface
{
public:
    virtual void newFrameStart(long _frameIndex) = 0;

    virtual void drawIndexed(const GameAIConstantBuffer &VSConstants, const GameAIConstantBuffer &PSConstants, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation) = 0;
};

GameAIInterface* GameAICreate();


#include "main.h"

GameAIInterface* GameAICreate()
{
    return new GameAI();
}

void GameAI::newFrameStart(long _frameIndex)
{
    frameIndex = _frameIndex;
}

void GameAI::drawIndexed(const GameAIConstantBuffer &VSConstants, const GameAIConstantBuffer &PSConstants, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{

}

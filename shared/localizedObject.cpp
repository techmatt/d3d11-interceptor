
#include "main.h"

void LocalizedObject::loadFromDrawIndexed(MyD3DAssets &assets, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
    vertices.clear();

    assets.loadVSConstantBuffer();
    const auto &indexBuffer = assets.getActiveIndexBuffer();
    const auto &vertexBuffer = assets.getActiveVertexBuffer();

    if (vertexBuffer.buffer != nullptr && indexBuffer.buffer != nullptr && assets.activeVertexLayout != nullptr && assets.activeVertexLayout->positionOffset != -1)
    {
        const WORD *indexDataStart = (WORD *)indexBuffer.buffer->data.data() + StartIndexLocation;
        const BYTE *vertexData = vertexBuffer.buffer->data.data();

        for (int indexIndex = 0; indexIndex < (int)IndexCount; indexIndex++)
        {
            const int curIndex = indexDataStart[indexIndex] + BaseVertexLocation;
            const BYTE *curVertex = (vertexData + (vertexBuffer.stride * curIndex));
            
            if (vertexBuffer.buffer->data.size() < vertexBuffer.stride * (curIndex + 1))
            {
                continue;
            }

            const int pOffset = assets.activeVertexLayout->positionOffset;
            const int cOffset = assets.activeVertexLayout->colorOffset;

            const float *pStart = (const float *)(curVertex + pOffset);
            
            LocalizedObjectVertex localizedVertex;

            const vec3f basePos(pStart[0], pStart[1], pStart[2]);
            localizedVertex.worldPos = assets.transformObjectToWorldGamecube(basePos);
            
            localizedVertex.color = vec4uc(0, 0, 0, 0);
            if (cOffset != -1)
            {
                localizedVertex.color = *((const vec4uc *)(curVertex + cOffset));
            }

            vertices.push_back(localizedVertex);
        }
    }
}
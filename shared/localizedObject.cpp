
#include "main.h"

struct VertexShaderConstants
{
    vec4f posnormalmatrix[6];
    vec4f projection[4];
    vec4i materials[4];
    struct Light
    {
        vec4i color;
        vec4f cosatt;
        vec4f distatt;
        vec4f pos;
        vec4f dir;
    } lights[8];
    vec4f texmatrices[24];
    vec4f transformmatrices[64];
    vec4f normalmatrices[32];
    vec4f posttransformmatrices[64];
    vec4f pixelcentercorrection;
};

void LocalizedObject::loadFromDrawIndexed(MyD3DAssets &assets, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
    drawIndex = g_logger->frameRenderIndex;

    vertices.clear();

    assets.loadVSConstantBuffer();
    //assets.loadPSTexture(0);
    const auto &indexBuffer = assets.getActiveIndexBuffer();
    const auto &vertexBuffer = assets.getActiveVertexBuffer();

    VertexShaderConstants &shaderConstants = *((VertexShaderConstants *)assets.VSBufferStorage.data());
    for (int i = 0; i < 4; i++)
        materials.push_back(shaderConstants.materials[i]);

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
            const int tOffset = assets.activeVertexLayout->tex0Offset;

            const float *pStart = (const float *)(curVertex + pOffset);
            
            LocalizedObjectVertex localizedVertex;

            const vec3f basePos(pStart[0], pStart[1], pStart[2]);
            localizedVertex.worldPos = assets.transformObjectToWorldGamecube(basePos);

            if (localizedVertex.worldPos.x != localizedVertex.worldPos.x) localizedVertex.worldPos = vec3f(0.0f, 0.0f, 0.0f);
            
            localizedVertex.color = vec4uc(255, 255, 255, 255);
            if (cOffset != -1)
            {
                localizedVertex.color = *((const vec4uc *)(curVertex + cOffset));
            }

            if (tOffset != -1 && assets.PSTexture.size() > 0)
            {
                //localizedVertex.color = assets.PSTexture(0.5f, 0.5f);
            }

            vertices.push_back(localizedVertex);
        }
    }
}
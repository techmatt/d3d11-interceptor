
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

struct PixelShaderConstants
{
    vec4i colors[4];
    vec4i kcolors[4];
    vec4i alpha;
    vec4f texdims[8];
    vec4i zbias[2];
    vec4i indtexscale[2];
    vec4i indtexmtx[6];
    vec4i fogcolor;
    vec4i fogi;
    vec4f fogf[2];
    vec4f zslope;
    vec4f efbscale;
};

void LocalizedObject::load(MyD3DAssets &assets, const DrawParameters &params)
{
    drawIndex = g_logger->frameRenderIndex;

    vertices.clear();
    indices.clear();

    assets.loadVSConstantBuffer();

    if (params.BaseVertexLocation == -1)
        loadFromDraw(assets, params.IndexCount, params.StartIndexLocation);
    else
        loadFromDrawIndexed(assets, params.IndexCount, params.StartIndexLocation, params.BaseVertexLocation);
}

void LocalizedObject::loadFromDraw(MyD3DAssets &assets, UINT  VertexCount, UINT  StartVertexLocation)
{
    const auto &vertexBuffer = assets.getActiveVertexBuffer();

    if (vertexBuffer.buffer != nullptr && assets.activeVertexLayout != nullptr && assets.activeVertexLayout->positionOffset != -1)
    {
        const BYTE *vertexData = vertexBuffer.buffer->data.data();

        for (int indexIndex = 0; indexIndex < (int)VertexCount; indexIndex++)
        {
            const int curIndex = StartVertexLocation + indexIndex;
            const BYTE *curVertex = (vertexData + (vertexBuffer.stride * curIndex));

            LocalizedObjectVertex localizedVertex;
            localizedVertex.worldPos = vec3f::origin;

            if (vertexBuffer.buffer->data.size() < vertexBuffer.stride * (curIndex + 1))
            {
                vertices.push_back(localizedVertex);
                indices.push_back(curIndex);
                continue;
            }

            const int pOffset = assets.activeVertexLayout->positionOffset;
            const int bOffset = assets.activeVertexLayout->blendOffset;

            const float *pStart = (const float *)(curVertex + pOffset);

            int blendMatrixIndex = -1;
            if (bOffset != -1)
            {
                vec4uc blendIndices = *((const vec4uc *)(curVertex + bOffset));
                blendMatrixIndex = blendIndices.x;
            }

            const vec3f basePos(pStart[0], pStart[1], pStart[2]);
            localizedVertex.worldPos = assets.transformObjectToWorldGamecube(basePos, blendMatrixIndex);

            if (basePos.x == 0.0f || localizedVertex.worldPos.x != localizedVertex.worldPos.x)
                localizedVertex.worldPos = vec3f(0.0f, 0.0f, 0.0f);

            vertices.push_back(localizedVertex);
            indices.push_back(curIndex);
        }
    }
}

void LocalizedObject::loadFromDrawIndexed(MyD3DAssets &assets, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
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
            
            LocalizedObjectVertex localizedVertex;
            localizedVertex.worldPos = vec3f::origin;

            if (vertexBuffer.buffer->data.size() < vertexBuffer.stride * (curIndex + 1))
            {
                vertices.push_back(localizedVertex);
                indices.push_back(curIndex);
                continue;
            }

            const int pOffset = assets.activeVertexLayout->positionOffset;
            const int bOffset = assets.activeVertexLayout->blendOffset;
            
            const float *pStart = (const float *)(curVertex + pOffset);

            int blendMatrixIndex = -1;
            if (bOffset != -1)
            {
                vec4uc blendIndices = *((const vec4uc *)(curVertex + bOffset));
                blendMatrixIndex = blendIndices.x;
            }

            const vec3f basePos(pStart[0], pStart[1], pStart[2]);
            localizedVertex.worldPos = assets.transformObjectToWorldGamecube(basePos, blendMatrixIndex);

            if (basePos.x == 0.0f || localizedVertex.worldPos.x != localizedVertex.worldPos.x)
                localizedVertex.worldPos = vec3f(0.0f, 0.0f, 0.0f);
            
            vertices.push_back(localizedVertex);
            indices.push_back(curIndex);
        }
    }
}
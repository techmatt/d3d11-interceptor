
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

UINT64 LocalizedObject::computeSignature(MyD3DAssets &assets, const DrawParameters &params, string &debugDesc)
{
    UINT64 sum = 0;

    const BufferCPU *VSConstants = assets.getVSConstantBuffer();
    const auto &indexBuffer = assets.getActiveIndexBuffer();
    const auto &vertexBuffer = assets.getActiveVertexBuffer();
    
    const int signatureIndexStart = 16;

    if (vertexBuffer.buffer == nullptr || indexBuffer.buffer == nullptr ||
        assets.activeVertexLayout == nullptr || assets.activeVertexLayout->tex0Offset == -1)
    {
        return 0;
    }

    if (params.BaseVertexLocation == -1)
    {
        return 0;
    }
    
    const WORD *indexDataStart = (WORD *)indexBuffer.buffer->data.data() + params.StartIndexLocation;
    const BYTE *vertexData = vertexBuffer.buffer->data.data();

    int prevIndex = -999;
    for (int indexIndex = signatureIndexStart; indexIndex < min(signatureIndexStart + 16, (int)params.IndexCount); indexIndex++)
    {
        const int curIndex = indexDataStart[indexIndex] + params.BaseVertexLocation;
        const BYTE *curVertex = (vertexData + (vertexBuffer.stride * curIndex));

        bool valid = (curIndex == prevIndex + 1);
        if (valid)
        {
            if (vertexBuffer.buffer->data.size() < vertexBuffer.stride * (curIndex + 1))
            {
                debugDesc += "overflow: vBuffer size=" + to_string(vertexBuffer.buffer->data.size()) + ", stride=" + to_string(vertexBuffer.stride) + ", curIndex+1=" + to_string(curIndex + 1);
                return 1;
            }

            const int tOffset = assets.activeVertexLayout->tex0Offset;
            const UINT32 *tStartI = (const UINT32 *)(curVertex + tOffset);

            sum += UINT64(tStartI[0]);
            sum += UINT64(tStartI[1]);

            const float *tStartF = (const float *)(curVertex + tOffset);
            debugDesc += to_string(tStartF[0]) + ", " + to_string(tStartF[1]) + "; ";
        }

        prevIndex = curIndex;
    }

    return sum;
}

void LocalizedObject::computeBoundingInfo(MyD3DAssets &assets, const DrawParameters &params, LocalizedObjectData &result)
{
    vec3f centroid = vec3f::origin;
    int centroidCount = 0;

    const float nan = numeric_limits<float>::quiet_NaN();
    for (int i = 0; i < LocalizedObjectData::vertexStoreCount; i++)
        result.vertices[i] = vec3f(nan, nan, nan);

    int vertexStoreIndex = 0;
    const int vertexStoreStride = max(1, (int)params.IndexCount / LocalizedObjectData::vertexStoreCount);
    int vertexStoreRemaining = 10;

    const auto &indexBuffer = assets.getActiveIndexBuffer();
    const auto &vertexBuffer = assets.getActiveVertexBuffer();
    const BufferCPU *VSConstants = assets.getVSConstantBuffer();

    const int bboxIndexStart = 8;

    if (vertexBuffer.buffer == nullptr || indexBuffer.buffer == nullptr ||
        assets.activeVertexLayout == nullptr || assets.activeVertexLayout->positionOffset == -1 ||
        params.BaseVertexLocation == -1)
    {
        return;
    }

    const WORD *indexDataStart = (WORD *)indexBuffer.buffer->data.data() + params.StartIndexLocation;
    const BYTE *vertexData = vertexBuffer.buffer->data.data();

    int prevIndex = -999;
    for (int indexIndex = bboxIndexStart; indexIndex < (int)params.IndexCount; indexIndex++)
    {
        if (vertexStoreRemaining > 0) vertexStoreRemaining--;
        const int curIndex = indexDataStart[indexIndex] + params.BaseVertexLocation;
        const BYTE *curVertex = (vertexData + (vertexBuffer.stride * curIndex));

        if (vertexBuffer.buffer->data.size() < vertexBuffer.stride * (curIndex + 1))
        {
            continue;
        }

        bool valid = (curIndex == prevIndex + 1);
        if (valid)
        {
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
            const vec3f worldPos = assets.transformObjectToWorldGamecube(VSConstants, basePos, blendMatrixIndex);

            if (worldPos.x == worldPos.x)
            {
                centroidCount++;
                centroid += worldPos;

                if (vertexStoreRemaining == 0 && vertexStoreIndex != LocalizedObjectData::vertexStoreCount)
                {
                    result.vertices[vertexStoreIndex++] = worldPos;
                    vertexStoreRemaining = vertexStoreStride;
                }
            }
        }

        prevIndex = curIndex;
    }

    if (centroidCount == 0) centroidCount = 1;

    result.centroid = centroid / (float)centroidCount;
}

void LocalizedObject::load(MyD3DAssets &assets, const DrawParameters &params, bool loadVertexData)
{
    signatureDebug = "init";

    data.drawIndex = g_logger->frameRenderIndex;
    data.signature = computeSignature(assets, params, signatureDebug);
    computeBoundingInfo(assets, params, data);

    if (loadVertexData)
    {
        vertices.clear();
        indices.clear();

        if (params.BaseVertexLocation == -1)
            loadFromDraw(assets, params.IndexCount, params.StartIndexLocation);
        else
            loadFromDrawIndexed(assets, params.IndexCount, params.StartIndexLocation, params.BaseVertexLocation);
    }
}

void LocalizedObject::loadFromDraw(MyD3DAssets &assets, UINT  VertexCount, UINT  StartVertexLocation)
{
    const BufferCPU *VSConstants = assets.getVSConstantBuffer();
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
            localizedVertex.tex0 = vec2f(0.0f, 0.0f);

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
            localizedVertex.worldPos = assets.transformObjectToWorldGamecube(VSConstants, basePos, blendMatrixIndex);

            if (basePos.x == 0.0f || localizedVertex.worldPos.x != localizedVertex.worldPos.x)
                localizedVertex.worldPos = vec3f(0.0f, 0.0f, 0.0f);

            vertices.push_back(localizedVertex);
            indices.push_back(curIndex);
        }
    }
}

void LocalizedObject::loadFromDrawIndexed(MyD3DAssets &assets, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
    const BufferCPU *VSConstants = assets.getVSConstantBuffer();
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
            localizedVertex.tex0 = vec2f::origin;

            if (vertexBuffer.buffer->data.size() < vertexBuffer.stride * (curIndex + 1))
            {
                vertices.push_back(localizedVertex);
                indices.push_back(curIndex);
                continue;
            }

            const int pOffset = assets.activeVertexLayout->positionOffset;
            const int bOffset = assets.activeVertexLayout->blendOffset;
            const int tOffset = assets.activeVertexLayout->tex0Offset;
            
            int blendMatrixIndex = -1;
            if (bOffset != -1)
            {
                vec4uc blendIndices = *((const vec4uc *)(curVertex + bOffset));
                blendMatrixIndex = blendIndices.x;
            }

            const float *pStart = (const float *)(curVertex + pOffset);
            const vec3f basePos(pStart[0], pStart[1], pStart[2]);
            localizedVertex.worldPos = assets.transformObjectToWorldGamecube(VSConstants, basePos, blendMatrixIndex);

            if (tOffset != -1)
            {
                const float *tStart = (const float *)(curVertex + tOffset);
                localizedVertex.tex0 = vec2f(tStart[0], tStart[1]);
            }

            vertices.push_back(localizedVertex);
            indices.push_back(curIndex);
        }
    }
}
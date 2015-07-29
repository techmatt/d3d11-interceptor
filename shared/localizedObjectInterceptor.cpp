
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

int LocalizedObject::computeIndexSkip(MyD3DAssets &assets, const DrawParameters &params, const GPUDrawBuffers &buffers)
{
    if (buffers.vertex.buffer == nullptr || buffers.vertex.buffer == nullptr ||
        assets.activeVertexLayout == nullptr || assets.activeVertexLayout->positionOffset == -1 ||
        params.BaseVertexLocation == -1)
    {
        return 0;
    }

    const int maxIndexSkip = 16;
    const WORD *indexDataStart = (WORD *)buffers.index.buffer->data.data() + params.StartIndexLocation;
    const BYTE *vertexData = buffers.vertex.buffer->data.data();

    int prevIndex = -999;
    vec3f startPos;
    for (int indexIndex = 0; indexIndex < min(maxIndexSkip, (int)params.IndexCount); indexIndex++)
    {
        const int curIndex = indexDataStart[indexIndex] + params.BaseVertexLocation;
        const BYTE *curVertex = (vertexData + (buffers.vertex.stride * curIndex));

        bool valid = (prevIndex == -999 || curIndex == prevIndex + 1);
        if (!valid)
        {
            return indexIndex + 1;
        }

        if (buffers.vertex.buffer->data.size() < buffers.vertex.stride * (curIndex + 1))
        {
            return 0;
        }

        const int pOffset = assets.activeVertexLayout->positionOffset;
        const float *pStart = (const float *)(curVertex + pOffset);

        const vec3f pos(pStart[0], pStart[1], pStart[2]);
        if (indexIndex == 0)
        {
            startPos = pos;
        }
        else if (indexIndex == 1)
        {
            if (pos != startPos) return 0;
        }
        else
        {
            if (pos != startPos) return indexIndex + 1;
        }

        prevIndex = curIndex;
    }

    return 0;
}

UINT64 LocalizedObject::computeSignature(MyD3DAssets &assets, const DrawParameters &params, const GPUDrawBuffers &buffers)
{
    UINT64 sum = 0;

    const int signatureIndexCount = 16;

    if (buffers.vertex.buffer == nullptr || buffers.index.buffer == nullptr ||
        assets.activeVertexLayout == nullptr || assets.activeVertexLayout->tex0Offset == -1)
    {
        return 0;
    }

    if (params.BaseVertexLocation == -1)
    {
        return 0;
    }
    
    const int indexSkip = computeIndexSkip(assets, params, buffers);

    const WORD *indexDataStart = (WORD *)buffers.index.buffer->data.data() + params.StartIndexLocation;
    const BYTE *vertexData = buffers.vertex.buffer->data.data();

    int prevIndex = -999;
    for (int indexIndex = indexSkip; indexIndex < min(indexSkip + signatureIndexCount, (int)params.IndexCount); indexIndex++)
    {
        const int curIndex = indexDataStart[indexIndex] + params.BaseVertexLocation;
        const BYTE *curVertex = (vertexData + (buffers.vertex.stride * curIndex));

        bool valid = (prevIndex == -999 || curIndex == prevIndex + 1);
        if (valid)
        {
            if (buffers.vertex.buffer->data.size() < buffers.vertex.stride * (curIndex + 1))
            {
                //debugDesc += "overflow: vBuffer size=" + to_string(vertexBuffer.buffer->data.size()) + ", stride=" + to_string(vertexBuffer.stride) + ", curIndex+1=" + to_string(curIndex + 1);
                return 1;
            }

            const int tOffset = assets.activeVertexLayout->tex0Offset;
            const UINT32 *tStartI = (const UINT32 *)(curVertex + tOffset);

            sum += UINT64(tStartI[0]);
            sum += UINT64(tStartI[1]);

            const float *tStartF = (const float *)(curVertex + tOffset);
            //debugDesc += to_string(tStartF[0]) + ", " + to_string(tStartF[1]) + "; ";
        }

        prevIndex = curIndex;
    }

    return sum;
}

void LocalizedObject::computeBoundingInfo(MyD3DAssets &assets, const DrawParameters &params, const GPUDrawBuffers &buffers, LocalizedObjectData &result)
{
    bbox3f bbox;
    vec3f centroid = vec3f::origin;
    int centroidCount = 0;

    const float nan = numeric_limits<float>::quiet_NaN();
    for (int i = 0; i < LocalizedObjectData::vertexStoreCount; i++)
        result.vertices[i] = vec3f(nan, nan, nan);

    const int indexSkip = computeIndexSkip(assets, params, buffers);

    int vertexStoreIndex = 0;
    const int vertexStoreStride = max(1, ((int)params.IndexCount - indexSkip) / LocalizedObjectData::vertexStoreCount);
    int vertexStoreRemaining = 0;

    if (buffers.vertex.buffer == nullptr || buffers.index.buffer == nullptr ||
        assets.activeVertexLayout == nullptr || assets.activeVertexLayout->positionOffset == -1 ||
        params.BaseVertexLocation == -1)
    {
        return;
    }

    const WORD *indexDataStart = (WORD *)buffers.index.buffer->data.data() + params.StartIndexLocation;
    const BYTE *vertexData = buffers.vertex.buffer->data.data();

    int prevIndex = -999;
    for (int indexIndex = indexSkip; indexIndex < (int)params.IndexCount; indexIndex++)
    {
        if (vertexStoreRemaining > 0) vertexStoreRemaining--;
        const int curIndex = indexDataStart[indexIndex] + params.BaseVertexLocation;
        const BYTE *curVertex = (vertexData + (buffers.vertex.stride * curIndex));

        if (buffers.vertex.buffer->data.size() < buffers.vertex.stride * (curIndex + 1))
        {
            continue;
        }

        bool valid = (prevIndex == -999 || curIndex == prevIndex + 1);
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
            const vec3f worldPos = assets.transformObjectToWorldGamecube(buffers.VSConstants, basePos, blendMatrixIndex);

            if (worldPos.x == worldPos.x)
            {
                bbox.include(worldPos);
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
    result.bbox = bbox;
}

void LocalizedObject::load(MyD3DAssets &assets, const DrawParameters &params, const GPUDrawBuffers &buffers, bool loadVertexData)
{
    //signatureDebug = "init";

    data.drawIndex = g_logger->frameRenderIndex;
    data.signature = computeSignature(assets, params, buffers);
    computeBoundingInfo(assets, params, buffers, data);

    if (loadVertexData)
    {
        vertices.clear();
        indices.clear();

        if (params.BaseVertexLocation == -1)
            loadFromDraw(assets, buffers, params.IndexCount, params.StartIndexLocation);
        else
            loadFromDrawIndexed(assets, buffers, params.IndexCount, params.StartIndexLocation, params.BaseVertexLocation);
    }
}

void LocalizedObject::loadFromDraw(MyD3DAssets &assets, const GPUDrawBuffers &buffers, UINT  VertexCount, UINT  StartVertexLocation)
{
    if (buffers.vertex.buffer != nullptr && assets.activeVertexLayout != nullptr && assets.activeVertexLayout->positionOffset != -1)
    {
        const BYTE *vertexData = buffers.vertex.buffer->data.data();

        for (int indexIndex = 0; indexIndex < (int)VertexCount; indexIndex++)
        {
            const int curIndex = StartVertexLocation + indexIndex;
            const BYTE *curVertex = (vertexData + (buffers.vertex.stride * curIndex));

            LocalizedObjectVertex localizedVertex;
            localizedVertex.worldPos = vec3f::origin;
            localizedVertex.tex0 = vec2f(0.0f, 0.0f);

            if (buffers.vertex.buffer->data.size() < buffers.vertex.stride * (curIndex + 1))
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
            localizedVertex.worldPos = assets.transformObjectToWorldGamecube(buffers.VSConstants, basePos, blendMatrixIndex);

            if (basePos.x == 0.0f || localizedVertex.worldPos.x != localizedVertex.worldPos.x)
                localizedVertex.worldPos = vec3f(0.0f, 0.0f, 0.0f);

            vertices.push_back(localizedVertex);
            indices.push_back(curIndex);
        }
    }
}

void LocalizedObject::loadFromDrawIndexed(MyD3DAssets &assets, const GPUDrawBuffers &buffers, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
    if (buffers.vertex.buffer != nullptr && buffers.index.buffer != nullptr && assets.activeVertexLayout != nullptr && assets.activeVertexLayout->positionOffset != -1)
    {
        const WORD *indexDataStart = (WORD *)buffers.index.buffer->data.data() + StartIndexLocation;
        const BYTE *vertexData = buffers.vertex.buffer->data.data();

        for (int indexIndex = 0; indexIndex < (int)IndexCount; indexIndex++)
        {
            const int curIndex = indexDataStart[indexIndex] + BaseVertexLocation;
            const BYTE *curVertex = (vertexData + (buffers.vertex.stride * curIndex));
            
            LocalizedObjectVertex localizedVertex;
            localizedVertex.worldPos = vec3f::origin;
            localizedVertex.tex0 = vec2f::origin;

            if (buffers.vertex.buffer->data.size() < buffers.vertex.stride * (curIndex + 1))
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
            localizedVertex.worldPos = assets.transformObjectToWorldGamecube(buffers.VSConstants, basePos, blendMatrixIndex);

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
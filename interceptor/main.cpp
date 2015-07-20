
#include "main.h"

#include "mLibCore.cpp"
#include "mLibLodePNG.cpp"

Logger *g_logger = nullptr;
GlobalState *g_state = nullptr;

ID3D11Buffer* MyD3DAssets::getStagingBuffer(ID3D11Buffer *baseBuffer)
{
    D3D11_BUFFER_DESC desc;
    baseBuffer->GetDesc(&desc);

    if (stagingBuffersBySize.count(desc.ByteWidth) == 0)
    {
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;

        ID3D11Buffer *stagingBuffer = nullptr;
        device->base->CreateBuffer(&desc, nullptr, &stagingBuffer);
        stagingBuffersBySize[desc.ByteWidth] = stagingBuffer;
    }

    return stagingBuffersBySize[desc.ByteWidth];
}

void MyD3DAssets::loadVSConstantBuffer()
{
    ID3D11Buffer *constantBuffer = nullptr;
    context->base->VSGetConstantBuffers(0, 1, &constantBuffer);

    if (constantBuffer != nullptr)
    {
        ID3D11Buffer *stagingBuffer = getStagingBuffer(constantBuffer);

        context->base->CopyResource(stagingBuffer, constantBuffer);

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        context->base->Map(stagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

        const UINT requiredFloatCount = mappedResource.RowPitch / 4;
        if (VSBufferStorage.size() < requiredFloatCount + 1)
            VSBufferStorage.resize(requiredFloatCount + 1);

        VSBufferSize = requiredFloatCount;

        memcpy(VSBufferStorage.data(), mappedResource.pData, mappedResource.RowPitch);

        context->base->Unmap(stagingBuffer, 0);
        
        constantBuffer->Release();
    }
}
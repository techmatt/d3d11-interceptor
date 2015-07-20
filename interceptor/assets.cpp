
#include "main.h"

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

ID3D11Texture2D* MyD3DAssets::getStagingTexture(ID3D11Texture2D *baseTexture)
{
    D3D11_TEXTURE2D_DESC desc;
    baseTexture->GetDesc(&desc);

    const UINT64 combinedSize = desc.Width * (long)0xFFFFFFFF + desc.Height;

    if (stagingTexturesBySize.count(combinedSize) == 0)
    {
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;

        ID3D11Texture2D *stagingTexture = nullptr;
        device->base->CreateTexture2D(&desc, nullptr, &stagingTexture);
        stagingTexturesBySize[combinedSize] = stagingTexture;
    }

    return stagingTexturesBySize[combinedSize];
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

void MyD3DAssets::loadPSTexture(int textureIndex)
{
    ID3D11ShaderResourceView *view = nullptr;
    context->base->PSGetShaderResources(textureIndex, 1, &view);

    if (view == nullptr)
    {
        PSTexture.free();
    }
    else
    {
        ID3D11Resource *texture0Resource = nullptr;
        view->GetResource(&texture0Resource);

        ID3D11Texture2D* texture0 = nullptr;
        HRESULT hr = texture0Resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture0));
        if (texture0 == nullptr || FAILED(hr))
        {
            PSTexture.free();
            g_logger->logErrorFile << "texture0Resource->QueryInterface failed" << endl;
        }
        else
        {
            readTexture(texture0, PSTexture);
            texture0->Release();
        }

        
        texture0Resource->Release();
        view->Release();
    }
}

void MyD3DAssets::readTexture(ID3D11Texture2D *inputTexture, Bitmap &result)
{
    ID3D11Texture2D *stagingTexture = getStagingTexture(inputTexture);

    D3D11_TEXTURE2D_DESC desc;
    stagingTexture->GetDesc(&desc);
    if (result.getWidth() != desc.Width || result.getHeight() != desc.Height)
        result.allocate(desc.Width, desc.Height);

    context->base->CopyResource(stagingTexture, inputTexture);

    D3D11_MAPPED_SUBRESOURCE resource;
    UINT subresource = D3D11CalcSubresource(0, 0, 0);
    HRESULT hr = context->base->Map(stagingTexture, subresource, D3D11_MAP_READ, 0, &resource);
    const BYTE *data = (BYTE *)resource.pData;

    for (UINT y = 0; y < desc.Height; y++)
    {
        memcpy(&result(0U, y), data + resource.RowPitch * y, desc.Width * sizeof(ml::vec4uc));
    }

    context->base->Unmap(stagingTexture, subresource);
}

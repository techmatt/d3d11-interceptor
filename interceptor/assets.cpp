
#include "main.h"

struct VertexShaderConstantsGamecube
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

vec3f MyD3DAssets::transformObjectToWorldGamecube(const vec3f &basePos, int blendMatrixStart) const
{
    if (VSBufferSize < sizeof(VertexShaderConstantsGamecube))
    {
        return vec3f::origin;
    }

    const VertexShaderConstantsGamecube &constants = *((const VertexShaderConstantsGamecube *)VSBufferStorage.data());

    vec4f rawPos(basePos, 1.0f);

    vec4f m0 = constants.posnormalmatrix[0];
    vec4f m1 = constants.posnormalmatrix[1];
    vec4f m2 = constants.posnormalmatrix[2];
    if (blendMatrixStart != -1)
    {
        m0 = constants.transformmatrices[blendMatrixStart + 0];
        m1 = constants.transformmatrices[blendMatrixStart + 1];
        m2 = constants.transformmatrices[blendMatrixStart + 2];
    }
    
    vec3f worldPos(m0 | rawPos,
                   m1 | rawPos,
                   m2 | rawPos);

    return worldPos;
}

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

    const UINT64 combinedSize = desc.Width * (long)0xFFFFFFFF + desc.Height + ml::util::hash64(desc.Format);

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

        if (VSBufferStorage.size() < mappedResource.RowPitch)
            VSBufferStorage.resize(mappedResource.RowPitch);

        VSBufferSize = mappedResource.RowPitch;

        memcpy(VSBufferStorage.data(), mappedResource.pData, mappedResource.RowPitch);

        context->base->Unmap(stagingBuffer, 0);
        
        constantBuffer->Release();
    }
}

void MyD3DAssets::loadPSConstantBuffer()
{
    ID3D11Buffer *constantBuffer = nullptr;
    context->base->PSGetConstantBuffers(0, 1, &constantBuffer);

    if (constantBuffer != nullptr)
    {
        ID3D11Buffer *stagingBuffer = getStagingBuffer(constantBuffer);

        context->base->CopyResource(stagingBuffer, constantBuffer);

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        context->base->Map(stagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

        if (PSBufferStorage.size() < mappedResource.RowPitch)
            PSBufferStorage.resize(mappedResource.RowPitch);

        PSBufferSize = mappedResource.RowPitch;

        memcpy(PSBufferStorage.data(), mappedResource.pData, mappedResource.RowPitch);

        context->base->Unmap(stagingBuffer, 0);

        constantBuffer->Release();
    }
}

VertexBufferState MyD3DAssets::getActiveVertexBuffer()
{
    VertexBufferState result;
    ID3D11Buffer *buffer;
    context->base->IAGetVertexBuffers(0, 1, &buffer, &result.stride, &result.offset);
    result.buffer = getBuffer(buffer);
    buffer->Release();
    return result;
}

IndexBufferState MyD3DAssets::getActiveIndexBuffer()
{
    IndexBufferState result;
    ID3D11Buffer *buffer;
    context->base->IAGetIndexBuffer(&buffer, nullptr, &result.offset);
    result.buffer = getBuffer(buffer);
    buffer->Release();
    return result;
}

const BufferCPU* MyD3DAssets::getBuffer(ID3D11Buffer *buffer)
{
    if (buffers.count((UINT64)buffer) == 0)
    {
        g_logger->logErrorFile << "buffer not found: " << pointerToString(buffer) << endl;
        return nullptr;
    }
    return buffers.find((UINT64)buffer)->second;
}

const BufferCPU* MyD3DAssets::loadBufferFromGPU(ID3D11Buffer *buffer)
{
    if (buffer == nullptr)
        return nullptr;

    if (buffers.count((UINT64)buffer) == 0)
    {
        if (g_logger->logInterfaceCalls) g_logger->log("*** Creating vertex buffer: " + pointerToString(buffer));
        buffers[(UINT64)buffer] = new BufferCPU(0, buffer);
    }
    
    BufferCPU *cpu = buffers[(UINT64)buffer];
    readBuffer(buffer, cpu->data);
    
    return cpu;
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
        ID3D11Resource *textureResource = nullptr;
        view->GetResource(&textureResource);

        ID3D11Texture2D* texture = nullptr;
        HRESULT hr = textureResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture));
        if (texture == nullptr || FAILED(hr))
        {
            PSTexture.free();
            g_logger->logErrorFile << "textureResource->QueryInterface failed" << endl;
        }
        else
        {
            readTexture(texture, PSTexture);
            texture->Release();
        }

        textureResource->Release();
        view->Release();
    }
}

void MyD3DAssets::readBuffer(ID3D11Buffer *inputBuffer, vector<BYTE> &result)
{
    ID3D11Buffer *stagingBuffer = getStagingBuffer(inputBuffer);

    context->base->CopyResource(stagingBuffer, inputBuffer);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->base->Map(stagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

    if (result.size() != mappedResource.RowPitch)
        result.resize(mappedResource.RowPitch);

    memcpy(result.data(), mappedResource.pData, mappedResource.RowPitch);

    context->base->Unmap(stagingBuffer, 0);
}

void MyD3DAssets::readTexture(ID3D11Texture2D *inputTexture, Bitmap &result)
{
    ID3D11Texture2D *stagingTexture = getStagingTexture(inputTexture);

    D3D11_TEXTURE2D_DESC desc;
    stagingTexture->GetDesc(&desc);
    if (result.getWidth() != desc.Width || result.getHeight() != desc.Height)
        result.allocate(desc.Width, desc.Height);

    for (auto &p : result)
        p.value = ml::vec4uc(50, 100, 150, 255);

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

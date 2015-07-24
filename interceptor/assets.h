class myIUnknown
{
protected:
    IUnknown*   m_pUnk;
    ULONG       m_ulRef;

public:
    myIUnknown(IUnknown* pUnk)
    {
        m_pUnk = pUnk;
        m_ulRef = 1;
    }

    ULONG AddRef()
    {
        m_pUnk->AddRef();
        return ++m_ulRef;
    }

    ULONG Release()
    {
        m_pUnk->Release();

        ULONG ulRef = --m_ulRef;
        if (0 == ulRef)
        {
            //delete this;
            return 0;
        }
        return ulRef;
    }
};

class myDXGISwapChain;
class myD3D11Device;
class myD3D11DeviceContext;

struct BufferCPU
{
    BufferCPU(size_t size, ID3D11Buffer* _GPUHandle)
    {
        data.resize(size);
        GPUHandle = _GPUHandle;
        mappedResource.RowPitch = (UINT)size;
        mappedResource.DepthPitch = (UINT)size;
        mappedResource.pData = (void *)data.data();
        mapped = false;
    }
    vector<BYTE> data;
    ID3D11Buffer* GPUHandle;
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    bool mapped;
    D3D11_MAP mapType;
    UINT mapFlags;
};

struct VertexBufferState
{
    const BufferCPU *buffer;
    UINT stride;
    UINT offset;
};

struct IndexBufferState
{
    const BufferCPU *buffer;
    UINT offset;
};

struct VertexLayout
{
    VertexLayout() {}
    VertexLayout(const D3D11_INPUT_ELEMENT_DESC *  pInputElementDescs, UINT  NumElements)
    {
        positionOffset = -1;
        colorOffset = -1;
        tex0Offset = -1;
        blendOffset = -1;

        for (int elem = 0; elem < (int)NumElements; elem++)
        {
            const D3D11_INPUT_ELEMENT_DESC &desc = pInputElementDescs[elem];
            
            if (string(desc.SemanticName) == string("POSITION"))
                positionOffset = desc.AlignedByteOffset;

            if (string(desc.SemanticName) == string("COLOR"))
                colorOffset = desc.AlignedByteOffset;

            if (string(desc.SemanticName) == string("TEXCOORD"))
                tex0Offset = desc.AlignedByteOffset;

            if (string(desc.SemanticName) == string("BLENDINDICES"))
                blendOffset = desc.AlignedByteOffset;
            
            htmlDescription += string(desc.SemanticName) + "-" + to_string(desc.SemanticIndex);
            htmlDescription += ", offset=" + to_string(desc.AlignedByteOffset);
            htmlDescription += ", format=" + to_string(desc.Format);
            htmlDescription += "<br />";
        }
    }

    int positionOffset;
    int colorOffset;
    int tex0Offset;
    int blendOffset;
    string htmlDescription;
};

struct MyD3DAssets
{
    ID3D11Buffer* getStagingBuffer(ID3D11Buffer *baseBuffer);
    ID3D11Texture2D* getStagingTexture(ID3D11Texture2D *baseTexture);

    void readTexture(ID3D11Texture2D *inputTexture, Bitmap &result);
    void readBuffer(ID3D11Buffer *inputBuffer, vector<BYTE> &result);

    vec3f transformObjectToWorldGamecube(const vec3f &basePos, int blendMatrixStart) const;

    void loadVSConstantBuffer();
    void loadPSConstantBuffer();
    void loadPSTexture(int textureIndex);

    VertexBufferState getActiveVertexBuffer();
    IndexBufferState getActiveIndexBuffer();
    const BufferCPU* loadBufferFromGPU(ID3D11Buffer *inputBuffer);
    const BufferCPU* getBuffer(ID3D11Buffer *inputBuffer);

    myDXGISwapChain *swapChain;
    myD3D11Device *device;
    myD3D11DeviceContext *context;

    vector<BYTE> VSBufferStorage;
    UINT VSBufferSize;

    vector<BYTE> PSBufferStorage;
    UINT PSBufferSize;

    Bitmap PSTexture;

    VertexLayout *activeVertexLayout;
    map<UINT64, VertexLayout*> vertexLayouts;
    map<UINT64, BufferCPU*> buffers;

    map<UINT, ID3D11Buffer*> stagingBuffersBySize;
    map<UINT64, ID3D11Texture2D*> stagingTexturesBySize;
};

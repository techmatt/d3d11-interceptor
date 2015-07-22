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
    BufferCPU()
    {
        dirty = false;
    }
    bool dirty;
    vector<BYTE> data;
    const void* handle;
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

        for (int elem = 0; elem < (int)NumElements; elem++)
        {
            const D3D11_INPUT_ELEMENT_DESC &desc = pInputElementDescs[elem];
            
            if (string(desc.SemanticName) == string("POSITION"))
                positionOffset = desc.AlignedByteOffset;

            if (string(desc.SemanticName) == string("COLOR"))
                colorOffset = desc.AlignedByteOffset;
        }
    }

    int positionOffset;
    int colorOffset;
};

struct MyD3DAssets
{
    ID3D11Buffer* getStagingBuffer(ID3D11Buffer *baseBuffer);
    ID3D11Texture2D* getStagingTexture(ID3D11Texture2D *baseTexture);

    void readTexture(ID3D11Texture2D *inputTexture, Bitmap &result);
    void readBuffer(ID3D11Buffer *inputBuffer, vector<BYTE> &result);

    vec3f transformObjectToWorldGamecube(const vec3f &basePos) const;

    void loadVSConstantBuffer();
    void loadPSTexture(int textureIndex);

    VertexBufferState getActiveVertexBuffer();
    IndexBufferState getActiveIndexBuffer();
    const BufferCPU* loadAndCacheBuffer(ID3D11Buffer *inputBuffer);

    myDXGISwapChain *swapChain;
    myD3D11Device *device;
    myD3D11DeviceContext *context;

    vector<float> VSBufferStorage;
    UINT VSBufferSize;

    Bitmap PSTexture;

    VertexLayout *activeVertexLayout;
    map<UINT64, VertexLayout*> vertexLayouts;

    map<UINT, ID3D11Buffer*> stagingBuffersBySize;
    map<UINT64, ID3D11Texture2D*> stagingTexturesBySize;
    map<UINT64, BufferCPU*> cachedBuffers;
};

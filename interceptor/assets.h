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

struct MyD3DAssets
{
    ID3D11Buffer* getStagingBuffer(ID3D11Buffer *baseBuffer);
    ID3D11Texture2D* getStagingTexture(ID3D11Texture2D *baseTexture);

    void readTexture(ID3D11Texture2D *inputTexture, Bitmap &result);

    void loadVSConstantBuffer();
    void loadPSTexture(int textureIndex);

    myDXGISwapChain *swapChain;
    myD3D11Device *device;
    myD3D11DeviceContext *context;

    vector<float> VSBufferStorage;
    UINT VSBufferSize;

    Bitmap PSTexture;

    map<UINT, ID3D11Buffer*> stagingBuffersBySize;
    map<UINT64, ID3D11Texture2D*> stagingTexturesBySize;
};

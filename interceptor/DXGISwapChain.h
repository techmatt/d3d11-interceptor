
class myDXGISwapChain : public IDXGISwapChain
{
public:
    myDXGISwapChain(IDXGISwapChain *baseObject)
        : unknown((IUnknown*)baseObject)
    {
        base = baseObject;
    }

    IDXGISwapChain *base;
    myIUnknown unknown;

    MyD3DAssets assets;

    HRESULT QueryInterface(const IID &, void **)
    {
        return E_FAIL;
    }
    ULONG Release()
    {
        return unknown.Release();
    }
    ULONG AddRef()
    {
        return unknown.AddRef();
    }
    
    HRESULT SetPrivateData(REFGUID  Name, UINT  DataSize, const void *  pData);
    HRESULT SetPrivateDataInterface(REFGUID  Name, const IUnknown *  pUnknown);
    HRESULT GetPrivateData(REFGUID  Name, _Inout_ UINT *  pDataSize, void *  pData);
    HRESULT GetParent(REFIID  riid, void * *  ppParent);
    HRESULT GetDevice(REFIID  riid, void * *  ppDevice);
    HRESULT Present(UINT  SyncInterval, UINT  Flags);
    HRESULT GetBuffer(UINT  Buffer, REFIID  riid, void * *  ppSurface);
    HRESULT SetFullscreenState(BOOL  Fullscreen, IDXGIOutput *  pTarget);
    HRESULT GetFullscreenState(BOOL *  pFullscreen, IDXGIOutput * *  ppTarget);
    HRESULT GetDesc(DXGI_SWAP_CHAIN_DESC *  pDesc);
    HRESULT ResizeBuffers(UINT  BufferCount, UINT  Width, UINT  Height, DXGI_FORMAT  NewFormat, UINT  SwapChainFlags);
    HRESULT ResizeTarget(const DXGI_MODE_DESC *  pNewTargetParameters);
    HRESULT GetContainingOutput(IDXGIOutput * *  ppOutput);
    HRESULT GetFrameStatistics(DXGI_FRAME_STATISTICS *  pStats);
    HRESULT GetLastPresentCount(UINT *  pLastPresentCount);
};

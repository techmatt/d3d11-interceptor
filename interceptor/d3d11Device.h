
class myD3D11Device : public ID3D11Device
{
public:
    myD3D11Device(ID3D11Device *baseObject)
        : unknown((IUnknown*)baseObject)
    {
        base = baseObject;
    }

    ID3D11Device *base;
    myIUnknown unknown;

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

    HRESULT CreateBuffer(const D3D11_BUFFER_DESC *  pDesc, const D3D11_SUBRESOURCE_DATA *  pInitialData, ID3D11Buffer * *  ppBuffer);
    HRESULT CreateTexture1D(const D3D11_TEXTURE1D_DESC *  pDesc, const D3D11_SUBRESOURCE_DATA *  pInitialData, ID3D11Texture1D * *  ppTexture1D);
    HRESULT CreateTexture2D(const D3D11_TEXTURE2D_DESC *  pDesc, const D3D11_SUBRESOURCE_DATA *  pInitialData, ID3D11Texture2D * *  ppTexture2D);
    HRESULT CreateTexture3D(const D3D11_TEXTURE3D_DESC *  pDesc, const D3D11_SUBRESOURCE_DATA *  pInitialData, ID3D11Texture3D * *  ppTexture3D);
    HRESULT CreateShaderResourceView(ID3D11Resource *  pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *  pDesc, ID3D11ShaderResourceView * *  ppSRView);
    HRESULT CreateUnorderedAccessView(ID3D11Resource *  pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC *  pDesc, ID3D11UnorderedAccessView * *  ppUAView);
    HRESULT CreateRenderTargetView(ID3D11Resource *  pResource, const D3D11_RENDER_TARGET_VIEW_DESC *  pDesc, ID3D11RenderTargetView * *  ppRTView);
    HRESULT CreateDepthStencilView(ID3D11Resource *  pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *  pDesc, ID3D11DepthStencilView * *  ppDepthStencilView);
    HRESULT CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *  pInputElementDescs, UINT  NumElements, const void *  pShaderBytecodeWithInputSignature, SIZE_T  BytecodeLength, ID3D11InputLayout * *  ppInputLayout);
    HRESULT CreateVertexShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11VertexShader * *  ppVertexShader);
    HRESULT CreateGeometryShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11GeometryShader * *  ppGeometryShader);
    HRESULT CreateGeometryShaderWithStreamOutput(const void *  pShaderBytecode, SIZE_T  BytecodeLength, const D3D11_SO_DECLARATION_ENTRY *  pSODeclaration, UINT  NumEntries, const UINT *  pBufferStrides, UINT  NumStrides, UINT  RasterizedStream, ID3D11ClassLinkage *  pClassLinkage, ID3D11GeometryShader * *  ppGeometryShader);
    HRESULT CreatePixelShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11PixelShader * *  ppPixelShader);
    HRESULT CreateHullShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11HullShader * *  ppHullShader);
    HRESULT CreateDomainShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11DomainShader * *  ppDomainShader);
    HRESULT CreateComputeShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11ComputeShader * *  ppComputeShader);
    HRESULT CreateClassLinkage(ID3D11ClassLinkage * *  ppLinkage);
    HRESULT CreateBlendState(const D3D11_BLEND_DESC *  pBlendStateDesc, ID3D11BlendState * *  ppBlendState);
    HRESULT CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC *  pDepthStencilDesc, ID3D11DepthStencilState * *  ppDepthStencilState);
    HRESULT CreateRasterizerState(const D3D11_RASTERIZER_DESC *  pRasterizerDesc, ID3D11RasterizerState * *  ppRasterizerState);
    HRESULT CreateSamplerState(const D3D11_SAMPLER_DESC *  pSamplerDesc, ID3D11SamplerState * *  ppSamplerState);
    HRESULT CreateQuery(const D3D11_QUERY_DESC *  pQueryDesc, ID3D11Query * *  ppQuery);
    HRESULT CreatePredicate(const D3D11_QUERY_DESC *  pPredicateDesc, ID3D11Predicate * *  ppPredicate);
    HRESULT CreateCounter(const D3D11_COUNTER_DESC *  pCounterDesc, ID3D11Counter * *  ppCounter);
    HRESULT CreateDeferredContext(UINT  ContextFlags, ID3D11DeviceContext * *  ppDeferredContext);
    HRESULT OpenSharedResource(HANDLE  hResource, REFIID  ReturnedInterface, void * *  ppResource);
    HRESULT CheckFormatSupport(DXGI_FORMAT  Format, UINT *  pFormatSupport);
    HRESULT CheckMultisampleQualityLevels(DXGI_FORMAT  Format, UINT  SampleCount, UINT *  pNumQualityLevels);
    void CheckCounterInfo(D3D11_COUNTER_INFO *  pCounterInfo);
    HRESULT CheckCounter(const D3D11_COUNTER_DESC *  pDesc, D3D11_COUNTER_TYPE *  pType, UINT *  pActiveCounters, LPSTR  szName, _Inout_opt_ UINT *  pNameLength, LPSTR  szUnits, _Inout_opt_ UINT *  pUnitsLength, LPSTR  szDescription, _Inout_opt_ UINT *  pDescriptionLength);
    HRESULT CheckFeatureSupport(D3D11_FEATURE  Feature, void *  pFeatureSupportData, UINT  FeatureSupportDataSize);
    HRESULT GetPrivateData(REFGUID  guid, _Inout_ UINT *  pDataSize, void *  pData);
    HRESULT SetPrivateData(REFGUID  guid, UINT  DataSize, const void *  pData);
    HRESULT SetPrivateDataInterface(REFGUID  guid, const IUnknown *  pData);
    D3D_FEATURE_LEVEL GetFeatureLevel(void);
    UINT GetCreationFlags(void);
    HRESULT GetDeviceRemovedReason(void);
    void GetImmediateContext(ID3D11DeviceContext * *  ppImmediateContext);
    HRESULT SetExceptionMode(UINT  RaiseFlags);
    UINT GetExceptionMode(void);
};

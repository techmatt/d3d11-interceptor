
class myD3D11DeviceContext : public ID3D11DeviceContext
{
public:
    myD3D11DeviceContext(ID3D11DeviceContext *baseObject)
        : unknown((IUnknown*)baseObject)
    {
        base = baseObject;
    }

    ID3D11DeviceContext *base;
    myIUnknown unknown;

    MyD3DAssets assets;
    
    void readRenderTarget(Bitmap &result)
    {
        ID3D11RenderTargetView *view;
        OMGetRenderTargets(1, &view, nullptr);

        ID3D11Resource *resource;
        view->GetResource(&resource);

        ID3D11Texture2D *inputTexture;

        HRESULT hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void **)&inputTexture);
        if (FAILED(hr))
        {
            g_logger->logErrorFile << "QueryInterface failed to cast to a texture" << endl;
        }
        else
        {
            readTexture(inputTexture, result);
        }

        view->Release();
    }

    void readTexture(ID3D11Texture2D *inputTexture, Bitmap &result)
    {
        ID3D11Texture2D *captureTexture;

        D3D11_TEXTURE2D_DESC renderDesc;
        inputTexture->GetDesc(&renderDesc);

        renderDesc.MipLevels = 0;
        renderDesc.ArraySize = 1;
        renderDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        renderDesc.SampleDesc.Count = 1;
        renderDesc.SampleDesc.Quality = 0;
        renderDesc.BindFlags = 0;
        renderDesc.MiscFlags = 0;
        renderDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        renderDesc.Usage = D3D11_USAGE_STAGING;
        assets.device->CreateTexture2D(&renderDesc, nullptr, &captureTexture);

        CopyResource(captureTexture, inputTexture);

        result.allocate(renderDesc.Width, renderDesc.Height);

        D3D11_MAPPED_SUBRESOURCE resource;
        UINT subresource = D3D11CalcSubresource(0, 0, 0);
        HRESULT hr = Map(captureTexture, subresource, D3D11_MAP_READ, 0, &resource);
        const BYTE *data = (BYTE *)resource.pData;

        for (UINT y = 0; y < renderDesc.Height; y++)
        {
            memcpy(&result(0U, y), data + resource.RowPitch * y, renderDesc.Width * sizeof(ml::vec4uc));
        }

        Unmap(captureTexture, subresource);

        captureTexture->Release();
    }

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

    void GetDevice(ID3D11Device * *  ppDevice);
    HRESULT GetPrivateData(REFGUID  guid, _Inout_ UINT *  pDataSize, void *  pData);
    HRESULT SetPrivateData(REFGUID  guid, UINT  DataSize, const void *  pData);
    HRESULT SetPrivateDataInterface(REFGUID  guid, const IUnknown *  pData);
    void VSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers);
    void PSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews);
    void PSSetShader(ID3D11PixelShader *  pPixelShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances);
    void PSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers);
    void VSSetShader(ID3D11VertexShader *  pVertexShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances);
    void DrawIndexed(UINT  IndexCount, UINT  StartIndexLocation, INT  BaseVertexLocation);
    void Draw(UINT  VertexCount, UINT  StartVertexLocation);
    HRESULT Map(ID3D11Resource *  pResource, UINT  Subresource, D3D11_MAP  MapType, UINT  MapFlags, D3D11_MAPPED_SUBRESOURCE *  pMappedResource);
    void Unmap(ID3D11Resource *  pResource, UINT  Subresource);
    void PSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers);
    void IASetInputLayout(ID3D11InputLayout *  pInputLayout);
    void IASetVertexBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppVertexBuffers, const UINT *  pStrides, const UINT *  pOffsets);
    void IASetIndexBuffer(ID3D11Buffer *  pIndexBuffer, DXGI_FORMAT  Format, UINT  Offset);
    void DrawIndexedInstanced(UINT  IndexCountPerInstance, UINT  InstanceCount, UINT  StartIndexLocation, INT  BaseVertexLocation, UINT  StartInstanceLocation);
    void DrawInstanced(UINT  VertexCountPerInstance, UINT  InstanceCount, UINT  StartVertexLocation, UINT  StartInstanceLocation);
    void GSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers);
    void GSSetShader(ID3D11GeometryShader *  pShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances);
    void IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY  Topology);
    void VSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews);
    void VSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers);
    void Begin(ID3D11Asynchronous *  pAsync);
    void End(ID3D11Asynchronous *  pAsync);
    HRESULT GetData(ID3D11Asynchronous *  pAsync, void *  pData, UINT  DataSize, UINT  GetDataFlags);
    void SetPredication(ID3D11Predicate *  pPredicate, BOOL  PredicateValue);
    void GSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews);
    void GSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers);
    void OMSetRenderTargets(UINT  NumViews, ID3D11RenderTargetView * const *  ppRenderTargetViews, ID3D11DepthStencilView *  pDepthStencilView);
    void OMSetRenderTargetsAndUnorderedAccessViews(UINT  NumRTVs, ID3D11RenderTargetView * const *  ppRenderTargetViews, ID3D11DepthStencilView *  pDepthStencilView, UINT  UAVStartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * const *  ppUnorderedAccessViews, const UINT *  pUAVInitialCounts);
    void OMSetBlendState(ID3D11BlendState *  pBlendState, const FLOAT BlendFactor[4], UINT  SampleMask);
    void OMSetDepthStencilState(ID3D11DepthStencilState *  pDepthStencilState, UINT  StencilRef);
    void SOSetTargets(UINT  NumBuffers, ID3D11Buffer * const *  ppSOTargets, const UINT *  pOffsets);
    void DrawAuto(void);
    void DrawIndexedInstancedIndirect(ID3D11Buffer *  pBufferForArgs, UINT  AlignedByteOffsetForArgs);
    void DrawInstancedIndirect(ID3D11Buffer *  pBufferForArgs, UINT  AlignedByteOffsetForArgs);
    void Dispatch(UINT  ThreadGroupCountX, UINT  ThreadGroupCountY, UINT  ThreadGroupCountZ);
    void DispatchIndirect(ID3D11Buffer *  pBufferForArgs, UINT  AlignedByteOffsetForArgs);
    void RSSetState(ID3D11RasterizerState *  pRasterizerState);
    void RSSetViewports(UINT  NumViewports, const D3D11_VIEWPORT *  pViewports);
    void RSSetScissorRects(UINT  NumRects, const D3D11_RECT *  pRects);
    void CopySubresourceRegion(ID3D11Resource *  pDstResource, UINT  DstSubresource, UINT  DstX, UINT  DstY, UINT  DstZ, ID3D11Resource *  pSrcResource, UINT  SrcSubresource, const D3D11_BOX *  pSrcBox);
    void CopyResource(ID3D11Resource *  pDstResource, ID3D11Resource *  pSrcResource);
    void UpdateSubresource(ID3D11Resource *  pDstResource, UINT  DstSubresource, const D3D11_BOX *  pDstBox, const void *  pSrcData, UINT  SrcRowPitch, UINT  SrcDepthPitch);
    void CopyStructureCount(ID3D11Buffer *  pDstBuffer, UINT  DstAlignedByteOffset, ID3D11UnorderedAccessView *  pSrcView);
    void ClearRenderTargetView(ID3D11RenderTargetView *  pRenderTargetView, const FLOAT ColorRGBA[4]);
    void ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView *  pUnorderedAccessView, const UINT Values[4]);
    void ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView *  pUnorderedAccessView, const FLOAT Values[4]);
    void ClearDepthStencilView(ID3D11DepthStencilView *  pDepthStencilView, UINT  ClearFlags, FLOAT  Depth, UINT8  Stencil);
    void GenerateMips(ID3D11ShaderResourceView *  pShaderResourceView);
    void SetResourceMinLOD(ID3D11Resource *  pResource, FLOAT  MinLOD);
    FLOAT GetResourceMinLOD(ID3D11Resource *  pResource);
    void ResolveSubresource(ID3D11Resource *  pDstResource, UINT  DstSubresource, ID3D11Resource *  pSrcResource, UINT  SrcSubresource, DXGI_FORMAT  Format);
    void ExecuteCommandList(ID3D11CommandList *  pCommandList, BOOL  RestoreContextState);
    void HSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews);
    void HSSetShader(ID3D11HullShader *  pHullShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances);
    void HSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers);
    void HSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers);
    void DSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews);
    void DSSetShader(ID3D11DomainShader *  pDomainShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances);
    void DSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers);
    void DSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers);
    void CSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews);
    void CSSetUnorderedAccessViews(UINT  StartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * const *  ppUnorderedAccessViews, const UINT *  pUAVInitialCounts);
    void CSSetShader(ID3D11ComputeShader *  pComputeShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances);
    void CSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers);
    void CSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers);
    void VSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers);
    void PSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews);
    void PSGetShader(ID3D11PixelShader * *  ppPixelShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances);
    void PSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers);
    void VSGetShader(ID3D11VertexShader * *  ppVertexShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances);
    void PSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers);
    void IAGetInputLayout(ID3D11InputLayout * *  ppInputLayout);
    void IAGetVertexBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppVertexBuffers, UINT *  pStrides, UINT *  pOffsets);
    void IAGetIndexBuffer(ID3D11Buffer * *  pIndexBuffer, DXGI_FORMAT *  Format, UINT *  Offset);
    void GSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers);
    void GSGetShader(ID3D11GeometryShader * *  ppGeometryShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances);
    void IAGetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY *  pTopology);
    void VSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews);
    void VSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers);
    void GetPredication(ID3D11Predicate * *  ppPredicate, BOOL *  pPredicateValue);
    void GSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews);
    void GSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers);
    void OMGetRenderTargets(UINT  NumViews, ID3D11RenderTargetView * *  ppRenderTargetViews, ID3D11DepthStencilView * *  ppDepthStencilView);
    void OMGetRenderTargetsAndUnorderedAccessViews(UINT  NumRTVs, ID3D11RenderTargetView * *  ppRenderTargetViews, ID3D11DepthStencilView * *  ppDepthStencilView, UINT  UAVStartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * *  ppUnorderedAccessViews);
    void OMGetBlendState(ID3D11BlendState * *  ppBlendState, FLOAT BlendFactor[4], UINT *  pSampleMask);
    void OMGetDepthStencilState(ID3D11DepthStencilState * *  ppDepthStencilState, UINT *  pStencilRef);
    void SOGetTargets(UINT  NumBuffers, ID3D11Buffer * *  ppSOTargets);
    void RSGetState(ID3D11RasterizerState * *  ppRasterizerState);
    void RSGetViewports(UINT *  pNumViewports, D3D11_VIEWPORT *  pViewports);
    void RSGetScissorRects(UINT *  pNumRects, D3D11_RECT *  pRects);
    void HSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews);
    void HSGetShader(ID3D11HullShader * *  ppHullShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances);
    void HSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers);
    void HSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers);
    void DSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews);
    void DSGetShader(ID3D11DomainShader * *  ppDomainShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances);
    void DSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers);
    void DSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers);
    void CSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews);
    void CSGetUnorderedAccessViews(UINT  StartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * *  ppUnorderedAccessViews);
    void CSGetShader(ID3D11ComputeShader * *  ppComputeShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances);
    void CSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers);
    void CSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers);
    void ClearState(void);
    void Flush(void);
    D3D11_DEVICE_CONTEXT_TYPE GetType(void);
    UINT GetContextFlags(void);
    HRESULT FinishCommandList(BOOL  RestoreDeferredContextState, ID3D11CommandList * *  ppCommandList);
};


#include "main.h"


HRESULT myD3D11Device::CreateBuffer(const D3D11_BUFFER_DESC *  pDesc, const D3D11_SUBRESOURCE_DATA *  pInitialData, ID3D11Buffer * *  ppBuffer)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateBuffer size=" + to_string(pDesc->ByteWidth));

    HRESULT result = base->CreateBuffer(pDesc, pInitialData, ppBuffer);

    if (*ppBuffer != nullptr && SUCCEEDED(result))
    {
        assets->buffers[(UINT64)*ppBuffer] = new BufferCPU(pDesc->ByteWidth, *ppBuffer);
    }

    return result;
}


HRESULT myD3D11Device::CreateTexture1D(const D3D11_TEXTURE1D_DESC *  pDesc, const D3D11_SUBRESOURCE_DATA *  pInitialData, ID3D11Texture1D * *  ppTexture1D)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateTexture1D");

    HRESULT result = base->CreateTexture1D(pDesc, pInitialData, ppTexture1D);

    return result;
}


HRESULT myD3D11Device::CreateTexture2D(const D3D11_TEXTURE2D_DESC *  pDesc, const D3D11_SUBRESOURCE_DATA *  pInitialData, ID3D11Texture2D * *  ppTexture2D)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateTexture2D");

    HRESULT result = base->CreateTexture2D(pDesc, pInitialData, ppTexture2D);

    return result;
}


HRESULT myD3D11Device::CreateTexture3D(const D3D11_TEXTURE3D_DESC *  pDesc, const D3D11_SUBRESOURCE_DATA *  pInitialData, ID3D11Texture3D * *  ppTexture3D)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateTexture3D");

    HRESULT result = base->CreateTexture3D(pDesc, pInitialData, ppTexture3D);

    return result;
}


HRESULT myD3D11Device::CreateShaderResourceView(ID3D11Resource *  pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *  pDesc, ID3D11ShaderResourceView * *  ppSRView)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateShaderResourceView");

    HRESULT result = base->CreateShaderResourceView(pResource, pDesc, ppSRView);

    return result;
}


HRESULT myD3D11Device::CreateUnorderedAccessView(ID3D11Resource *  pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC *  pDesc, ID3D11UnorderedAccessView * *  ppUAView)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateUnorderedAccessView");

    HRESULT result = base->CreateUnorderedAccessView(pResource, pDesc, ppUAView);

    return result;
}


HRESULT myD3D11Device::CreateRenderTargetView(ID3D11Resource *  pResource, const D3D11_RENDER_TARGET_VIEW_DESC *  pDesc, ID3D11RenderTargetView * *  ppRTView)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateRenderTargetView");

    HRESULT result = base->CreateRenderTargetView(pResource, pDesc, ppRTView);

    return result;
}


HRESULT myD3D11Device::CreateDepthStencilView(ID3D11Resource *  pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *  pDesc, ID3D11DepthStencilView * *  ppDepthStencilView)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateDepthStencilView");

    HRESULT result = base->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);

    return result;
}


HRESULT myD3D11Device::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *  pInputElementDescs, UINT  NumElements, const void *  pShaderBytecodeWithInputSignature, SIZE_T  BytecodeLength, ID3D11InputLayout * *  ppInputLayout)
{
    if (g_logger->logInterfaceCalls)
    {
        g_logger->log("CreateInputLayout");
        g_logger->logInputLayoutFile << endl << " *** New input layout:" << endl;
        for (int elem = 0; elem < (int)NumElements; elem++)
        {
            const D3D11_INPUT_ELEMENT_DESC &desc = pInputElementDescs[elem];
            g_logger->logInputLayoutFile << desc.SemanticName << "-" << desc.SemanticIndex << ", offset=" << desc.AlignedByteOffset << ", format=" << desc.Format << endl;
            
            if (desc.InputSlot != 0)
                g_logger->logInputLayoutFile << "InputSlot=" << desc.InputSlot << endl;

            if (desc.InputSlotClass != 0)
                g_logger->logInputLayoutFile << "InputSlotClass=" << desc.InputSlotClass << endl;

            if (desc.InstanceDataStepRate != 0)
                g_logger->logInputLayoutFile << "InstanceDataStepRate=" << desc.InstanceDataStepRate << endl;
        }
    }

    HRESULT result = base->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);

    if (*ppInputLayout != nullptr && SUCCEEDED(result))
    {
        g_logger->logInputLayoutFile << endl << "Handle=" << pointerToString(*ppInputLayout) << endl;
        assets->vertexLayouts[(UINT64)*ppInputLayout] = new VertexLayout(pInputElementDescs, NumElements);
    }

    return result;
}


HRESULT myD3D11Device::CreateVertexShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11VertexShader * *  ppVertexShader)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateVertexShader");

    HRESULT result = base->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);

    return result;
}


HRESULT myD3D11Device::CreateGeometryShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11GeometryShader * *  ppGeometryShader)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateGeometryShader");

    HRESULT result = base->CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);

    return result;
}


HRESULT myD3D11Device::CreateGeometryShaderWithStreamOutput(const void *  pShaderBytecode, SIZE_T  BytecodeLength, const D3D11_SO_DECLARATION_ENTRY *  pSODeclaration, UINT  NumEntries, const UINT *  pBufferStrides, UINT  NumStrides, UINT  RasterizedStream, ID3D11ClassLinkage *  pClassLinkage, ID3D11GeometryShader * *  ppGeometryShader)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateGeometryShaderWithStreamOutput");

    HRESULT result = base->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);

    return result;
}


HRESULT myD3D11Device::CreatePixelShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11PixelShader * *  ppPixelShader)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreatePixelShader");

    HRESULT result = base->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);

    return result;
}


HRESULT myD3D11Device::CreateHullShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11HullShader * *  ppHullShader)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateHullShader");

    HRESULT result = base->CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);

    return result;
}


HRESULT myD3D11Device::CreateDomainShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11DomainShader * *  ppDomainShader)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateDomainShader");

    HRESULT result = base->CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);

    return result;
}


HRESULT myD3D11Device::CreateComputeShader(const void *  pShaderBytecode, SIZE_T  BytecodeLength, ID3D11ClassLinkage *  pClassLinkage, ID3D11ComputeShader * *  ppComputeShader)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateComputeShader");

    HRESULT result = base->CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);

    return result;
}


HRESULT myD3D11Device::CreateClassLinkage(ID3D11ClassLinkage * *  ppLinkage)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateClassLinkage");

    HRESULT result = base->CreateClassLinkage(ppLinkage);

    return result;
}


HRESULT myD3D11Device::CreateBlendState(const D3D11_BLEND_DESC *  pBlendStateDesc, ID3D11BlendState * *  ppBlendState)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateBlendState");

    HRESULT result = base->CreateBlendState(pBlendStateDesc, ppBlendState);

    return result;
}


HRESULT myD3D11Device::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC *  pDepthStencilDesc, ID3D11DepthStencilState * *  ppDepthStencilState)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateDepthStencilState");

    HRESULT result = base->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);

    return result;
}


HRESULT myD3D11Device::CreateRasterizerState(const D3D11_RASTERIZER_DESC *  pRasterizerDesc, ID3D11RasterizerState * *  ppRasterizerState)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateRasterizerState");

    HRESULT result = base->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);

    return result;
}


HRESULT myD3D11Device::CreateSamplerState(const D3D11_SAMPLER_DESC *  pSamplerDesc, ID3D11SamplerState * *  ppSamplerState)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateSamplerState");

    HRESULT result = base->CreateSamplerState(pSamplerDesc, ppSamplerState);

    return result;
}


HRESULT myD3D11Device::CreateQuery(const D3D11_QUERY_DESC *  pQueryDesc, ID3D11Query * *  ppQuery)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateQuery");

    HRESULT result = base->CreateQuery(pQueryDesc, ppQuery);

    return result;
}


HRESULT myD3D11Device::CreatePredicate(const D3D11_QUERY_DESC *  pPredicateDesc, ID3D11Predicate * *  ppPredicate)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreatePredicate");

    HRESULT result = base->CreatePredicate(pPredicateDesc, ppPredicate);

    return result;
}


HRESULT myD3D11Device::CreateCounter(const D3D11_COUNTER_DESC *  pCounterDesc, ID3D11Counter * *  ppCounter)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateCounter");

    HRESULT result = base->CreateCounter(pCounterDesc, ppCounter);

    return result;
}


HRESULT myD3D11Device::CreateDeferredContext(UINT  ContextFlags, ID3D11DeviceContext * *  ppDeferredContext)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CreateDeferredContext");

    HRESULT result = base->CreateDeferredContext(ContextFlags, ppDeferredContext);

    return result;
}


HRESULT myD3D11Device::OpenSharedResource(HANDLE  hResource, REFIID  ReturnedInterface, void * *  ppResource)
{
    if (g_logger->logInterfaceCalls) g_logger->log("OpenSharedResource");

    HRESULT result = base->OpenSharedResource(hResource, ReturnedInterface, ppResource);

    return result;
}


HRESULT myD3D11Device::CheckFormatSupport(DXGI_FORMAT  Format, UINT *  pFormatSupport)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CheckFormatSupport");

    HRESULT result = base->CheckFormatSupport(Format, pFormatSupport);

    return result;
}


HRESULT myD3D11Device::CheckMultisampleQualityLevels(DXGI_FORMAT  Format, UINT  SampleCount, UINT *  pNumQualityLevels)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CheckMultisampleQualityLevels");

    HRESULT result = base->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);

    return result;
}


void myD3D11Device::CheckCounterInfo(D3D11_COUNTER_INFO *  pCounterInfo)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CheckCounterInfo");

    base->CheckCounterInfo(pCounterInfo);
}


HRESULT myD3D11Device::CheckCounter(const D3D11_COUNTER_DESC *  pDesc, D3D11_COUNTER_TYPE *  pType, UINT *  pActiveCounters, LPSTR  szName, _Inout_ UINT *  pNameLength, LPSTR  szUnits, _Inout_ UINT *  pUnitsLength, LPSTR  szDescription, _Inout_ UINT *  pDescriptionLength)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CheckCounter");

    HRESULT result = base->CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);

    return result;
}


HRESULT myD3D11Device::CheckFeatureSupport(D3D11_FEATURE  Feature, void *  pFeatureSupportData, UINT  FeatureSupportDataSize)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CheckFeatureSupport");

    HRESULT result = base->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);

    return result;
}


HRESULT myD3D11Device::GetPrivateData(REFGUID  guid, _Inout_ UINT *  pDataSize, void *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetPrivateData");

    HRESULT result = base->GetPrivateData(guid, pDataSize, pData);

    return result;
}


HRESULT myD3D11Device::SetPrivateData(REFGUID  guid, UINT  DataSize, const void *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("SetPrivateData");

    HRESULT result = base->SetPrivateData(guid, DataSize, pData);

    return result;
}


HRESULT myD3D11Device::SetPrivateDataInterface(REFGUID  guid, const IUnknown *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("SetPrivateDataInterface");

    HRESULT result = base->SetPrivateDataInterface(guid, pData);

    return result;
}


D3D_FEATURE_LEVEL myD3D11Device::GetFeatureLevel(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetFeatureLevel");

    D3D_FEATURE_LEVEL result = base->GetFeatureLevel();

    return result;
}


UINT myD3D11Device::GetCreationFlags(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetCreationFlags");

    UINT result = base->GetCreationFlags();

    return result;
}


HRESULT myD3D11Device::GetDeviceRemovedReason(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetDeviceRemovedReason");

    HRESULT result = base->GetDeviceRemovedReason();

    return result;
}


void myD3D11Device::GetImmediateContext(ID3D11DeviceContext * *  ppImmediateContext)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetImmediateContext");

    base->GetImmediateContext(ppImmediateContext);
}


HRESULT myD3D11Device::SetExceptionMode(UINT  RaiseFlags)
{
    if (g_logger->logInterfaceCalls) g_logger->log("SetExceptionMode");

    HRESULT result = base->SetExceptionMode(RaiseFlags);

    return result;
}


UINT myD3D11Device::GetExceptionMode(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetExceptionMode");

    UINT result = base->GetExceptionMode();

    return result;
}


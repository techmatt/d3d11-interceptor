
#include "main.h"

void myD3D11DeviceContext::readSwapChain(Bitmap &result)
{
    ID3D11Texture2D* swapChainTexture;
    HRESULT hr = assets->swapChain->base->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast< void** >(&swapChainTexture));
    if (FAILED(hr))
    {
        g_logger->logErrorFile << "assets.swapChain->base->GetBuffer failed" << endl;
    }
    else
    {
        readTexture(swapChainTexture, result);
    }

    swapChainTexture->Release();
}

void myD3D11DeviceContext::readRenderTarget(Bitmap &result)
{
    ID3D11RenderTargetView *view;
    assets->context->base->OMGetRenderTargets(1, &view, nullptr);

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

void myD3D11DeviceContext::readTexture(ID3D11Texture2D *inputTexture, Bitmap &result)
{
    ID3D11Texture2D *captureTexture;

    D3D11_TEXTURE2D_DESC renderDesc;
    inputTexture->GetDesc(&renderDesc);

    renderDesc.MipLevels = 1;
    renderDesc.ArraySize = 1;
    renderDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    renderDesc.SampleDesc.Count = 1;
    renderDesc.SampleDesc.Quality = 0;
    renderDesc.BindFlags = 0;
    renderDesc.MiscFlags = 0;
    renderDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    renderDesc.Usage = D3D11_USAGE_STAGING;
    assets->device->base->CreateTexture2D(&renderDesc, nullptr, &captureTexture);

    assets->context->base->CopyResource(captureTexture, inputTexture);

    result.allocate(renderDesc.Width, renderDesc.Height);

    D3D11_MAPPED_SUBRESOURCE resource;
    UINT subresource = D3D11CalcSubresource(0, 0, 0);
    HRESULT hr = assets->context->base->Map(captureTexture, subresource, D3D11_MAP_READ, 0, &resource);
    const BYTE *data = (BYTE *)resource.pData;

    for (UINT y = 0; y < renderDesc.Height; y++)
    {
        memcpy(&result(0U, y), data + resource.RowPitch * y, renderDesc.Width * sizeof(ml::vec4uc));
    }

    assets->context->base->Unmap(captureTexture, subresource);

    captureTexture->Release();
}

void myD3D11DeviceContext::GetDevice(ID3D11Device * *  ppDevice)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetDevice");

    base->GetDevice(ppDevice);
}


HRESULT myD3D11DeviceContext::GetPrivateData(REFGUID  guid, _Inout_ UINT *  pDataSize, void *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetPrivateData");

    HRESULT result = base->GetPrivateData(guid, pDataSize, pData);

    return result;
}


HRESULT myD3D11DeviceContext::SetPrivateData(REFGUID  guid, UINT  DataSize, const void *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("SetPrivateData");

    HRESULT result = base->SetPrivateData(guid, DataSize, pData);

    return result;
}


HRESULT myD3D11DeviceContext::SetPrivateDataInterface(REFGUID  guid, const IUnknown *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("SetPrivateDataInterface");

    HRESULT result = base->SetPrivateDataInterface(guid, pData);

    return result;
}


void myD3D11DeviceContext::VSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls)
    {
        string pointer = ppConstantBuffers == nullptr ? "<invalid>" : pointerToString(ppConstantBuffers[0]);
        g_logger->log("VSSetConstantBuffers StartSlot=" + to_string(StartSlot) + ", NumBuffers=" + to_string(NumBuffers) + ", buffers[0]=" + pointer);
    }

    base->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::PSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->logInterfaceFile << "PSSetShaderResources StartSlot=" << StartSlot << ", NumViews=" << NumViews << endl;
    if (g_logger->capturingFrame)
    {
        string r0Type = "<invalid>";
        int r0Width = 0;
        int r0Height = 0;

        if (ppShaderResourceViews != nullptr && ppShaderResourceViews[0] != nullptr)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc;
            ppShaderResourceViews[0]->GetDesc(&desc);

            switch (desc.ViewDimension)
            {
            case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
                {
                    r0Type = "texture2DArray(" + to_string(desc.Texture2DArray.ArraySize) + ")";
                }
                break;
            default:
                r0Type = "unknown-" + to_string(desc.ViewDimension);
            }
        }

        g_logger->logFrameCaptureFile << "PSSetShaderResources StartSlot=" << StartSlot << ", NumViews=" << NumViews << ", view0Stats=" << r0Type << "," << r0Width << "," << r0Height << endl;
    }

    base->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::PSSetShader(ID3D11PixelShader *  pPixelShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("PSSetShader " + pointerToString(pPixelShader));

    base->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::PSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("PSSetSamplers");

    base->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::VSSetShader(ID3D11VertexShader *  pVertexShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("VSSetShader " + pointerToString(pVertexShader));

    base->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::DrawIndexed(UINT  IndexCount, UINT  StartIndexLocation, INT  BaseVertexLocation)
{
    if (g_logger->logInterfaceCalls) g_logger->logInterfaceFile << "DrawIndexed" << endl;
    if (g_logger->logDrawCalls) g_logger->logDrawFile << "DrawIndexed IndexCount=" << IndexCount << ", StartIndexLocation=" << StartIndexLocation << ", BaseVertexLocation=" << BaseVertexLocation << endl;
    
    DrawParameters params(IndexCount, StartIndexLocation, BaseVertexLocation);
    
    g_logger->recordSignatureColorPreDraw(*assets, params);

    base->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);

    g_logger->recordSignatureColorPostDraw(*assets, params);

    const bool reportAIRender = g_logger->capturingFrame;
    if (reportAIRender)
    {
        //g_state->AI->drawIndexed(VSBuffer, VSBuffer, IndexCount, StartIndexLocation, BaseVertexLocation);
    }

    if (g_logger->capturingFrame)
    {
        g_logger->logFrameCaptureFile << "DrawIndexed-" << g_logger->frameRenderIndex << " IndexCount=" << IndexCount << ", StartIndexLocation=" << StartIndexLocation << ", BaseVertexLocation=" << BaseVertexLocation << endl;
        g_logger->recordDrawEvent(*assets, params);
    }
}


void myD3D11DeviceContext::Draw(UINT  VertexCount, UINT  StartVertexLocation)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Draw");
    if (g_logger->logDrawCalls) g_logger->logDrawFile << "Draw vertexCount=" << VertexCount << ", StartVertexLocation=" << StartVertexLocation << endl;

    base->Draw(VertexCount, StartVertexLocation);

    if (g_logger->capturingFrame)
    {
        g_logger->logFrameCaptureFile << "Draw vertexCount=" << VertexCount << ", StartVertexLocation=" << StartVertexLocation << endl;
        g_logger->recordDrawEvent(*assets, DrawParameters(VertexCount, StartVertexLocation, -1));
    }
}


HRESULT myD3D11DeviceContext::Map(ID3D11Resource *  pResource, UINT  Subresource, D3D11_MAP  MapType, UINT  MapFlags, D3D11_MAPPED_SUBRESOURCE *  pMappedResource)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Map resource=" + pointerToString(pResource) + " subResource=" + to_string(Subresource) + " type=" + to_string(MapType));

    UINT64 ptr = (UINT64)pResource;
    if (capturingAllBuffers &&
        (MapType == D3D11_MAP_WRITE_NO_OVERWRITE || MapType == D3D11_MAP_WRITE_DISCARD || MapType == D3D11_MAP_WRITE ||
         MapType == (D3D11_MAP)1234) &&
        assets->buffers.count(ptr) > 0)
    {
        auto &buffer = *assets->buffers.find(ptr)->second;

        bool shim = (buffer.data.size() < 1024 * 1024 || MapType == (D3D11_MAP)1234);
        
        if (shim)
        {
            *pMappedResource = buffer.mappedResource;
            if (MapType != (D3D11_MAP)1234)
            {
                buffer.mapped = true;
                buffer.mapType = MapType;
                buffer.mapFlags = MapFlags;
            }
            return S_OK;
        }
    }

    HRESULT result = base->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);

    return result;
}


void myD3D11DeviceContext::Unmap(ID3D11Resource *  pResource, UINT  Subresource)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Unmap resource=" + pointerToString(pResource) + " subResource=" + to_string(Subresource));

    UINT64 ptr = (UINT64)pResource;
    if (assets->buffers.count(ptr) > 0)
    {
        auto &buffer = *assets->buffers.find(ptr)->second;
        if (buffer.mapped)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            base->Map(pResource, Subresource, buffer.mapType, buffer.mapFlags, &mappedResource);
            memcpy(mappedResource.pData, buffer.mappedResource.pData, buffer.mappedResource.RowPitch);
            buffer.mapped = false;
        }
    }

    base->Unmap(pResource, Subresource);
}


void myD3D11DeviceContext::PSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("PSSetConstantBuffers");

    base->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::IASetInputLayout(ID3D11InputLayout *  pInputLayout)
{
    if (g_logger->logInterfaceCalls) g_logger->log("IASetInputLayout handle=" + pointerToString(pInputLayout));

    
    if (assets->vertexLayouts.count((UINT64)pInputLayout) == 0)
        assets->activeVertexLayout = nullptr;
    else
        assets->activeVertexLayout = assets->vertexLayouts.find((UINT64)pInputLayout)->second;

    base->IASetInputLayout(pInputLayout);
}


void myD3D11DeviceContext::IASetVertexBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppVertexBuffers, const UINT *  pStrides, const UINT *  pOffsets)
{
    if (g_logger->logInterfaceCalls) g_logger->log("IASetVertexBuffers");

    base->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}


void myD3D11DeviceContext::IASetIndexBuffer(ID3D11Buffer *  pIndexBuffer, DXGI_FORMAT  Format, UINT  Offset)
{
    if (g_logger->logInterfaceCalls) g_logger->log("IASetIndexBuffer");

    base->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}


void myD3D11DeviceContext::DrawIndexedInstanced(UINT  IndexCountPerInstance, UINT  InstanceCount, UINT  StartIndexLocation, INT  BaseVertexLocation, UINT  StartInstanceLocation)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DrawIndexedInstanced");

    base->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}


void myD3D11DeviceContext::DrawInstanced(UINT  VertexCountPerInstance, UINT  InstanceCount, UINT  StartVertexLocation, UINT  StartInstanceLocation)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DrawInstanced");

    base->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}


void myD3D11DeviceContext::GSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GSSetConstantBuffers");

    base->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::GSSetShader(ID3D11GeometryShader *  pShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GSSetShader");

    base->GSSetShader(pShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY  Topology)
{
    if (g_logger->logInterfaceCalls) g_logger->log("IASetPrimitiveTopology");

    base->IASetPrimitiveTopology(Topology);
}


void myD3D11DeviceContext::VSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("VSSetShaderResources");

    base->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::VSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("VSSetSamplers");

    base->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::Begin(ID3D11Asynchronous *  pAsync)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Begin");

    base->Begin(pAsync);
}


void myD3D11DeviceContext::End(ID3D11Asynchronous *  pAsync)
{
    if (g_logger->logInterfaceCalls) g_logger->log("End");

    base->End(pAsync);
}


HRESULT myD3D11DeviceContext::GetData(ID3D11Asynchronous *  pAsync, void *  pData, UINT  DataSize, UINT  GetDataFlags)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetData");

    HRESULT result = base->GetData(pAsync, pData, DataSize, GetDataFlags);

    return result;
}


void myD3D11DeviceContext::SetPredication(ID3D11Predicate *  pPredicate, BOOL  PredicateValue)
{
    if (g_logger->logInterfaceCalls) g_logger->log("SetPredication");

    base->SetPredication(pPredicate, PredicateValue);
}


void myD3D11DeviceContext::GSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GSSetShaderResources");

    base->GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::GSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GSSetSamplers");

    base->GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::OMSetRenderTargets(UINT  NumViews, ID3D11RenderTargetView * const *  ppRenderTargetViews, ID3D11DepthStencilView *  pDepthStencilView)
{
    if (g_logger->logInterfaceCalls) g_logger->log("OMSetRenderTargets");

    base->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}


void myD3D11DeviceContext::OMSetRenderTargetsAndUnorderedAccessViews(UINT  NumRTVs, ID3D11RenderTargetView * const *  ppRenderTargetViews, ID3D11DepthStencilView *  pDepthStencilView, UINT  UAVStartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * const *  ppUnorderedAccessViews, const UINT *  pUAVInitialCounts)
{
    if (g_logger->logInterfaceCalls) g_logger->log("OMSetRenderTargetsAndUnorderedAccessViews");

    base->OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}


void myD3D11DeviceContext::OMSetBlendState(ID3D11BlendState *  pBlendState, const FLOAT BlendFactor[4], UINT  SampleMask)
{
    if (g_logger->logInterfaceCalls) g_logger->log("OMSetBlendState");

    base->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
}


void myD3D11DeviceContext::OMSetDepthStencilState(ID3D11DepthStencilState *  pDepthStencilState, UINT  StencilRef)
{
    if (g_logger->logInterfaceCalls) g_logger->log("OMSetDepthStencilState");

    base->OMSetDepthStencilState(pDepthStencilState, StencilRef);
}


void myD3D11DeviceContext::SOSetTargets(UINT  NumBuffers, ID3D11Buffer * const *  ppSOTargets, const UINT *  pOffsets)
{
    if (g_logger->logInterfaceCalls) g_logger->log("SOSetTargets");

    base->SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
}


void myD3D11DeviceContext::DrawAuto(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DrawAuto");

    base->DrawAuto();
}


void myD3D11DeviceContext::DrawIndexedInstancedIndirect(ID3D11Buffer *  pBufferForArgs, UINT  AlignedByteOffsetForArgs)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DrawIndexedInstancedIndirect");

    base->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}


void myD3D11DeviceContext::DrawInstancedIndirect(ID3D11Buffer *  pBufferForArgs, UINT  AlignedByteOffsetForArgs)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DrawInstancedIndirect");

    base->DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}


void myD3D11DeviceContext::Dispatch(UINT  ThreadGroupCountX, UINT  ThreadGroupCountY, UINT  ThreadGroupCountZ)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Dispatch");

    base->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}


void myD3D11DeviceContext::DispatchIndirect(ID3D11Buffer *  pBufferForArgs, UINT  AlignedByteOffsetForArgs)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DispatchIndirect");

    base->DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}


void myD3D11DeviceContext::RSSetState(ID3D11RasterizerState *  pRasterizerState)
{
    if (g_logger->logInterfaceCalls) g_logger->log("RSSetState");

    base->RSSetState(pRasterizerState);
}


void myD3D11DeviceContext::RSSetViewports(UINT  NumViewports, const D3D11_VIEWPORT *  pViewports)
{
    if (g_logger->logInterfaceCalls) g_logger->log("RSSetViewports");

    base->RSSetViewports(NumViewports, pViewports);
}


void myD3D11DeviceContext::RSSetScissorRects(UINT  NumRects, const D3D11_RECT *  pRects)
{
    if (g_logger->logInterfaceCalls) g_logger->log("RSSetScissorRects");

    base->RSSetScissorRects(NumRects, pRects);
}


void myD3D11DeviceContext::CopySubresourceRegion(ID3D11Resource *  pDstResource, UINT  DstSubresource, UINT  DstX, UINT  DstY, UINT  DstZ, ID3D11Resource *  pSrcResource, UINT  SrcSubresource, const D3D11_BOX *  pSrcBox)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CopySubresourceRegion");

    base->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
}


void myD3D11DeviceContext::CopyResource(ID3D11Resource *  pDstResource, ID3D11Resource *  pSrcResource)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CopyResource dst=" + pointerToString(pDstResource) + " src=" + pointerToString(pSrcResource));

    base->CopyResource(pDstResource, pSrcResource);
}


void myD3D11DeviceContext::UpdateSubresource(ID3D11Resource *  pDstResource, UINT  DstSubresource, const D3D11_BOX *  pDstBox, const void *  pSrcData, UINT  SrcRowPitch, UINT  SrcDepthPitch)
{
    if (g_logger->logInterfaceCalls) g_logger->log("UpdateSubresource");

    base->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}


void myD3D11DeviceContext::CopyStructureCount(ID3D11Buffer *  pDstBuffer, UINT  DstAlignedByteOffset, ID3D11UnorderedAccessView *  pSrcView)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CopyStructureCount");

    base->CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
}


void myD3D11DeviceContext::ClearRenderTargetView(ID3D11RenderTargetView *  pRenderTargetView, const FLOAT ColorRGBA[4])
{
    if (g_logger->logInterfaceCalls) g_logger->log("ClearRenderTargetView");

    base->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
}


void myD3D11DeviceContext::ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView *  pUnorderedAccessView, const UINT Values[4])
{
    if (g_logger->logInterfaceCalls) g_logger->log("ClearUnorderedAccessViewUint");

    base->ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
}


void myD3D11DeviceContext::ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView *  pUnorderedAccessView, const FLOAT Values[4])
{
    if (g_logger->logInterfaceCalls) g_logger->log("ClearUnorderedAccessViewFloat");

    base->ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
}


void myD3D11DeviceContext::ClearDepthStencilView(ID3D11DepthStencilView *  pDepthStencilView, UINT  ClearFlags, FLOAT  Depth, UINT8  Stencil)
{
    if (g_logger->logInterfaceCalls) g_logger->log("ClearDepthStencilView");

    base->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
}


void myD3D11DeviceContext::GenerateMips(ID3D11ShaderResourceView *  pShaderResourceView)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GenerateMips");

    base->GenerateMips(pShaderResourceView);
}


void myD3D11DeviceContext::SetResourceMinLOD(ID3D11Resource *  pResource, FLOAT  MinLOD)
{
    if (g_logger->logInterfaceCalls) g_logger->log("SetResourceMinLOD");

    base->SetResourceMinLOD(pResource, MinLOD);
}


FLOAT myD3D11DeviceContext::GetResourceMinLOD(ID3D11Resource *  pResource)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetResourceMinLOD");

    FLOAT result = base->GetResourceMinLOD(pResource);

    return result;
}


void myD3D11DeviceContext::ResolveSubresource(ID3D11Resource *  pDstResource, UINT  DstSubresource, ID3D11Resource *  pSrcResource, UINT  SrcSubresource, DXGI_FORMAT  Format)
{
    if (g_logger->logInterfaceCalls) g_logger->log("ResolveSubresource");

    base->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
}


void myD3D11DeviceContext::ExecuteCommandList(ID3D11CommandList *  pCommandList, BOOL  RestoreContextState)
{
    if (g_logger->logInterfaceCalls) g_logger->log("ExecuteCommandList");

    base->ExecuteCommandList(pCommandList, RestoreContextState);
}


void myD3D11DeviceContext::HSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("HSSetShaderResources");

    base->HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::HSSetShader(ID3D11HullShader *  pHullShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("HSSetShader");

    base->HSSetShader(pHullShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::HSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("HSSetSamplers");

    base->HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::HSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("HSSetConstantBuffers");

    base->HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::DSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DSSetShaderResources");

    base->DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::DSSetShader(ID3D11DomainShader *  pDomainShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DSSetShader");

    base->DSSetShader(pDomainShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::DSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DSSetSamplers");

    base->DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::DSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DSSetConstantBuffers");

    base->DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::CSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSSetShaderResources");

    base->CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::CSSetUnorderedAccessViews(UINT  StartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * const *  ppUnorderedAccessViews, const UINT *  pUAVInitialCounts)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSSetUnorderedAccessViews");

    base->CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}


void myD3D11DeviceContext::CSSetShader(ID3D11ComputeShader *  pComputeShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSSetShader");

    base->CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::CSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSSetSamplers");

    base->CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::CSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSSetConstantBuffers");

    base->CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::VSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("VSGetConstantBuffers");

    base->VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::PSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("PSGetShaderResources");

    base->PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::PSGetShader(ID3D11PixelShader * *  ppPixelShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("PSGetShader");

    base->PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::PSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("PSGetSamplers");

    base->PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::VSGetShader(ID3D11VertexShader * *  ppVertexShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("VSGetShader");

    base->VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::PSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("PSGetConstantBuffers");

    base->PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::IAGetInputLayout(ID3D11InputLayout * *  ppInputLayout)
{
    if (g_logger->logInterfaceCalls) g_logger->log("IAGetInputLayout");

    base->IAGetInputLayout(ppInputLayout);
}


void myD3D11DeviceContext::IAGetVertexBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppVertexBuffers, UINT *  pStrides, UINT *  pOffsets)
{
    if (g_logger->logInterfaceCalls) g_logger->log("IAGetVertexBuffers");

    base->IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}


void myD3D11DeviceContext::IAGetIndexBuffer(ID3D11Buffer * *  pIndexBuffer, DXGI_FORMAT *  Format, UINT *  Offset)
{
    if (g_logger->logInterfaceCalls) g_logger->log("IAGetIndexBuffer");

    base->IAGetIndexBuffer(pIndexBuffer, Format, Offset);
}


void myD3D11DeviceContext::GSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GSGetConstantBuffers");

    base->GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::GSGetShader(ID3D11GeometryShader * *  ppGeometryShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GSGetShader");

    base->GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::IAGetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY *  pTopology)
{
    if (g_logger->logInterfaceCalls) g_logger->log("IAGetPrimitiveTopology");

    base->IAGetPrimitiveTopology(pTopology);
}


void myD3D11DeviceContext::VSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("VSGetShaderResources");

    base->VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::VSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("VSGetSamplers");

    base->VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::GetPredication(ID3D11Predicate * *  ppPredicate, BOOL *  pPredicateValue)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetPredication");

    base->GetPredication(ppPredicate, pPredicateValue);
}


void myD3D11DeviceContext::GSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GSGetShaderResources");

    base->GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::GSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GSGetSamplers");

    base->GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::OMGetRenderTargets(UINT  NumViews, ID3D11RenderTargetView * *  ppRenderTargetViews, ID3D11DepthStencilView * *  ppDepthStencilView)
{
    if (g_logger->logInterfaceCalls) g_logger->log("OMGetRenderTargets");

    base->OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
}


void myD3D11DeviceContext::OMGetRenderTargetsAndUnorderedAccessViews(UINT  NumRTVs, ID3D11RenderTargetView * *  ppRenderTargetViews, ID3D11DepthStencilView * *  ppDepthStencilView, UINT  UAVStartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * *  ppUnorderedAccessViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("OMGetRenderTargetsAndUnorderedAccessViews");

    base->OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
}


void myD3D11DeviceContext::OMGetBlendState(ID3D11BlendState * *  ppBlendState, FLOAT BlendFactor[4], UINT *  pSampleMask)
{
    if (g_logger->logInterfaceCalls) g_logger->log("OMGetBlendState");

    base->OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
}


void myD3D11DeviceContext::OMGetDepthStencilState(ID3D11DepthStencilState * *  ppDepthStencilState, UINT *  pStencilRef)
{
    if (g_logger->logInterfaceCalls) g_logger->log("OMGetDepthStencilState");

    base->OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
}


void myD3D11DeviceContext::SOGetTargets(UINT  NumBuffers, ID3D11Buffer * *  ppSOTargets)
{
    if (g_logger->logInterfaceCalls) g_logger->log("SOGetTargets");

    base->SOGetTargets(NumBuffers, ppSOTargets);
}


void myD3D11DeviceContext::RSGetState(ID3D11RasterizerState * *  ppRasterizerState)
{
    if (g_logger->logInterfaceCalls) g_logger->log("RSGetState");

    base->RSGetState(ppRasterizerState);
}


void myD3D11DeviceContext::RSGetViewports(UINT *  pNumViewports, D3D11_VIEWPORT *  pViewports)
{
    if (g_logger->logInterfaceCalls) g_logger->log("RSGetViewports");

    base->RSGetViewports(pNumViewports, pViewports);
}


void myD3D11DeviceContext::RSGetScissorRects(UINT *  pNumRects, D3D11_RECT *  pRects)
{
    if (g_logger->logInterfaceCalls) g_logger->log("RSGetScissorRects");

    base->RSGetScissorRects(pNumRects, pRects);
}


void myD3D11DeviceContext::HSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("HSGetShaderResources");

    base->HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::HSGetShader(ID3D11HullShader * *  ppHullShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("HSGetShader");

    base->HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::HSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("HSGetSamplers");

    base->HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::HSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("HSGetConstantBuffers");

    base->HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::DSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DSGetShaderResources");

    base->DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::DSGetShader(ID3D11DomainShader * *  ppDomainShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DSGetShader");

    base->DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::DSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DSGetSamplers");

    base->DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::DSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("DSGetConstantBuffers");

    base->DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::CSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSGetShaderResources");

    base->CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::CSGetUnorderedAccessViews(UINT  StartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * *  ppUnorderedAccessViews)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSGetUnorderedAccessViews");

    base->CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
}


void myD3D11DeviceContext::CSGetShader(ID3D11ComputeShader * *  ppComputeShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSGetShader");

    base->CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::CSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSGetSamplers");

    base->CSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::CSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    if (g_logger->logInterfaceCalls) g_logger->log("CSGetConstantBuffers");

    base->CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::ClearState(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("ClearState");

    base->ClearState();
}


void myD3D11DeviceContext::Flush(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Flush");

    base->Flush();
}


D3D11_DEVICE_CONTEXT_TYPE myD3D11DeviceContext::GetType(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetType");

    D3D11_DEVICE_CONTEXT_TYPE result = base->GetType();

    return result;
}


UINT myD3D11DeviceContext::GetContextFlags(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("GetContextFlags");

    UINT result = base->GetContextFlags();

    return result;
}


HRESULT myD3D11DeviceContext::FinishCommandList(BOOL  RestoreDeferredContextState, ID3D11CommandList * *  ppCommandList)
{
    if (g_logger->logInterfaceCalls) g_logger->log("FinishCommandList");

    HRESULT result = base->FinishCommandList(RestoreDeferredContextState, ppCommandList);

    return result;
}



#include "main.h"


void myD3D11DeviceContext::GetDevice(ID3D11Device * *  ppDevice)
{
    g_logger->log("Interface call: GetDevice");

    base->GetDevice(ppDevice);
}


HRESULT myD3D11DeviceContext::GetPrivateData(REFGUID  guid, _Inout_ UINT *  pDataSize, void *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetPrivateData");

    HRESULT result = base->GetPrivateData(guid, pDataSize, pData);

    return result;
}


HRESULT myD3D11DeviceContext::SetPrivateData(REFGUID  guid, UINT  DataSize, const void *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: SetPrivateData");

    HRESULT result = base->SetPrivateData(guid, DataSize, pData);

    return result;
}


HRESULT myD3D11DeviceContext::SetPrivateDataInterface(REFGUID  guid, const IUnknown *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: SetPrivateDataInterface");

    HRESULT result = base->SetPrivateDataInterface(guid, pData);

    return result;
}


void myD3D11DeviceContext::VSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    g_logger->log("Interface call: VSSetConstantBuffers");

    base->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::PSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    g_logger->log("Interface call: PSSetShaderResources");

    base->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::PSSetShader(ID3D11PixelShader *  pPixelShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    g_logger->log("Interface call: PSSetShader");

    base->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::PSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    g_logger->log("Interface call: PSSetSamplers");

    base->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::VSSetShader(ID3D11VertexShader *  pVertexShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    g_logger->log("Interface call: VSSetShader");

    base->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::DrawIndexed(UINT  IndexCount, UINT  StartIndexLocation, INT  BaseVertexLocation)
{
    g_logger->log("Interface call: DrawIndexed");

    base->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}


void myD3D11DeviceContext::Draw(UINT  VertexCount, UINT  StartVertexLocation)
{
    g_logger->log("Interface call: Draw");

    base->Draw(VertexCount, StartVertexLocation);
}


HRESULT myD3D11DeviceContext::Map(ID3D11Resource *  pResource, UINT  Subresource, D3D11_MAP  MapType, UINT  MapFlags, D3D11_MAPPED_SUBRESOURCE *  pMappedResource)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: Map");

    HRESULT result = base->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);

    return result;
}


void myD3D11DeviceContext::Unmap(ID3D11Resource *  pResource, UINT  Subresource)
{
    g_logger->log("Interface call: Unmap");

    base->Unmap(pResource, Subresource);
}


void myD3D11DeviceContext::PSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    g_logger->log("Interface call: PSSetConstantBuffers");

    base->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::IASetInputLayout(ID3D11InputLayout *  pInputLayout)
{
    g_logger->log("Interface call: IASetInputLayout");

    base->IASetInputLayout(pInputLayout);
}


void myD3D11DeviceContext::IASetVertexBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppVertexBuffers, const UINT *  pStrides, const UINT *  pOffsets)
{
    g_logger->log("Interface call: IASetVertexBuffers");

    base->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}


void myD3D11DeviceContext::IASetIndexBuffer(ID3D11Buffer *  pIndexBuffer, DXGI_FORMAT  Format, UINT  Offset)
{
    g_logger->log("Interface call: IASetIndexBuffer");

    base->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}


void myD3D11DeviceContext::DrawIndexedInstanced(UINT  IndexCountPerInstance, UINT  InstanceCount, UINT  StartIndexLocation, INT  BaseVertexLocation, UINT  StartInstanceLocation)
{
    g_logger->log("Interface call: DrawIndexedInstanced");

    base->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}


void myD3D11DeviceContext::DrawInstanced(UINT  VertexCountPerInstance, UINT  InstanceCount, UINT  StartVertexLocation, UINT  StartInstanceLocation)
{
    g_logger->log("Interface call: DrawInstanced");

    base->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}


void myD3D11DeviceContext::GSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    g_logger->log("Interface call: GSSetConstantBuffers");

    base->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::GSSetShader(ID3D11GeometryShader *  pShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    g_logger->log("Interface call: GSSetShader");

    base->GSSetShader(pShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY  Topology)
{
    g_logger->log("Interface call: IASetPrimitiveTopology");

    base->IASetPrimitiveTopology(Topology);
}


void myD3D11DeviceContext::VSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    g_logger->log("Interface call: VSSetShaderResources");

    base->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::VSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    g_logger->log("Interface call: VSSetSamplers");

    base->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::Begin(ID3D11Asynchronous *  pAsync)
{
    g_logger->log("Interface call: Begin");

    base->Begin(pAsync);
}


void myD3D11DeviceContext::End(ID3D11Asynchronous *  pAsync)
{
    g_logger->log("Interface call: End");

    base->End(pAsync);
}


HRESULT myD3D11DeviceContext::GetData(ID3D11Asynchronous *  pAsync, void *  pData, UINT  DataSize, UINT  GetDataFlags)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetData");

    HRESULT result = base->GetData(pAsync, pData, DataSize, GetDataFlags);

    return result;
}


void myD3D11DeviceContext::SetPredication(ID3D11Predicate *  pPredicate, BOOL  PredicateValue)
{
    g_logger->log("Interface call: SetPredication");

    base->SetPredication(pPredicate, PredicateValue);
}


void myD3D11DeviceContext::GSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    g_logger->log("Interface call: GSSetShaderResources");

    base->GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::GSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    g_logger->log("Interface call: GSSetSamplers");

    base->GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::OMSetRenderTargets(UINT  NumViews, ID3D11RenderTargetView * const *  ppRenderTargetViews, ID3D11DepthStencilView *  pDepthStencilView)
{
    g_logger->log("Interface call: OMSetRenderTargets");

    base->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}


void myD3D11DeviceContext::OMSetRenderTargetsAndUnorderedAccessViews(UINT  NumRTVs, ID3D11RenderTargetView * const *  ppRenderTargetViews, ID3D11DepthStencilView *  pDepthStencilView, UINT  UAVStartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * const *  ppUnorderedAccessViews, const UINT *  pUAVInitialCounts)
{
    g_logger->log("Interface call: OMSetRenderTargetsAndUnorderedAccessViews");

    base->OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}


void myD3D11DeviceContext::OMSetBlendState(ID3D11BlendState *  pBlendState, const FLOAT BlendFactor[4], UINT  SampleMask)
{
    g_logger->log("Interface call: OMSetBlendState");

    base->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
}


void myD3D11DeviceContext::OMSetDepthStencilState(ID3D11DepthStencilState *  pDepthStencilState, UINT  StencilRef)
{
    g_logger->log("Interface call: OMSetDepthStencilState");

    base->OMSetDepthStencilState(pDepthStencilState, StencilRef);
}


void myD3D11DeviceContext::SOSetTargets(UINT  NumBuffers, ID3D11Buffer * const *  ppSOTargets, const UINT *  pOffsets)
{
    g_logger->log("Interface call: SOSetTargets");

    base->SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
}


void myD3D11DeviceContext::DrawAuto(void)
{
    g_logger->log("Interface call: DrawAuto");

    base->DrawAuto();
}


void myD3D11DeviceContext::DrawIndexedInstancedIndirect(ID3D11Buffer *  pBufferForArgs, UINT  AlignedByteOffsetForArgs)
{
    g_logger->log("Interface call: DrawIndexedInstancedIndirect");

    base->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}


void myD3D11DeviceContext::DrawInstancedIndirect(ID3D11Buffer *  pBufferForArgs, UINT  AlignedByteOffsetForArgs)
{
    g_logger->log("Interface call: DrawInstancedIndirect");

    base->DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}


void myD3D11DeviceContext::Dispatch(UINT  ThreadGroupCountX, UINT  ThreadGroupCountY, UINT  ThreadGroupCountZ)
{
    g_logger->log("Interface call: Dispatch");

    base->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}


void myD3D11DeviceContext::DispatchIndirect(ID3D11Buffer *  pBufferForArgs, UINT  AlignedByteOffsetForArgs)
{
    g_logger->log("Interface call: DispatchIndirect");

    base->DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}


void myD3D11DeviceContext::RSSetState(ID3D11RasterizerState *  pRasterizerState)
{
    g_logger->log("Interface call: RSSetState");

    base->RSSetState(pRasterizerState);
}


void myD3D11DeviceContext::RSSetViewports(UINT  NumViewports, const D3D11_VIEWPORT *  pViewports)
{
    g_logger->log("Interface call: RSSetViewports");

    base->RSSetViewports(NumViewports, pViewports);
}


void myD3D11DeviceContext::RSSetScissorRects(UINT  NumRects, const D3D11_RECT *  pRects)
{
    g_logger->log("Interface call: RSSetScissorRects");

    base->RSSetScissorRects(NumRects, pRects);
}


void myD3D11DeviceContext::CopySubresourceRegion(ID3D11Resource *  pDstResource, UINT  DstSubresource, UINT  DstX, UINT  DstY, UINT  DstZ, ID3D11Resource *  pSrcResource, UINT  SrcSubresource, const D3D11_BOX *  pSrcBox)
{
    g_logger->log("Interface call: CopySubresourceRegion");

    base->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
}


void myD3D11DeviceContext::CopyResource(ID3D11Resource *  pDstResource, ID3D11Resource *  pSrcResource)
{
    g_logger->log("Interface call: CopyResource");

    base->CopyResource(pDstResource, pSrcResource);
}


void myD3D11DeviceContext::UpdateSubresource(ID3D11Resource *  pDstResource, UINT  DstSubresource, const D3D11_BOX *  pDstBox, const void *  pSrcData, UINT  SrcRowPitch, UINT  SrcDepthPitch)
{
    g_logger->log("Interface call: UpdateSubresource");

    base->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}


void myD3D11DeviceContext::CopyStructureCount(ID3D11Buffer *  pDstBuffer, UINT  DstAlignedByteOffset, ID3D11UnorderedAccessView *  pSrcView)
{
    g_logger->log("Interface call: CopyStructureCount");

    base->CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
}


void myD3D11DeviceContext::ClearRenderTargetView(ID3D11RenderTargetView *  pRenderTargetView, const FLOAT ColorRGBA[4])
{
    g_logger->log("Interface call: ClearRenderTargetView");

    base->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
}


void myD3D11DeviceContext::ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView *  pUnorderedAccessView, const UINT Values[4])
{
    g_logger->log("Interface call: ClearUnorderedAccessViewUint");

    base->ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
}


void myD3D11DeviceContext::ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView *  pUnorderedAccessView, const FLOAT Values[4])
{
    g_logger->log("Interface call: ClearUnorderedAccessViewFloat");

    base->ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
}


void myD3D11DeviceContext::ClearDepthStencilView(ID3D11DepthStencilView *  pDepthStencilView, UINT  ClearFlags, FLOAT  Depth, UINT8  Stencil)
{
    g_logger->log("Interface call: ClearDepthStencilView");

    base->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
}


void myD3D11DeviceContext::GenerateMips(ID3D11ShaderResourceView *  pShaderResourceView)
{
    g_logger->log("Interface call: GenerateMips");

    base->GenerateMips(pShaderResourceView);
}


void myD3D11DeviceContext::SetResourceMinLOD(ID3D11Resource *  pResource, FLOAT  MinLOD)
{
    g_logger->log("Interface call: SetResourceMinLOD");

    base->SetResourceMinLOD(pResource, MinLOD);
}


FLOAT myD3D11DeviceContext::GetResourceMinLOD(ID3D11Resource *  pResource)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetResourceMinLOD");

    FLOAT result = base->GetResourceMinLOD(pResource);

    return result;
}


void myD3D11DeviceContext::ResolveSubresource(ID3D11Resource *  pDstResource, UINT  DstSubresource, ID3D11Resource *  pSrcResource, UINT  SrcSubresource, DXGI_FORMAT  Format)
{
    g_logger->log("Interface call: ResolveSubresource");

    base->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
}


void myD3D11DeviceContext::ExecuteCommandList(ID3D11CommandList *  pCommandList, BOOL  RestoreContextState)
{
    g_logger->log("Interface call: ExecuteCommandList");

    base->ExecuteCommandList(pCommandList, RestoreContextState);
}


void myD3D11DeviceContext::HSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    g_logger->log("Interface call: HSSetShaderResources");

    base->HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::HSSetShader(ID3D11HullShader *  pHullShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    g_logger->log("Interface call: HSSetShader");

    base->HSSetShader(pHullShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::HSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    g_logger->log("Interface call: HSSetSamplers");

    base->HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::HSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    g_logger->log("Interface call: HSSetConstantBuffers");

    base->HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::DSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    g_logger->log("Interface call: DSSetShaderResources");

    base->DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::DSSetShader(ID3D11DomainShader *  pDomainShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    g_logger->log("Interface call: DSSetShader");

    base->DSSetShader(pDomainShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::DSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    g_logger->log("Interface call: DSSetSamplers");

    base->DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::DSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    g_logger->log("Interface call: DSSetConstantBuffers");

    base->DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::CSSetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * const *  ppShaderResourceViews)
{
    g_logger->log("Interface call: CSSetShaderResources");

    base->CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::CSSetUnorderedAccessViews(UINT  StartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * const *  ppUnorderedAccessViews, const UINT *  pUAVInitialCounts)
{
    g_logger->log("Interface call: CSSetUnorderedAccessViews");

    base->CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}


void myD3D11DeviceContext::CSSetShader(ID3D11ComputeShader *  pComputeShader, ID3D11ClassInstance * const *  ppClassInstances, UINT  NumClassInstances)
{
    g_logger->log("Interface call: CSSetShader");

    base->CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
}


void myD3D11DeviceContext::CSSetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * const *  ppSamplers)
{
    g_logger->log("Interface call: CSSetSamplers");

    base->CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::CSSetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * const *  ppConstantBuffers)
{
    g_logger->log("Interface call: CSSetConstantBuffers");

    base->CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::VSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    g_logger->log("Interface call: VSGetConstantBuffers");

    base->VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::PSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    g_logger->log("Interface call: PSGetShaderResources");

    base->PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::PSGetShader(ID3D11PixelShader * *  ppPixelShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    g_logger->log("Interface call: PSGetShader");

    base->PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::PSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    g_logger->log("Interface call: PSGetSamplers");

    base->PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::VSGetShader(ID3D11VertexShader * *  ppVertexShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    g_logger->log("Interface call: VSGetShader");

    base->VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::PSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    g_logger->log("Interface call: PSGetConstantBuffers");

    base->PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::IAGetInputLayout(ID3D11InputLayout * *  ppInputLayout)
{
    g_logger->log("Interface call: IAGetInputLayout");

    base->IAGetInputLayout(ppInputLayout);
}


void myD3D11DeviceContext::IAGetVertexBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppVertexBuffers, UINT *  pStrides, UINT *  pOffsets)
{
    g_logger->log("Interface call: IAGetVertexBuffers");

    base->IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}


void myD3D11DeviceContext::IAGetIndexBuffer(ID3D11Buffer * *  pIndexBuffer, DXGI_FORMAT *  Format, UINT *  Offset)
{
    g_logger->log("Interface call: IAGetIndexBuffer");

    base->IAGetIndexBuffer(pIndexBuffer, Format, Offset);
}


void myD3D11DeviceContext::GSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    g_logger->log("Interface call: GSGetConstantBuffers");

    base->GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::GSGetShader(ID3D11GeometryShader * *  ppGeometryShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    g_logger->log("Interface call: GSGetShader");

    base->GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::IAGetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY *  pTopology)
{
    g_logger->log("Interface call: IAGetPrimitiveTopology");

    base->IAGetPrimitiveTopology(pTopology);
}


void myD3D11DeviceContext::VSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    g_logger->log("Interface call: VSGetShaderResources");

    base->VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::VSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    g_logger->log("Interface call: VSGetSamplers");

    base->VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::GetPredication(ID3D11Predicate * *  ppPredicate, BOOL *  pPredicateValue)
{
    g_logger->log("Interface call: GetPredication");

    base->GetPredication(ppPredicate, pPredicateValue);
}


void myD3D11DeviceContext::GSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    g_logger->log("Interface call: GSGetShaderResources");

    base->GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::GSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    g_logger->log("Interface call: GSGetSamplers");

    base->GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::OMGetRenderTargets(UINT  NumViews, ID3D11RenderTargetView * *  ppRenderTargetViews, ID3D11DepthStencilView * *  ppDepthStencilView)
{
    g_logger->log("Interface call: OMGetRenderTargets");

    base->OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
}


void myD3D11DeviceContext::OMGetRenderTargetsAndUnorderedAccessViews(UINT  NumRTVs, ID3D11RenderTargetView * *  ppRenderTargetViews, ID3D11DepthStencilView * *  ppDepthStencilView, UINT  UAVStartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * *  ppUnorderedAccessViews)
{
    g_logger->log("Interface call: OMGetRenderTargetsAndUnorderedAccessViews");

    base->OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
}


void myD3D11DeviceContext::OMGetBlendState(ID3D11BlendState * *  ppBlendState, FLOAT BlendFactor[4], UINT *  pSampleMask)
{
    g_logger->log("Interface call: OMGetBlendState");

    base->OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
}


void myD3D11DeviceContext::OMGetDepthStencilState(ID3D11DepthStencilState * *  ppDepthStencilState, UINT *  pStencilRef)
{
    g_logger->log("Interface call: OMGetDepthStencilState");

    base->OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
}


void myD3D11DeviceContext::SOGetTargets(UINT  NumBuffers, ID3D11Buffer * *  ppSOTargets)
{
    g_logger->log("Interface call: SOGetTargets");

    base->SOGetTargets(NumBuffers, ppSOTargets);
}


void myD3D11DeviceContext::RSGetState(ID3D11RasterizerState * *  ppRasterizerState)
{
    g_logger->log("Interface call: RSGetState");

    base->RSGetState(ppRasterizerState);
}


void myD3D11DeviceContext::RSGetViewports(UINT *  pNumViewports, D3D11_VIEWPORT *  pViewports)
{
    g_logger->log("Interface call: RSGetViewports");

    base->RSGetViewports(pNumViewports, pViewports);
}


void myD3D11DeviceContext::RSGetScissorRects(UINT *  pNumRects, D3D11_RECT *  pRects)
{
    g_logger->log("Interface call: RSGetScissorRects");

    base->RSGetScissorRects(pNumRects, pRects);
}


void myD3D11DeviceContext::HSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    g_logger->log("Interface call: HSGetShaderResources");

    base->HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::HSGetShader(ID3D11HullShader * *  ppHullShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    g_logger->log("Interface call: HSGetShader");

    base->HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::HSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    g_logger->log("Interface call: HSGetSamplers");

    base->HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::HSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    g_logger->log("Interface call: HSGetConstantBuffers");

    base->HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::DSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    g_logger->log("Interface call: DSGetShaderResources");

    base->DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::DSGetShader(ID3D11DomainShader * *  ppDomainShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    g_logger->log("Interface call: DSGetShader");

    base->DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::DSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    g_logger->log("Interface call: DSGetSamplers");

    base->DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::DSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    g_logger->log("Interface call: DSGetConstantBuffers");

    base->DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::CSGetShaderResources(UINT  StartSlot, UINT  NumViews, ID3D11ShaderResourceView * *  ppShaderResourceViews)
{
    g_logger->log("Interface call: CSGetShaderResources");

    base->CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}


void myD3D11DeviceContext::CSGetUnorderedAccessViews(UINT  StartSlot, UINT  NumUAVs, ID3D11UnorderedAccessView * *  ppUnorderedAccessViews)
{
    g_logger->log("Interface call: CSGetUnorderedAccessViews");

    base->CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
}


void myD3D11DeviceContext::CSGetShader(ID3D11ComputeShader * *  ppComputeShader, ID3D11ClassInstance * *  ppClassInstances, UINT *  pNumClassInstances)
{
    g_logger->log("Interface call: CSGetShader");

    base->CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
}


void myD3D11DeviceContext::CSGetSamplers(UINT  StartSlot, UINT  NumSamplers, ID3D11SamplerState * *  ppSamplers)
{
    g_logger->log("Interface call: CSGetSamplers");

    base->CSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}


void myD3D11DeviceContext::CSGetConstantBuffers(UINT  StartSlot, UINT  NumBuffers, ID3D11Buffer * *  ppConstantBuffers)
{
    g_logger->log("Interface call: CSGetConstantBuffers");

    base->CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}


void myD3D11DeviceContext::ClearState(void)
{
    g_logger->log("Interface call: ClearState");

    base->ClearState();
}


void myD3D11DeviceContext::Flush(void)
{
    g_logger->log("Interface call: Flush");

    base->Flush();
}


D3D11_DEVICE_CONTEXT_TYPE myD3D11DeviceContext::GetType(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetType");

    D3D11_DEVICE_CONTEXT_TYPE result = base->GetType();

    return result;
}


UINT myD3D11DeviceContext::GetContextFlags(void)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetContextFlags");

    UINT result = base->GetContextFlags();

    return result;
}


HRESULT myD3D11DeviceContext::FinishCommandList(BOOL  RestoreDeferredContextState, ID3D11CommandList * *  ppCommandList)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: FinishCommandList");

    HRESULT result = base->FinishCommandList(RestoreDeferredContextState, ppCommandList);

    return result;
}


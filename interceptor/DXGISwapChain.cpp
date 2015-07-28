
#include "main.h"


HRESULT myDXGISwapChain::SetPrivateData(REFGUID  Name, UINT  DataSize, const void *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: SetPrivateData");

    HRESULT result = base->SetPrivateData(Name, DataSize, pData);

    return result;
}


HRESULT myDXGISwapChain::SetPrivateDataInterface(REFGUID  Name, const IUnknown *  pUnknown)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: SetPrivateDataInterface");

    HRESULT result = base->SetPrivateDataInterface(Name, pUnknown);

    return result;
}


HRESULT myDXGISwapChain::GetPrivateData(REFGUID  Name, _Inout_ UINT *  pDataSize, void *  pData)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetPrivateData");

    HRESULT result = base->GetPrivateData(Name, pDataSize, pData);

    return result;
}


HRESULT myDXGISwapChain::GetParent(REFIID  riid, void * *  ppParent)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetParent");

    HRESULT result = base->GetParent(riid, ppParent);

    return result;
}


HRESULT myDXGISwapChain::GetDevice(REFIID  riid, void * *  ppDevice)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetDevice");

    HRESULT result = base->GetDevice(riid, ppDevice);

    return result;
}


HRESULT myDXGISwapChain::Present(UINT  SyncInterval, UINT  Flags)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: Present");
    if (g_logger->logDrawCalls) g_logger->logDrawFile << "Present Flags=" << Flags << endl;

    if (g_logger->capturingFrame)
    {
        Bitmap image;
        assets->context->readSwapChain(image);
        LodePNG::save(image, g_logger->captureDir + "final.png");

        g_logger->endFrameCapture();
    }

    if (g_logger->newSignaturesThisFrame > 0)
    {
        g_logger->logSignatureFile << "Frame " << g_logger->frameIndex << " has " << g_logger->newSignaturesThisFrame << " new signatures" << endl;
        g_logger->colorMap.save(g_logger->colorMapFilename());
    }

    if (g_logger->curFrame->objects.size() > 0)
    {
        g_logger->logSignatureFile << "Frame " << g_logger->frameIndex << " has " << g_logger->curFrame->objects.size() << " objects" << endl;
        g_logger->allFrames.frames.push_back(g_logger->curFrame);

        if (g_logger->frameIndex == frameDumpIndex)
            g_logger->allFrames.save(g_logger->logDir + "allFrames.dat");

        g_logger->curFrame = new FrameObjectData();
    }

    HRESULT result = base->Present(SyncInterval, Flags);
    g_logger->frameIndex++;
    g_logger->frameRenderIndex = 0;
    g_logger->newSignaturesThisFrame = 0;

    g_state->AI->newFrameStart(g_logger->frameIndex);

    //if (GetAsyncKeyState(VK_F8) || g_logger->frameIndex == 20 || g_logger->frameIndex == 21)
    if (GetAsyncKeyState(VK_F8))
    {
        g_logger->beginFrameCapture();
    }

    return result;
}


HRESULT myDXGISwapChain::GetBuffer(UINT  Buffer, REFIID  riid, void * *  ppSurface)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetBuffer");

    HRESULT result = base->GetBuffer(Buffer, riid, ppSurface);

    return result;
}


HRESULT myDXGISwapChain::SetFullscreenState(BOOL  Fullscreen, IDXGIOutput *  pTarget)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: SetFullscreenState");

    HRESULT result = base->SetFullscreenState(Fullscreen, pTarget);

    return result;
}


HRESULT myDXGISwapChain::GetFullscreenState(BOOL *  pFullscreen, IDXGIOutput * *  ppTarget)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetFullscreenState");

    HRESULT result = base->GetFullscreenState(pFullscreen, ppTarget);

    return result;
}


HRESULT myDXGISwapChain::GetDesc(DXGI_SWAP_CHAIN_DESC *  pDesc)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetDesc");

    HRESULT result = base->GetDesc(pDesc);

    return result;
}


HRESULT myDXGISwapChain::ResizeBuffers(UINT  BufferCount, UINT  Width, UINT  Height, DXGI_FORMAT  NewFormat, UINT  SwapChainFlags)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: ResizeBuffers");

    HRESULT result = base->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);

    return result;
}


HRESULT myDXGISwapChain::ResizeTarget(const DXGI_MODE_DESC *  pNewTargetParameters)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: ResizeTarget");

    HRESULT result = base->ResizeTarget(pNewTargetParameters);

    return result;
}


HRESULT myDXGISwapChain::GetContainingOutput(IDXGIOutput * *  ppOutput)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetContainingOutput");

    HRESULT result = base->GetContainingOutput(ppOutput);

    return result;
}


HRESULT myDXGISwapChain::GetFrameStatistics(DXGI_FRAME_STATISTICS *  pStats)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetFrameStatistics");

    HRESULT result = base->GetFrameStatistics(pStats);

    return result;
}


HRESULT myDXGISwapChain::GetLastPresentCount(UINT *  pLastPresentCount)
{
    if (g_logger->logInterfaceCalls) g_logger->log("Interface call: GetLastPresentCount");

    HRESULT result = base->GetLastPresentCount(pLastPresentCount);

    return result;
}



#include "main.h"


typedef HRESULT(WINAPI *D3DKMTDestroyAllocationType)();
HRESULT WINAPI myD3DKMTDestroyAllocation()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTDestroyAllocation");

    D3DKMTDestroyAllocationType proc = (D3DKMTDestroyAllocationType)g_state->getProcedure("D3DKMTDestroyAllocation");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTDestroyContextType)();
HRESULT WINAPI myD3DKMTDestroyContext()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTDestroyContext");

    D3DKMTDestroyContextType proc = (D3DKMTDestroyContextType)g_state->getProcedure("D3DKMTDestroyContext");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTDestroyDeviceType)();
HRESULT WINAPI myD3DKMTDestroyDevice()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTDestroyDevice");

    D3DKMTDestroyDeviceType proc = (D3DKMTDestroyDeviceType)g_state->getProcedure("D3DKMTDestroyDevice");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTDestroySynchronizationObjectType)();
HRESULT WINAPI myD3DKMTDestroySynchronizationObject()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTDestroySynchronizationObject");

    D3DKMTDestroySynchronizationObjectType proc = (D3DKMTDestroySynchronizationObjectType)g_state->getProcedure("D3DKMTDestroySynchronizationObject");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTQueryAdapterInfoType)();
HRESULT WINAPI myD3DKMTQueryAdapterInfo()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTQueryAdapterInfo");

    D3DKMTQueryAdapterInfoType proc = (D3DKMTQueryAdapterInfoType)g_state->getProcedure("D3DKMTQueryAdapterInfo");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTSetDisplayPrivateDriverFormatType)();
HRESULT WINAPI myD3DKMTSetDisplayPrivateDriverFormat()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTSetDisplayPrivateDriverFormat");

    D3DKMTSetDisplayPrivateDriverFormatType proc = (D3DKMTSetDisplayPrivateDriverFormatType)g_state->getProcedure("D3DKMTSetDisplayPrivateDriverFormat");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTSignalSynchronizationObjectType)();
HRESULT WINAPI myD3DKMTSignalSynchronizationObject()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTSignalSynchronizationObject");

    D3DKMTSignalSynchronizationObjectType proc = (D3DKMTSignalSynchronizationObjectType)g_state->getProcedure("D3DKMTSignalSynchronizationObject");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTUnlockType)();
HRESULT WINAPI myD3DKMTUnlock()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTUnlock");

    D3DKMTUnlockType proc = (D3DKMTUnlockType)g_state->getProcedure("D3DKMTUnlock");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTWaitForSynchronizationObjectType)();
HRESULT WINAPI myD3DKMTWaitForSynchronizationObject()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTWaitForSynchronizationObject");

    D3DKMTWaitForSynchronizationObjectType proc = (D3DKMTWaitForSynchronizationObjectType)g_state->getProcedure("D3DKMTWaitForSynchronizationObject");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *OpenAdapter10Type)();
HRESULT WINAPI myOpenAdapter10()
{
    initGlobals();
    g_logger->log("DLL call: OpenAdapter10");

    OpenAdapter10Type proc = (OpenAdapter10Type)g_state->getProcedure("OpenAdapter10");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *OpenAdapter10_2Type)();
HRESULT WINAPI myOpenAdapter10_2()
{
    initGlobals();
    g_logger->log("DLL call: OpenAdapter10_2");

    OpenAdapter10_2Type proc = (OpenAdapter10_2Type)g_state->getProcedure("OpenAdapter10_2");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3D11CoreCreateDeviceType)();
HRESULT WINAPI myD3D11CoreCreateDevice()
{
    initGlobals();
    g_logger->log("DLL call: D3D11CoreCreateDevice");

    D3D11CoreCreateDeviceType proc = (D3D11CoreCreateDeviceType)g_state->getProcedure("D3D11CoreCreateDevice");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3D11CoreCreateLayeredDeviceType)();
HRESULT WINAPI myD3D11CoreCreateLayeredDevice()
{
    initGlobals();
    g_logger->log("DLL call: D3D11CoreCreateLayeredDevice");

    D3D11CoreCreateLayeredDeviceType proc = (D3D11CoreCreateLayeredDeviceType)g_state->getProcedure("D3D11CoreCreateLayeredDevice");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3D11CoreGetLayeredDeviceSizeType)();
HRESULT WINAPI myD3D11CoreGetLayeredDeviceSize()
{
    initGlobals();
    g_logger->log("DLL call: D3D11CoreGetLayeredDeviceSize");

    D3D11CoreGetLayeredDeviceSizeType proc = (D3D11CoreGetLayeredDeviceSizeType)g_state->getProcedure("D3D11CoreGetLayeredDeviceSize");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3D11CoreRegisterLayersType)();
HRESULT WINAPI myD3D11CoreRegisterLayers()
{
    initGlobals();
    g_logger->log("DLL call: D3D11CoreRegisterLayers");

    D3D11CoreRegisterLayersType proc = (D3D11CoreRegisterLayersType)g_state->getProcedure("D3D11CoreRegisterLayers");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3D11CreateDeviceType)(IDXGIAdapter* p0, D3D_DRIVER_TYPE p1, HMODULE p2, UINT p3, const D3D_FEATURE_LEVEL* p4, UINT p5, UINT p6, ID3D11Device** p7, D3D_FEATURE_LEVEL* p8, ID3D11DeviceContext ** p9);
HRESULT WINAPI myD3D11CreateDevice(IDXGIAdapter* p0, D3D_DRIVER_TYPE p1, HMODULE p2, UINT p3, const D3D_FEATURE_LEVEL* p4, UINT p5, UINT p6, ID3D11Device** p7, D3D_FEATURE_LEVEL* p8, ID3D11DeviceContext ** p9)
{
    initGlobals();
    g_logger->log("DLL call: D3D11CreateDevice");

    D3D11CreateDeviceType proc = (D3D11CreateDeviceType)g_state->getProcedure("D3D11CreateDevice");

    HRESULT result = proc(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);

    return result;
}


typedef HRESULT(WINAPI *D3D11CreateDeviceAndSwapChainType)(IDXGIAdapter* p0, D3D_DRIVER_TYPE p1, HMODULE p2, UINT p3, const D3D_FEATURE_LEVEL* p4, UINT p5, UINT p6, const DXGI_SWAP_CHAIN_DESC * p7, IDXGISwapChain ** p8, ID3D11Device** p9, D3D_FEATURE_LEVEL* p10, ID3D11DeviceContext ** p11);
HRESULT WINAPI myD3D11CreateDeviceAndSwapChain(IDXGIAdapter* p0, D3D_DRIVER_TYPE p1, HMODULE p2, UINT p3, const D3D_FEATURE_LEVEL* p4, UINT p5, UINT p6, const DXGI_SWAP_CHAIN_DESC * p7, IDXGISwapChain ** p8, ID3D11Device** p9, D3D_FEATURE_LEVEL* p10, ID3D11DeviceContext ** p11)
{
    initGlobals();
    g_logger->log("DLL call: D3D11CreateDeviceAndSwapChain");

    D3D11CreateDeviceAndSwapChainType proc = (D3D11CreateDeviceAndSwapChainType)g_state->getProcedure("D3D11CreateDeviceAndSwapChain");

    HRESULT result = proc(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);

    if (result != S_OK)
        return result;

    myDXGISwapChain *mySwapChain = new myDXGISwapChain(*p8);
    myD3D11Device *myDevice = new myD3D11Device(*p9);
    myD3D11DeviceContext *myContext = new myD3D11DeviceContext(*p11);
    *p8 = mySwapChain;
    *p9 = myDevice;
    *p11 = myContext;

    return S_OK;
}


typedef HRESULT(WINAPI *D3DKMTCreateAllocationType)();
HRESULT WINAPI myD3DKMTCreateAllocation()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTCreateAllocation");

    D3DKMTCreateAllocationType proc = (D3DKMTCreateAllocationType)g_state->getProcedure("D3DKMTCreateAllocation");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTCreateContextType)();
HRESULT WINAPI myD3DKMTCreateContext()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTCreateContext");

    D3DKMTCreateContextType proc = (D3DKMTCreateContextType)g_state->getProcedure("D3DKMTCreateContext");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTCreateDeviceType)();
HRESULT WINAPI myD3DKMTCreateDevice()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTCreateDevice");

    D3DKMTCreateDeviceType proc = (D3DKMTCreateDeviceType)g_state->getProcedure("D3DKMTCreateDevice");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTCreateSynchronizationObjectType)();
HRESULT WINAPI myD3DKMTCreateSynchronizationObject()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTCreateSynchronizationObject");

    D3DKMTCreateSynchronizationObjectType proc = (D3DKMTCreateSynchronizationObjectType)g_state->getProcedure("D3DKMTCreateSynchronizationObject");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTEscapeType)();
HRESULT WINAPI myD3DKMTEscape()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTEscape");

    D3DKMTEscapeType proc = (D3DKMTEscapeType)g_state->getProcedure("D3DKMTEscape");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTGetContextSchedulingPriorityType)();
HRESULT WINAPI myD3DKMTGetContextSchedulingPriority()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTGetContextSchedulingPriority");

    D3DKMTGetContextSchedulingPriorityType proc = (D3DKMTGetContextSchedulingPriorityType)g_state->getProcedure("D3DKMTGetContextSchedulingPriority");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTGetDeviceStateType)();
HRESULT WINAPI myD3DKMTGetDeviceState()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTGetDeviceState");

    D3DKMTGetDeviceStateType proc = (D3DKMTGetDeviceStateType)g_state->getProcedure("D3DKMTGetDeviceState");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTGetDisplayModeListType)();
HRESULT WINAPI myD3DKMTGetDisplayModeList()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTGetDisplayModeList");

    D3DKMTGetDisplayModeListType proc = (D3DKMTGetDisplayModeListType)g_state->getProcedure("D3DKMTGetDisplayModeList");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTGetMultisampleMethodListType)();
HRESULT WINAPI myD3DKMTGetMultisampleMethodList()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTGetMultisampleMethodList");

    D3DKMTGetMultisampleMethodListType proc = (D3DKMTGetMultisampleMethodListType)g_state->getProcedure("D3DKMTGetMultisampleMethodList");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTGetRuntimeDataType)();
HRESULT WINAPI myD3DKMTGetRuntimeData()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTGetRuntimeData");

    D3DKMTGetRuntimeDataType proc = (D3DKMTGetRuntimeDataType)g_state->getProcedure("D3DKMTGetRuntimeData");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTGetSharedPrimaryHandleType)();
HRESULT WINAPI myD3DKMTGetSharedPrimaryHandle()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTGetSharedPrimaryHandle");

    D3DKMTGetSharedPrimaryHandleType proc = (D3DKMTGetSharedPrimaryHandleType)g_state->getProcedure("D3DKMTGetSharedPrimaryHandle");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTLockType)();
HRESULT WINAPI myD3DKMTLock()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTLock");

    D3DKMTLockType proc = (D3DKMTLockType)g_state->getProcedure("D3DKMTLock");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTOpenAdapterFromHdcType)();
HRESULT WINAPI myD3DKMTOpenAdapterFromHdc()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTOpenAdapterFromHdc");

    D3DKMTOpenAdapterFromHdcType proc = (D3DKMTOpenAdapterFromHdcType)g_state->getProcedure("D3DKMTOpenAdapterFromHdc");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTOpenResourceType)();
HRESULT WINAPI myD3DKMTOpenResource()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTOpenResource");

    D3DKMTOpenResourceType proc = (D3DKMTOpenResourceType)g_state->getProcedure("D3DKMTOpenResource");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTPresentType)();
HRESULT WINAPI myD3DKMTPresent()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTPresent");

    D3DKMTPresentType proc = (D3DKMTPresentType)g_state->getProcedure("D3DKMTPresent");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTQueryAllocationResidencyType)();
HRESULT WINAPI myD3DKMTQueryAllocationResidency()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTQueryAllocationResidency");

    D3DKMTQueryAllocationResidencyType proc = (D3DKMTQueryAllocationResidencyType)g_state->getProcedure("D3DKMTQueryAllocationResidency");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTQueryResourceInfoType)();
HRESULT WINAPI myD3DKMTQueryResourceInfo()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTQueryResourceInfo");

    D3DKMTQueryResourceInfoType proc = (D3DKMTQueryResourceInfoType)g_state->getProcedure("D3DKMTQueryResourceInfo");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTRenderType)();
HRESULT WINAPI myD3DKMTRender()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTRender");

    D3DKMTRenderType proc = (D3DKMTRenderType)g_state->getProcedure("D3DKMTRender");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTSetAllocationPriorityType)();
HRESULT WINAPI myD3DKMTSetAllocationPriority()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTSetAllocationPriority");

    D3DKMTSetAllocationPriorityType proc = (D3DKMTSetAllocationPriorityType)g_state->getProcedure("D3DKMTSetAllocationPriority");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTSetContextSchedulingPriorityType)();
HRESULT WINAPI myD3DKMTSetContextSchedulingPriority()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTSetContextSchedulingPriority");

    D3DKMTSetContextSchedulingPriorityType proc = (D3DKMTSetContextSchedulingPriorityType)g_state->getProcedure("D3DKMTSetContextSchedulingPriority");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTSetDisplayModeType)();
HRESULT WINAPI myD3DKMTSetDisplayMode()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTSetDisplayMode");

    D3DKMTSetDisplayModeType proc = (D3DKMTSetDisplayModeType)g_state->getProcedure("D3DKMTSetDisplayMode");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTSetGammaRampType)();
HRESULT WINAPI myD3DKMTSetGammaRamp()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTSetGammaRamp");

    D3DKMTSetGammaRampType proc = (D3DKMTSetGammaRampType)g_state->getProcedure("D3DKMTSetGammaRamp");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTSetVidPnSourceOwnerType)();
HRESULT WINAPI myD3DKMTSetVidPnSourceOwner()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTSetVidPnSourceOwner");

    D3DKMTSetVidPnSourceOwnerType proc = (D3DKMTSetVidPnSourceOwnerType)g_state->getProcedure("D3DKMTSetVidPnSourceOwner");

    HRESULT result = proc();

    return result;
}


typedef HRESULT(WINAPI *D3DKMTWaitForVerticalBlankEventType)();
HRESULT WINAPI myD3DKMTWaitForVerticalBlankEvent()
{
    initGlobals();
    g_logger->log("DLL call: D3DKMTWaitForVerticalBlankEvent");

    D3DKMTWaitForVerticalBlankEventType proc = (D3DKMTWaitForVerticalBlankEventType)g_state->getProcedure("D3DKMTWaitForVerticalBlankEvent");

    HRESULT result = proc();

    return result;
}


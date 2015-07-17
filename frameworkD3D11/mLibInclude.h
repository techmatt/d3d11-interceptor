
//
// mLib config options
//

#define MLIB_ERROR_CHECK
#define MLIB_BOUNDS_CHECK

//
// mLib includes
//

#include "mLibCore.h"

#define USE_INTERCEPTOR_D3D11DLL

#ifdef USE_INTERCEPTOR_D3D11DLL

#pragma warning ( disable : 4005)
#include <d3d11.h>
__declspec(dllimport) HRESULT WINAPI myD3D11CreateDeviceAndSwapChain(IDXGIAdapter* p0, D3D_DRIVER_TYPE p1, HMODULE p2, UINT p3, const D3D_FEATURE_LEVEL* p4, UINT p5, UINT p6, const DXGI_SWAP_CHAIN_DESC * p7, IDXGISwapChain ** p8, ID3D11Device** p9, D3D_FEATURE_LEVEL* p10, ID3D11DeviceContext ** p11);
__declspec(dllimport) HRESULT WINAPI myD3D11CreateDevice(IDXGIAdapter* p0, D3D_DRIVER_TYPE p1, HMODULE p2, UINT p3, const D3D_FEATURE_LEVEL* p4, UINT p5, UINT p6, ID3D11Device** p7, D3D_FEATURE_LEVEL* p8, ID3D11DeviceContext ** p9);

#endif

#include "mLibD3D11.h"
#include "mLibD3D11Font.h"

using namespace ml;
using namespace std;

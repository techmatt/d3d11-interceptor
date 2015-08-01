
#pragma once

#include "mLibInclude.h"

#ifdef USE_INTERCEPTOR_D3D11DLL

#pragma warning ( disable : 4005)
#include <d3d11.h>
__declspec(dllimport) HRESULT WINAPI myD3D11CreateDeviceAndSwapChain(IDXGIAdapter* p0, D3D_DRIVER_TYPE p1, HMODULE p2, UINT p3, const D3D_FEATURE_LEVEL* p4, UINT p5, UINT p6, const DXGI_SWAP_CHAIN_DESC * p7, IDXGISwapChain ** p8, ID3D11Device** p9, D3D_FEATURE_LEVEL* p10, ID3D11DeviceContext ** p11);
__declspec(dllimport) HRESULT WINAPI myD3D11CreateDevice(IDXGIAdapter* p0, D3D_DRIVER_TYPE p1, HMODULE p2, UINT p3, const D3D_FEATURE_LEVEL* p4, UINT p5, UINT p6, ID3D11Device** p7, D3D_FEATURE_LEVEL* p8, ID3D11DeviceContext ** p9);

#endif

using namespace ml;
using namespace std;

typedef ColorImageR8G8B8A8 Bitmap;

#include "learningParams.h"

#include "../shared/localizedObject.h"
#include "../shared/objectCollection.h"
#include "../shared/signatureColorMap.h"
#include "../shared/geometryDatabase.h"

#include "frameProcessing.h"
#include "character.h"
#include "segmentAnalyzer.h"

#include "appState.h"
#include "datasetProcessor.h"
#include "vizzer.h"

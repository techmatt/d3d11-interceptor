
#include "mLibCore.h"

#include "mLibLodePNG.h"

using namespace std;
using namespace ml;

typedef ColorImageR8G8B8A8 Bitmap;

#include <windows.h>
#include <d3d11.h>

#include "globals.h"

class myIUnknown
{
protected:
    IUnknown*   m_pUnk;
    ULONG       m_ulRef;

public:
    myIUnknown(IUnknown* pUnk)
    {
        m_pUnk = pUnk;
        m_ulRef = 1;
    }

    ULONG AddRef()
    {
        m_pUnk->AddRef();
        return ++m_ulRef;
    }

    ULONG Release()
    {
        m_pUnk->Release();

        ULONG ulRef = --m_ulRef;
        if (0 == ulRef)
        {
            //delete this;
            return 0;
        }
        return ulRef;
    }
};

class myDXGISwapChain;
class myD3D11Device;
class myD3D11DeviceContext;

struct MyD3DAssets
{
    myDXGISwapChain *swapChain;
    myD3D11Device *device;
    myD3D11DeviceContext *context;
};

#include "d3d11Device.h"
#include "d3d11DeviceContext.h"
#include "DXGISwapChain.h"
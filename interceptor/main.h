
#include <string>
#include <fstream>
using namespace std;

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

#include "d3d11Device.h"
#include "d3d11DeviceContext.h"
#include "DXGISwapChain.h"
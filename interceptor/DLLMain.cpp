
#include "main.h"

BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle,
    IN DWORD     nReason,
    IN LPVOID    Reserved)
{
    BOOLEAN bSuccess = TRUE;

    //  Perform global initialization.

    switch (nReason)
    {
    case DLL_PROCESS_ATTACH:

        break;

    case DLL_PROCESS_DETACH:

        break;
    }

    return bSuccess;

}

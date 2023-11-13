#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "SWR.h"

DWORD WINAPI ModThread(LPVOID hModule)
{
    SWRGame swr;

    FreeLibraryAndExitThread((HMODULE)hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        if (MessageBox(NULL, L"Limit racers to a random guy?", L"Limit?", MB_YESNO) == IDYES)
        {
            DWORD threadID = 0;
            CreateThread(NULL, 0, ModThread, hModule, 0, &threadID);
        }
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "SWR.h"
#include "resource.h"

#include "mini/ini.h"

DWORD WINAPI ModThread(LPVOID hModule)
{
    SWRGame swr;

    while (true)
    {
        if (GetAsyncKeyState(VK_NUMPAD0) & 1)
        {
            swr.KillPod();
        }
    }

    FreeLibraryAndExitThread((HMODULE)hModule, 0);
    return 0;
}

INT_PTR WINAPI APLoginDialog(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    mINI::INIFile file("SWR_AP_Client.ini");
    mINI::INIStructure ini;

    switch (uMsg) {
    case WM_INITDIALOG:
        // Load config from file
        if (file.read(ini))
        {
            std::string serverStr = ini.get("Archipelago").get("Server");
            std::string playerStr = ini.get("Archipelago").get("Player");
            std::string pwStr = ini.get("Archipelago").get("Password");

            SetDlgItemTextA(hwnd, IDC_SERVER_BOX, serverStr.c_str());
            SetDlgItemTextA(hwnd, IDC_PLAYER_BOX, playerStr.c_str());
            SetDlgItemTextA(hwnd, IDC_PASSWORD_BOX, pwStr.c_str());
        }
        return TRUE;

    case WM_COMMAND:
        switch (wParam) {
        case IDC_LOGIN:
            // Store config
            APServerInfo apInfo;

            GetDlgItemTextA(hwnd, IDC_SERVER_BOX, apInfo.server, 64);
            GetDlgItemTextA(hwnd, IDC_PLAYER_BOX, apInfo.player, 64);
            GetDlgItemTextA(hwnd, IDC_PASSWORD_BOX, apInfo.pw, 64);

            ini["Archipelago"]["Server"] = apInfo.server;
            ini["Archipelago"]["Player"] = apInfo.player;
            ini["Archipelago"]["Password"] = apInfo.pw;
            file.write(ini);

            EndDialog(hwnd, 1);
            return TRUE;
        case IDC_NO_AP:
            EndDialog(hwnd, 2);
            return FALSE;
        }
        break;
    }

    return FALSE;
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
        if (DialogBox(hModule, MAKEINTRESOURCE(IDD_FORMVIEW), NULL, APLoginDialog) == 1)
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


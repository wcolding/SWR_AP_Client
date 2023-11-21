#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "SWR.h"
#include "resource.h"

#include "mini/ini.h"

struct ThreadParams
{
    APServerInfo apInfo;
    bool debugConsole = false;
};

ThreadParams threadParams;

DWORD WINAPI ModThread(LPVOID hModule)
{
    if (threadParams.debugConsole)
    {
        FILE* pFile = nullptr;
        AllocConsole();
        freopen_s(&pFile, "CONOUT$", "w", stdout);
    }

    SWRGame::Init();

    while (true)
    {
        if (GetAsyncKeyState(VK_NUMPAD0) & 1)
        {
            SWRGame::QueueDeath();
        }

        SWRGame::Update();
    }

    if (threadParams.debugConsole)
        FreeConsole();

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
            std::string consoleStr = ini.get("Client").get("UseDebugConsole");
            if (!consoleStr.empty())
            {
                if ((consoleStr.compare("True") == 0) || (consoleStr.compare("true") == 0) || (consoleStr.compare("TRUE") == 0))
                    threadParams.debugConsole = true;
            }

            SetDlgItemTextA(hwnd, IDC_SERVER_BOX, serverStr.c_str());
            SetDlgItemTextA(hwnd, IDC_PLAYER_BOX, playerStr.c_str());
            SetDlgItemTextA(hwnd, IDC_PASSWORD_BOX, pwStr.c_str());
        }
        return TRUE;

    case WM_COMMAND:
        switch (wParam) {
        case IDC_LOGIN:
            // Store config

            GetDlgItemTextA(hwnd, IDC_SERVER_BOX, threadParams.apInfo.server, 64);
            GetDlgItemTextA(hwnd, IDC_PLAYER_BOX, threadParams.apInfo.player, 64);
            GetDlgItemTextA(hwnd, IDC_PASSWORD_BOX, threadParams.apInfo.pw, 64);

            ini["Archipelago"]["Server"] = threadParams.apInfo.server;
            ini["Archipelago"]["Player"] = threadParams.apInfo.player;
            ini["Archipelago"]["Password"] = threadParams.apInfo.pw;
            ini["Client"]["UseDebugConsole"] = threadParams.debugConsole ? "true" : "false";
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


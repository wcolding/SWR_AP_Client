#include "dllmain.h"

using SWRGame::saveData;
using SWRGame::serverInfo;

bool debugConsole = false;

DWORD WINAPI ModThread(LPVOID hModule)
{
    if (debugConsole)
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

    if (debugConsole)
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
                    debugConsole = true;
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

            GetDlgItemTextA(hwnd, IDC_SERVER_BOX, SWRGame::serverInfo.server, 64);
            GetDlgItemTextA(hwnd, IDC_PLAYER_BOX, SWRGame::serverInfo.player, 64);
            GetDlgItemTextA(hwnd, IDC_PASSWORD_BOX, SWRGame::serverInfo.pw, 64);

            ini["Archipelago"]["Server"] = SWRGame::serverInfo.server;
            ini["Archipelago"]["Player"] = SWRGame::serverInfo.player;
            ini["Archipelago"]["Password"] = SWRGame::serverInfo.pw;
            ini["Client"]["UseDebugConsole"] = debugConsole ? "true" : "false";
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


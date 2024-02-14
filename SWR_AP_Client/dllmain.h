#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "mini/ini.h"

#include "resource.h"
#include "Structs.h"

namespace SWRGame
{
    extern AP_ProgressData progress;
    extern AP_ServerInfo serverInfo;
    extern SWRGameState gamestate;
    extern void Init();
    extern void Update();
    extern void StartupSequenceLoop();
}
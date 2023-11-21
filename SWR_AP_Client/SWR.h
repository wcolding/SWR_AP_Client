#pragma once

#include "Enums.h"
#include "Structs.h"
#include "APInfo.h"
#include "Patches.h"

#define POD_PARTS_OFFSET 0xA35AA1
#define POD_DATA_PTR_OFFSET 0xA29C44
#define SAVE_DATA_OFFSET 0xA35A60
#define RACE_DATA_OFFSET 0xA29C20

namespace SWRGame
{
	void Log(const char* format, ...);
	void ScanLocationChecks();
	void ProcessDeathQueue();
	void KillPod();

	void Init();
	void Update();

	bool isSaveFileLoaded();
	bool isPlayerInRace();
	bool isPlayerKillable();

	void QueueDeath();
}


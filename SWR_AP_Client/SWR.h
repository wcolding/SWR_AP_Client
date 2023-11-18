#pragma once

#include "Enums.h"
#include "Structs.h"
#include "Archipelago.h"
#include "Patches.h"

#define POD_PARTS_OFFSET 0xA35AA1
#define POD_DATA_PTR_OFFSET 0xA29C44
#define SAVE_DATA_OFFSET 0xA35A60
#define RACE_DATA_OFFSET 0xA29C20

class SWRGame
{
private:
	int baseAddress;
	int queuedDeaths;
	RacePlacement requiredPlacement;

	void Log(const char* format, ...);
	void ScanLocationChecks();
	void ProcessDeathQueue();
	void KillPod();
public:
	SWRGame();
	void Update();

	bool isSaveFileLoaded();
	bool isPlayerInRace();
	bool isPlayerKillable();

	void QueueDeath();
};


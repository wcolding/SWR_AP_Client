#pragma once

#include "Enums.h"
#include "Structs.h"
#include "Patches.h"

#define POD_PARTS_OFFSET 0xA35AA1
#define POD_DATA_PTR_OFFSET 0xA29C44
#define SAVE_DATA_OFFSET 0xA35A60
#define SAVE_DATA_PTR_OFFSET 0xA29BCC
#define RACE_DATA_OFFSET 0xA29C20
#define COURSE_MAPPINGS_OFFSET 0xC0018
#define IS_PAUSED_OFFSET 0xD6B58
#define SHOP_DATA_START 0xC1CB8

#define SWR_AP_BASE_ID 11380000
#define RACERS_COUNT 22

namespace SWRGame
{
	void Log(const char* format, ...);
	void ScanLocationChecks();

	void QueueDeath();
	void ProcessDeathQueue();
	void KillPod();
	void CheckPodKilled();

	void Init();
	void StartupSequenceLoop();
	void Update();
	void InitSaveData();
	void CopySaveData(RacerSaveData* racerSaveData);

	void ScoutWattoShop();

	bool isSaveFileLoaded();
	bool isSaveDataReady();
	bool isPlayerInRace();
	bool isPlayerKillable();

	int baseAddress;
	APServerInfo serverInfo;
	APSaveData saveData;

	SWRGameState gamestate;
}


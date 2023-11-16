#pragma once

#include "Enums.h"
#include "Structs.h"
#include "Archipelago.h"
#include "Patches.h"

#define POD_PARTS_OFFSET 0xA35AA1
#define POD_DATA_PTR_OFFSET 0xA29C44
#define SAVE_DATA_OFFSET 0xA35A5C
#define RACE_DATA_OFFSET 0xA29C20

class SWRGame
{
private:
	int baseAddress;
	int queuedDeaths;

	void ProcessDeathQueue();
	void KillPod();
public:
	SWRGame();
	void Update();

	bool isPlayerInRace();
	bool isPlayerKillable();

	void QueueDeath();
};


#include "SWR.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <random>
#include <chrono>

SWRGame::SWRGame()
{
	baseAddress = (int)GetModuleHandleA("SWEP1RCR.EXE");
	Patches::SetBaseAddress(baseAddress);

	queuedDeaths = 0;
	
	// Temporary, for testing purposes
	// Limit character selection to a random racer
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen(seed);
	std::uniform_int_distribution<int> distribution(0, 22);
	int random = distribution(gen);
	int randomSelection = 1 << random;

	Patches::LimitAvailableRacers((RacerUnlocks)randomSelection);
	Patches::DisablePitDroidShop();
	Patches::DisablePartDegradation();
}

void SWRGame::Update()
{
	ProcessDeathQueue();
}

bool SWRGame::isPlayerInRace()
{
	PodData* playerPodData = *(PodData**)(baseAddress + POD_DATA_PTR_OFFSET);
	if (playerPodData == nullptr)
		return false;

	RaceData* raceData = (RaceData*)(baseAddress + RACE_DATA_OFFSET);
	if (raceData == nullptr)
		return false;

	if (raceData->timer > 0.0f)
		return true;

	return false;
}

bool SWRGame::isPlayerKillable()
{
	if (!isPlayerInRace())
		return false;

	PodData* playerPodData = *(PodData**)(baseAddress + POD_DATA_PTR_OFFSET);
	if (playerPodData == nullptr)
		return false;

	int unkillable = PodStatus::Destroyed | PodStatus::Autopilot | PodStatus::Invincible;

	if ((playerPodData->status & unkillable) != 0)
		return false;

	return true;
}

void SWRGame::KillPod() 
{
	// Function at SWE1RCR.EXE + 0x74970 checks this flag and destroys the pod if it is set
	PodData* playerPodData = *(PodData**)(baseAddress + POD_DATA_PTR_OFFSET);
	if (playerPodData == nullptr)
		return;

	playerPodData->status |= PodStatus::Destroyed;
	queuedDeaths--;
}

void SWRGame::QueueDeath()
{
	queuedDeaths++;
}

void SWRGame::ProcessDeathQueue()
{
	if ((queuedDeaths > 0) && isPlayerKillable())
		KillPod();
}
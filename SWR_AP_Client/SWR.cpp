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
	requiredPlacement = RacePlacement::First;

	Log("Star Wars Episode I Racer Archipelago Client started");
	
	// Temporary, for testing purposes
	// Limit character selection to a random racer
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen(seed);
	std::uniform_int_distribution<int> distribution(0, 22);
	int random = distribution(gen);
	int randomSelection = 1 << random;

	Log("Applying patch: Limit Available Racers");
	Patches::LimitAvailableRacers((RacerUnlocks)randomSelection);
	Log("Applying patch: Disable Pit Droid Shop");
	Patches::DisablePitDroidShop();
	Log("Applying patch: Disable Part Degredation");
	Patches::DisablePartDegradation();
}

void SWRGame::Update()
{
	if (isSaveFileLoaded())
	{
		ScanLocationChecks();
	}

	ProcessDeathQueue();

	Sleep(50);
}

void SWRGame::Log(const char* format, ...)
{
	auto now = std::chrono::system_clock::now();
	std::string newFormat = std::format("[{0:%T}] {1}\n", now, format);
	va_list args;
	va_start(args, format);
	vprintf(newFormat.c_str(), args);
	va_end(args);
}

bool SWRGame::isSaveFileLoaded()
{
	char* firstChar = (char*)(baseAddress + SAVE_DATA_OFFSET);
	return firstChar[0] != 0;
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

	Log("Killing player");
	Log("Queued deaths: %i", queuedDeaths);
}

void SWRGame::QueueDeath()
{
	queuedDeaths++;

	Log("Deathlink received! Queueing death");
	Log("Queued deaths: %i", queuedDeaths);
}

void SWRGame::ScanLocationChecks()
{
	SaveData* saveData = (SaveData*)(baseAddress + SAVE_DATA_OFFSET);

	// Race progress
	if (requiredPlacement == RacePlacement::Fourth)
	{
		// Check unlocked courses

	}
	else
	{
		// Check placement flags
		int flag;

		for (int i = 0; i < completedCourses.size(); i++)
		{
			if (completedCourses[i].completed)
				continue;

			flag = saveData->racePlacements >> (completedCourses[i].slot * 2);
			flag &= 0x03;

			if (flag >= (int)requiredPlacement)
			{
				completedCourses[i].completed = true;

				// Notify of location check
				Log("Location checked: %s", completedCourses[i].name.c_str());
			}
		}
	}

	// Watto Shop

	// Junkyard

	// Pit Droid Shop
}

void SWRGame::ProcessDeathQueue()
{
	if ((queuedDeaths > 0) && isPlayerKillable())
		KillPod();
}
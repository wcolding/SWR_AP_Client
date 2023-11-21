#include "SWR.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <random>
#include <chrono>

namespace SWRGame
{
	int queuedDeaths;
	RacePlacement requiredPlacement;

	void Init()
	{
		baseAddress = (int)GetModuleHandleA("SWEP1RCR.EXE");

		queuedDeaths = 0;
		requiredPlacement = RacePlacement::First;

		Log("Star Wars Episode I Racer Archipelago Client started");

		Log("Applying patch: Limit Available Racers");
		Patches::LimitAvailableRacers();
		Log("Applying patch: Disable Pit Droid Shop");
		Patches::DisablePitDroidShop();
		Log("Applying patch: Disable Part Degredation");
		Patches::DisablePartDegradation();

		ResetSaveData();

		// Temporary, for testing purposes
		// Limit character selection to a random racer
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine gen(seed);
		std::uniform_int_distribution<int> distribution(0, 22);
		int random = distribution(gen);
		saveData.unlockedRacers = (RacerUnlocks)(1 << random);
	}

	void Update()
	{
		if (isSaveFileLoaded())
		{
			ScanLocationChecks();
		}

		ProcessDeathQueue();

		Sleep(50);
	}

	void Log(const char* format, ...)
	{
		auto now = std::chrono::system_clock::now();
		std::string newFormat = std::format("[{0:%T}] {1}\n", now, format);
		va_list args;
		va_start(args, format);
		vprintf(newFormat.c_str(), args);
		va_end(args);
	}

	bool isSaveFileLoaded()
	{
		char* firstChar = (char*)(baseAddress + SAVE_DATA_OFFSET);
		return firstChar[0] != 0;
	}

	bool isPlayerInRace()
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

	bool isPlayerKillable()
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

	void KillPod()
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

	void QueueDeath()
	{
		queuedDeaths++;

		Log("Deathlink received! Queueing death");
		Log("Queued deaths: %i", queuedDeaths);
	}

	void ScanLocationChecks()
	{
		RacerSaveData* racerSaveData = (RacerSaveData*)(baseAddress + SAVE_DATA_OFFSET);

		// Race progress
		if (requiredPlacement == RacePlacement::Fourth)
		{
			// Check unlocked courses

		}
		else
		{
			// Check placement flags
			int flag;
			CourseData* course;
			for (int i = 0; i < saveData.completedCourses.size(); i++)
			{
				course = &saveData.completedCourses[i];
				if (course->completed)
					continue;

				flag = racerSaveData->racePlacements >> (course->slot * 2);
				flag &= 0x03;

				if (flag >= (int)requiredPlacement)
				{
					course->completed = true;

					// Notify of location check
					Log("Location checked: %s", course->name.c_str());
				}
			}
		}

		// Watto Shop

		// Junkyard

		// Pit Droid Shop
	}

	void ProcessDeathQueue()
	{
		if ((queuedDeaths > 0) && isPlayerKillable())
			KillPod();
	}

	void ResetSaveData()
	{
		saveData.completedCourses.clear();
		saveData.completedCourses = {
				{"Amateur Race 1", 0, false},
				{"Amateur Race 2", 1, false},
				{"Amateur Race 3", 2, false},
				{"Amateur Race 4", 3, false},
				{"Amateur Race 5", 4, false},
				{"Amateur Race 6", 5, false},
				{"Amateur Race 7", 6, false},

				{"Semi-Pro Race 1", 8, false},
				{"Semi-Pro Race 2", 9, false},
				{"Semi-Pro Race 3", 10, false},
				{"Semi-Pro Race 4", 11, false},
				{"Semi-Pro Race 5", 12, false},
				{"Semi-Pro Race 6", 13, false},
				{"Semi-Pro Race 7", 14, false},

				{"Galactic Race 1", 16, false},
				{"Galactic Race 2", 17, false},
				{"Galactic Race 3", 18, false},
				{"Galactic Race 4", 19, false},
				{"Galactic Race 5", 20, false},
				{"Galactic Race 6", 21, false},
				{"Galactic Race 7", 22, false},

				{"Invitational Race 1", 24, false},
				{"Invitational Race 2", 25, false},
				{"Invitational Race 3", 26, false},
				{"Invitational Race 4", 27, false}
		};
	}


}
#include "SWR.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <random>
#include <chrono>
#include <map>

#include "APCpp/Archipelago.h"

namespace SWRGame
{
	int queuedDeaths;
	RacePlacement requiredPlacement;
	std::map<int, int> courseLayout;
	DeathState deathState = DeathState::Alive;

	void Update()
	{
		if (isSaveFileLoaded())
		{
			ScanLocationChecks();
		}

		if (isPlayerInRace())
		{
			CheckPodKilled();
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

		int paused = *(int*)(baseAddress + IS_PAUSED_OFFSET);
		if (paused == 1)
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
		// Function at SWEP1RCR.EXE + 0x74970 checks this flag and destroys the pod if it is set
		PodData* playerPodData = *(PodData**)(baseAddress + POD_DATA_PTR_OFFSET);
		if (playerPodData == nullptr)
			return;

		playerPodData->status |= PodStatus::Destroyed;
		queuedDeaths--;
		deathState = DeathState::Deathlink;

		Log("Killing player");
		Log("Queued deaths: %i", queuedDeaths);
	}

	void CheckPodKilled()
	{
		PodData* playerPodData = *(PodData**)(baseAddress + POD_DATA_PTR_OFFSET);
		if (playerPodData == nullptr)
			return;

		PodStatus respawning = (PodStatus)(PodStatus::Autopilot | PodStatus::Invincible);
		if ((playerPodData->status & respawning) != 0)
		{
			if (deathState == DeathState::Alive)
			{
				AP_DeathLinkSend();
				Log("Pod destroyed!");
				deathState = DeathState::Local;
			}
		}
		else
		{
			deathState = DeathState::Alive;
		}
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

	void ReceiveItem(int64_t itemID, bool notify)
	{

	}

	void SetLocationChecked(int64_t locID)
	{

	}

	void SetDisablePartDegradation(int value) 
	{
		if (value)
		{
			Log("Applying patch: Disable Part Degredation");
			Patches::DisablePartDegradation();
			Log("Applying patch: Disable Pit Droid Shop");
			Patches::DisablePitDroidShop();
		}
	}

	void SetStartingRacers(int value)
	{
		saveData.unlockedRacers = (RacerUnlocks)value;

		Log("Applying patch: Limit Available Racers");
		Patches::LimitAvailableRacers();
	}

	void SetCourses(std::map<int, int> courseValues)
	{
		courseLayout = courseValues;

		for (int i = 0; i < courseLayout.size(); i++)
		{
			memcpy((void*)(baseAddress + COURSE_MAPPINGS_OFFSET + 4 * i), &courseLayout[i], 4);
		}
		
		Log("Courses set");
	}

	void APSetup()
	{
		AP_Init(serverInfo.server, "Star Wars Episode I Racer", serverInfo.player, serverInfo.pw);

		AP_NetworkVersion version = { 0, 4, 2 };
		AP_SetClientVersion(&version);
		AP_SetDeathLinkSupported(true);

		AP_SetItemClearCallback(&ResetSaveData);
		AP_SetItemRecvCallback(&ReceiveItem);
		AP_SetLocationCheckedCallback(&SetLocationChecked);
		AP_SetDeathLinkRecvCallback(&QueueDeath);

		AP_RegisterSlotDataIntCallback("StartingRacers", &SetStartingRacers);
		AP_RegisterSlotDataIntCallback("DisablePartDegradation", &SetDisablePartDegradation);
		AP_RegisterSlotDataMapIntIntCallback("Courses", &SetCourses);

		AP_Start();
	}

	void Init()
	{
		baseAddress = (int)GetModuleHandleA("SWEP1RCR.EXE");

		APSetup();

		queuedDeaths = 0;
		requiredPlacement = RacePlacement::First;

		Log("Star Wars Episode I Racer Archipelago Client started");

		ResetSaveData();
	}
}
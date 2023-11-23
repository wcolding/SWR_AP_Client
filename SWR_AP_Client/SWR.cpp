#include "SWR.h"
#include "Locations.h"

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
	bool initSave = false;

	void Update()
	{
		if (isSaveFileLoaded())
		{
			if (initSave)
			{
				InitSaveData();
			}
			else
			{
				ScanLocationChecks();
			}
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
		if (racerSaveData == nullptr)
			return;

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
					int locID = courseSlotToId[course->slot];
					Log("Location checked: %s", locationTable[locID].c_str());
					locID += SWR_AP_BASE_ID;
					AP_SendItem(locID);
				}
			}
		}

		// Racer Unlock Checks
		if (racerSaveData->racerUnlocks != saveData.racerSaveDataCopy.racerUnlocks)
		{
			RacerUnlocks curRacer;
			for (int i = 0; i < RACERS_COUNT; i++)
			{
				curRacer = (RacerUnlocks)(1 << i);
				if ((racerSaveData->racerUnlocks & curRacer) != 0)
				{
					if (racerUnlockTable.contains(curRacer))
					{
						int locID = racerUnlockTable[curRacer];
						Log("Location checked: %s", locationTable[locID].c_str());
						locID += SWR_AP_BASE_ID;
						AP_SendItem(locID);
					}
				}
			}

			CopySaveData(racerSaveData);
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


	void CopySaveData(RacerSaveData* racerSaveData)
	{
		memcpy(&saveData.racerSaveDataCopy, racerSaveData, sizeof(RacerSaveData));
	}

	void InitSaveData()
	{
		RacerSaveData* racerSaveData = (RacerSaveData*)(baseAddress + SAVE_DATA_OFFSET);
		if (racerSaveData == nullptr)
			return;

		racerSaveData->racerUnlocks = RacerUnlocks::None;
		racerSaveData->trackUnlocks.semipro = 0;
		racerSaveData->trackUnlocks.galactic = 0;

		CopySaveData(racerSaveData);

		saveData.completedCourses.clear();
		saveData.completedCourses = {
				{0, false},
				{1, false},
				{2, false},
				{3, false},
				{4, false},
				{5, false},
				{6, false},

				{8, false},
				{9, false},
				{10, false},
				{11, false},
				{12, false},
				{13, false},
				{14, false},

				{16, false},
				{17, false},
				{18, false},
				{19, false},
				{20, false},
				{21, false},
				{22, false},

				{24, false},
				{25, false},
				{26, false},
				{27, false}
		};

		initSave = false;
		Log("Save data initialized");
	}

	void ResetSaveData()
	{
		CourseData* course;
		for (int i = 0; i < saveData.completedCourses.size(); i++)
		{
			course = &saveData.completedCourses[i];
			course->completed = false;
		}
	}

	void ReceiveItem(int64_t itemID, bool notify)
	{

	}

	void SetLocationChecked(int64_t locID)
	{

	}

	void SetStartingRacers(int value)
	{
		saveData.unlockedRacers = (RacerUnlocks)value;
		Patches::LimitAvailableRacers();
	}

	void SetDisablePartDegradation(int value)
	{
		if (value)
		{
			Patches::DisablePartDegradation();
			Patches::DisablePitDroidShop();
		}
	}

	void SetRequiredPlacement(int value)
	{
		switch (value)
		{
		case 0:
			requiredPlacement = RacePlacement::First;
			break;
		case 1:
			requiredPlacement = RacePlacement::Second;
			break;
		case 2:
			requiredPlacement = RacePlacement::Third;
			break;
		}
		
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

		AP_NetworkVersion version = { 0, 4, 3 };
		AP_SetClientVersion(&version);
		AP_SetDeathLinkSupported(true);

		AP_SetItemClearCallback(&ResetSaveData);
		AP_SetItemRecvCallback(&ReceiveItem);
		AP_SetLocationCheckedCallback(&SetLocationChecked);
		AP_SetDeathLinkRecvCallback(&QueueDeath);

		AP_RegisterSlotDataIntCallback("StartingRacers", &SetStartingRacers);
		AP_RegisterSlotDataIntCallback("DisablePartDegradation", &SetDisablePartDegradation);
		AP_RegisterSlotDataIntCallback("RequiredPlacement", &SetRequiredPlacement);
		AP_RegisterSlotDataMapIntIntCallback("Courses", &SetCourses);

		AP_Start();
	}

	void Init()
	{
		Log("Star Wars Episode I Racer Archipelago Client started");
		baseAddress = (int)GetModuleHandleA("SWEP1RCR.EXE");

		APSetup();

		queuedDeaths = 0;

		initSave = true;
	}
}
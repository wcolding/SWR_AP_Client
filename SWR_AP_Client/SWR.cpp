#include "SWR.h"
#include "Locations.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <chrono>
#include <map>

#define LOAD_PROFILE_FUNC 0x21850
typedef void(__cdecl* _LoadProfile)(const char* profileName);

namespace SWRGame
{
	int queuedDeaths;
	DeathState deathState = DeathState::Alive;
	bool doInitSave = false;
	RacerSaveData* racerSaveData; 
	RacerSaveData** saveDataPtr;

	std::vector<ItemShopEntry*> wattoShopData;
	std::vector<std::string> wattoShopItemNames;

	_LoadProfile LoadProfile;

	void Update()
	{
		ScanLocationChecks();

		if (isPlayerInRace())
			CheckPodKilled();

		ProcessDeathQueue();
		ProcessItemQueue();

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

	bool isSaveDataReady()
	{
		if (*saveDataPtr == nullptr)
			return false;
		else
		{
			racerSaveData = *saveDataPtr;
			return true;
		}
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

	void ScanLocationChecks()
	{
		if (!isSaveDataReady())
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

	void CopySaveData(RacerSaveData* inGameSaveData)
	{
		memcpy(&saveData.racerSaveDataCopy, inGameSaveData, sizeof(RacerSaveData));
	}

	void InitSaveData()
	{
		if (!isSaveDataReady())
			return;

		racerSaveData->racerUnlocks = RacerUnlocks::None;
		racerSaveData->trackUnlocks[0] = 1;
		racerSaveData->trackUnlocks[1] = 0;
		racerSaveData->trackUnlocks[2] = 0;
		racerSaveData->trackUnlocks[3] = 0;

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

		doInitSave = false;
		Log("Save data initialized");
	}

	void GivePart(int type, int part)
	{
		if (!isSaveDataReady())
			return;
		
		char partVal = racerSaveData->parts[type];
		if (partVal >= 5)
			return;

		if (part == -1) // Progressive
		{
			partVal++;
			racerSaveData->parts[type] = partVal;
		}
		else
		{
			if (partVal > part)
				return;
			racerSaveData->parts[type] = (char)part;
		}
	}

	void GiveRacer(int racerID)
	{
		saveData.unlockedRacers = (RacerUnlocks)(saveData.unlockedRacers | racerID);
	}

	void GivePitDroid()
	{
		if (!isSaveDataReady())
			return;

		racerSaveData->pitDroids++;
	}

	void GiveCircuitPass(int type)
	{
		if (!isSaveDataReady())
			return;

		if (type == -1)
		{
			// Progressive
			for (int i = 1; i < 4; i++)
			{
				if (racerSaveData->trackUnlocks[i] == 0)
				{
					racerSaveData->trackUnlocks[i] |= 0x01;
					return;
				}
			}
		}
		else
			racerSaveData->trackUnlocks[type] |= 0x01;
	}

	void GiveMoney(int amount)
	{
		if (!isSaveDataReady())
			return;

		racerSaveData->money += amount;
	}

	void ProcessItemQueue()
	{
		if (!itemQueue.empty() && isSaveDataReady())
		{
			ItemInfo itemInfo = itemQueue.front();
			itemQueue.erase(itemQueue.begin());
			Log("Received item \'%s\'", itemInfo.name.c_str());

			switch (itemInfo.type)
			{
			case ItemType::PodPart:
				GivePart(itemInfo.param1, itemInfo.param2);
				break;
			case ItemType::Racer:
				GiveRacer(itemInfo.param1);
				break;
			case ItemType::PitDroid:
				GivePitDroid();
				break;
			case ItemType::CircuitPass:
				GiveCircuitPass(itemInfo.param1);
				break;
			case ItemType::Money:
				GiveMoney(itemInfo.param1);
				break;
			default:
				break;
			}
		}
	}

	void ScoutWattoShop()
	{
		std::vector<int64_t> locations;

		for (int i = 100; i < 135; i++)
			locations.push_back(i + SWR_AP_BASE_ID);

		AP_SendLocationScouts(locations, 0);
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
		AP_SetLocationInfoCallback(&RecvLocationInfo);
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
		gamestate = SWRGameState::Starting;

		LoadProfile = (_LoadProfile)(baseAddress + LOAD_PROFILE_FUNC);

		saveDataPtr = (RacerSaveData**)(baseAddress + SAVE_DATA_PTR_OFFSET);
		racerSaveData = nullptr;

		queuedDeaths = 0;

		for (int i = 0; i < 42; i++)
			wattoShopData.push_back((ItemShopEntry*)(baseAddress + SHOP_DATA_START + sizeof(ItemShopEntry) * i));

		APSetup();
	}

	void StartupSequenceLoop()
	{
		if (gamestate == SWRGameState::Starting)
		{
			// Wait for AP to fully connect to the server
			if (AP_GetConnectionStatus() == AP_ConnectionStatus::Authenticated)
			{
				gamestate = SWRGameState::AP_Authenticated;

				//// Apply patches we don't need an AP callback for
				Patches::FixCourseSelection();
				Patches::RewriteWattoShop();

				// Set save directory
				AP_RoomInfo roomInfo;
				AP_GetRoomInfo(&roomInfo);
				std::string saveDirStr = R"(.\data\player\AP_)" + roomInfo.seed_name + R"(\)";
				memcpy((void*) &saveDirectory, saveDirStr.c_str(), saveDirStr.size());
				Patches::RedirectSaveFiles();
			}
		}

		if (gamestate == SWRGameState::AP_Authenticated)
		{
			// Wait for game to load
			if (isSaveDataReady())
			{
				InitSaveData();
				gamestate = SWRGameState::Save_Initialized;
			}
		}

		if (gamestate == SWRGameState::Save_Initialized)
		{
			// Scout locations, do any other things we want done before we start the update loop
			ScoutWattoShop();

			gamestate = SWRGameState::Ready;
		}
	}
}
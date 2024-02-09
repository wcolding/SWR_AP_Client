#include "SWR.h"
#include "Locations.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <chrono>
#include <map>

#define LOAD_PROFILE_FUNC 0x21850
#define SAVE_PROFILE_FUNC 0x219D0
typedef bool(__cdecl* _SaveLoadProfile)(const char* profileName);
typedef void(__cdecl* _WriteWhiteText)(int16_t xPos, int16_t yPos, const char* text);

namespace SWRGame
{
	int queuedDeaths;
	DeathState deathState = DeathState::Alive;
	RacerSaveData* racerSaveData; 
	RacerSaveData** saveDataPtr;

	std::vector<ItemShopEntry*> wattoShopData;
	std::vector<std::string> wattoShopItemNames;

	_SaveLoadProfile LoadProfile;
	_SaveLoadProfile SaveProfile;

	_WriteWhiteText WriteWhiteText;

	std::chrono::steady_clock::time_point prevTime;

	void OnDraw()
	{
		auto curTime = std::chrono::steady_clock::now();

		if (!notifyQueue.empty())
		{
			if (notifyQueue[0].timeRemaining <= 0)
				notifyQueue.erase(notifyQueue.begin());
			else
			{
				const std::chrono::duration<float> deltaTime = curTime - prevTime;
				notifyQueue[0].timeRemaining -= deltaTime.count();
				WriteWhiteText(5, 5, notifyQueue[0].msg.c_str());
			}
		}

		prevTime = curTime;
	}

	void QueueNotifyMsg(std::string _msg)
	{
		NotifyMsg newMsg;
		newMsg.msg = _msg;
		newMsg.timeRemaining = 2.5;
		notifyQueue.push_back(newMsg);
	}

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

	template<typename T>
	T* GetArrayEntry(void* array, int index)
	{
		return (T*)((int)array + sizeof(T) * index);
	}

	template<typename T>
	void RemoveElementFromArray(void* array, int index, size_t arrayLength)
	{
		for (int i = index; i < arrayLength - 1; i++)
		{
			T* curElement = (T*)((int)array + sizeof(T) * i);
			T* nextElement = (T*)((int)array + sizeof(T) * (i + 1));
			*curElement = *nextElement;
		}
	}

	void __fastcall MarkRaceCompletion(int circuit, int course)
	{
		int locationOffset = 145 + circuit * 7 + course;
		std::string locationName = locationTable[locationOffset];
		Log("Location checked: %s", locationName.c_str());
		AP_SendItem(locationOffset + SWR_AP_BASE_ID);
	}

	void __fastcall MarkShopPurchase(int entryOffset)
	{
		int tableOffset = entryOffset / 0x10;

		for (auto pair : wattoShopLocationToOffset)
		{
			if (pair.second == tableOffset)
			{
				Log("Location checked: %s", locationTable[pair.first].c_str());
				wattoShopData[tableOffset]->requiredRaces |= 0x80; // mark as completed
				AP_SendItem(pair.first + SWR_AP_BASE_ID);
				
				int* removedIndex = (int*)(baseAddress + 0xA295D0);
				size_t* selectionCount = (size_t*)(baseAddress + 0xA295CC);

				void* shopInvModels = (void*)(baseAddress + 0xA29A88);

				// Hide model
				MeshData*** removedMeshPtr = (MeshData***)((int)shopInvModels + 4 * *removedIndex);
				MeshData* removedMesh = **removedMeshPtr;
				removedMesh->visible = false;

				RemoveElementFromArray<int>(shopInvModels, *removedIndex, *selectionCount);

				void* shopInvData = (void*)(baseAddress + 0xA2A6C0);
				RemoveElementFromArray<ShopInventoryEntry>(shopInvData, *removedIndex, *selectionCount);

				--*selectionCount;
				if (*removedIndex >= *selectionCount)
					--*removedIndex;
			}
		}
	}

	void __fastcall MarkPitDroidPurchase() 
	{
		if (pitDroidChecksCompleted >= 4)
			return;

		int locationOffset = 141 + pitDroidChecksCompleted;
		std::string locationName = locationTable[locationOffset];
		Log("Location checked: %s", locationName.c_str());
		AP_SendItem(locationOffset + SWR_AP_BASE_ID);
		pitDroidChecksCompleted++;
	}
	void ScanLocationChecks()
	{
		if (!isSaveDataReady())
			return;

		

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
		bool saveExists = LoadProfile(serverInfo.player);

		// Reset values of progressive/stackable items (except circuit pass)
		// AP will send items on connect so we will recalculate from the base values
		racerSaveData->money = 400;
		racerSaveData->pitDroids = 1;
		memset(&racerSaveData->parts, 0, 7);

		if (!saveExists)
		{
			// Set actual initial data
			// todo: figure out handling progressive circuit pass
			memcpy(&racerSaveData->profileName, &serverInfo.player, 24);
			racerSaveData->racerUnlocks = RacerUnlocks::None;
			racerSaveData->trackUnlocks[0] = 1;
			racerSaveData->trackUnlocks[1] = 0;
			racerSaveData->trackUnlocks[2] = 0;
			racerSaveData->trackUnlocks[3] = 0;
			racerSaveData->cutscenesBitfield = 0xFFFFFFFF;
			memset(&racerSaveData->partsHealth, 0xFF, 7);

			// Saving once appears to write to the "live" save area of tgfd.dat
			// Saving twice appears to make an actual .sav file
			SaveProfile(serverInfo.player);
			SaveProfile(serverInfo.player);
		}

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
			QueuedItem item = itemQueue.front();
			ItemInfo itemInfo = item.info;
			Log("Received item \'%s\'", itemInfo.name.c_str());
			if (item.notify)
				QueueNotifyMsg(itemInfo.name);

			itemQueue.erase(itemQueue.begin());

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

		AP_NetworkVersion version = CLIENT_VERSION;
		AP_SetClientVersion(&version);
		AP_SetDeathLinkSupported(true);

		AP_SetItemClearCallback(&ResetSaveData);
		AP_SetItemRecvCallback(&ReceiveItem);
		AP_SetLocationCheckedCallback(&SetLocationChecked); 
		AP_SetLocationInfoCallback(&RecvLocationInfo);
		AP_SetDeathLinkRecvCallback(&QueueDeath);

		AP_RegisterSlotDataIntCallback("StartingRacers", &SetStartingRacers);
		AP_RegisterSlotDataIntCallback("DisablePartDegradation", &SetDisablePartDegradation);
		AP_RegisterSlotDataMapIntIntCallback("Courses", &SetCourses);

		AP_Start();
	}

	void Init()
	{
		Log("Star Wars Episode I Racer Archipelago Client started");
		baseAddress = (int)GetModuleHandleA("SWEP1RCR.EXE");
		gamestate = SWRGameState::Starting;

		LoadProfile = (_SaveLoadProfile)(baseAddress + LOAD_PROFILE_FUNC);
		SaveProfile = (_SaveLoadProfile)(baseAddress + SAVE_PROFILE_FUNC);

		WriteWhiteText = (_WriteWhiteText)(baseAddress + 0x50560);

		saveDataPtr = (RacerSaveData**)(baseAddress + SAVE_DATA_PTR_OFFSET);
		racerSaveData = nullptr;

		queuedDeaths = 0;

		for (int i = 0; i < 42; i++)
		{
			ItemShopEntry* nextItem = (ItemShopEntry*)(baseAddress + SHOP_DATA_START + sizeof(ItemShopEntry) * i);
			if (i % 6 == 0)
				nextItem->requiredRaces |= 0x80; // mark base items so the shop doesn't display them
			wattoShopData.push_back(nextItem);
		}

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
				Patches::HookDraw();
				Patches::FixCourseSelection();
				Patches::RewriteWattoShop(); 
				Patches::HookRaceRewards();
				Patches::DisableJunkyard();

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
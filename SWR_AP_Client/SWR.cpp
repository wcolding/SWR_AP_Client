#include "SWR.h"
#include "Locations.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <chrono>
#include <map>

#define LOAD_PROFILE_FUNC 0x21850
#define SAVE_PROFILE_FUNC 0x219D0
typedef bool(__cdecl* _SaveLoadProfile)(const char* profileName);

namespace SWRGame
{
	int queuedDeaths;
	DeathState deathState = DeathState::Alive;
	SWR_SaveData** saveDataPtr;

	std::vector<SWR_PodPartEntry*> wattoShopData;

	_SaveLoadProfile LoadProfile;
	_SaveLoadProfile SaveProfile;

	char sessionProgressivePasses = 0;

	void QueueNotifyMsg(std::string _msg)
	{
		NotifyMsg newMsg;
		newMsg.msg = _msg;
		newMsg.timeRemaining = 2.5;
		notifyQueue.push_back(newMsg);
	}

	void Update()
	{
		// Only act on matching save data
		if (swrSaveData->apPartialSeed == partialSeed)
		{
			SyncProgress();

			if (isPlayerInRace())
				CheckPodKilled();
			
			ProcessMessages();
			ProcessDeathQueue();
			ProcessItemQueue();

			if (GetKeyState(VK_HOME) & 0x80)
				ChangeAIModifier(0.100);
			if (GetKeyState(VK_END) & 0x80)
				ChangeAIModifier(-0.100);
			if (GetKeyState(VK_PRIOR) & 0x80)
				ChangeAIModifier(0.010);
			if (GetKeyState(VK_NEXT) & 0x80)
				ChangeAIModifier(-0.010);
		}
		
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

	void SendAPItem(int localLocationId)
	{
		std::vector<int64_t> locations;
		locations.push_back(localLocationId + SWR_AP_BASE_ID);
		std::string locationName = locationTable[localLocationId];
		Log("Location checked: %s", locationName.c_str());
		AP_SendItem(locations[0]);
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

		swrSaveData = *saveDataPtr;
		return true;
	}

	bool isPlayerInRace()
	{
		PodData* playerPodData = *(PodData**)(baseAddress + POD_DATA_PTR_OFFSET);
		if (playerPodData == nullptr)
			return false;

		RaceData* raceData = (RaceData*)(baseAddress + RACE_DATA_OFFSET);
		if (raceData == nullptr)
			return false;

		return true;
	}

	bool isPlayerKillable()
	{
		if (!isPlayerInRace())
			return false;
		else
		{
			RaceData* raceData = (RaceData*)(baseAddress + RACE_DATA_OFFSET);
			if (raceData->timer < 0.1f)
				return false;
		}

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

	int __fastcall CalculateRacesCompleted()
	{
		int count = 0;
		int flag;

		for (int i = 0; i < 25; i++)
		{
			flag = 1 << i;
			if ((swrSaveData->racesCompleted & flag) != 0)
				count++;
		}
		
		return count;
	}

	int prevRacesCount = 0;

	void SendShopHints(int coursesCompleted)
	{
		std::vector<int64_t> locations;
		for (auto id : shopUnlockHintsTable[coursesCompleted])
			locations.push_back(id + SWR_AP_BASE_ID);

		AP_SendLocationScouts(locations, 1);
	}

	void __fastcall MarkRaceCompletion(int circuit, int course)
	{
		int courseIndex = circuit * 7 + course;
		int locationOffset = 145 + courseIndex;
		SendAPItem(locationOffset);

		if (invitationalCircuitPass && (circuit == 3))
		{
			int unlockFlag = 1 << (course + 1);
			progress.cachedSave.trackUnlocks[3] |= (char)unlockFlag;
		}

		if (shuffledCourseUnlocks)
		{
			if (courseClearToUnlock.contains(locationOffset))
				SendAPItem(courseClearToUnlock[locationOffset]);
		}

		int courseFlag = 1 << courseIndex;
		swrSaveData->racesCompleted |= courseFlag;

		if (hintShop)
		{
			int completed = CalculateRacesCompleted();
			if ((completed < 17) && (completed % 2 == 0))
			{
				if (completed != prevRacesCount)
				{
					SendShopHints(completed);
					prevRacesCount = completed;
				}
			}
		}

		if (swrSaveData->racesCompleted == 0x1FFFFFF)
			AP_StoryComplete();
	}

	void __fastcall MarkShopPurchase()
	{
		int tableOffset = GetShopItemOffset();
		int cursor = *(int*)(baseAddress + 0xA295D0);
		SWR_PodPartEntry* entry;

		for (auto pair : wattoShopLocationToOffset)
		{
			if (pair.second == tableOffset)
			{
				entry = &apShopData.entries[tableOffset];
				if ((entry->requiredRaces & 0x80) == 0)
				{
					entry->requiredRaces |= 0x80; // mark as completed
					SendAPItem(pair.first);
					swrSaveData->money -= entry->cost; // manually deduct cost since we skipped the vanilla call

					// Change model to Watto
					entry->modelId = 0x6E;
					UpdateShopModel(cursor, 0x6E);
				}
			}
		}
	}

	bool __fastcall isItemPodPart()
	{
		int offset = GetShopItemOffset();
		return apShopData.entries[offset].itemType < 7;
	}

	typedef void(__cdecl* _LoadItemModelO)(int modelId, int unk_00, int unk_01, int unk_02);
	_LoadItemModelO LoadItemModelO;

	void __fastcall LoadItemModel(int unk_00, int tableOffset)
	{
		int entryIndex = tableOffset / 0x10;
		auto entry = &apShopData.entries[entryIndex];

		if (((entry->requiredRaces & 0x80) == 0) && ((entry->requiredRaces & 0x20) != 0))
		{
			int level = (int)(swrSaveData->parts[entry->itemType]) + 1;
			if (level > 5)
				level = 5;

			int itemTableIndex = 6 + level + (5 * entry->itemType);
			entry->modelId = itemTable[itemTableIndex].modelId;
			entry->seriesId = level;
		}

		if ((entry->requiredRaces & 0x80) != 0)
			LoadItemModelO(0x6E, -1, unk_00, 00);
		else
			LoadItemModelO(entry->modelId, -1, unk_00, 00);
	}

	int GetShopItemOffset()
	{
		int cursor = *(int*)(baseAddress + 0xA295D0);
		return (int)*(char*)(baseAddress + 0xA2A6C0 + (0x38 * cursor));
	}

	void __fastcall MarkPitDroidPurchase() 
	{
		if (progress.pitDroidCounter >= 4)
			return;

		int locationOffset = 141 + progress.pitDroidCounter;
		SendAPItem(locationOffset);
	}

	PodParts partsCache = { 0,0,0,0,0,0,0 };

	typedef int(__cdecl* _UpdateStatDisplay)(int unk_00, int partType, int shopPartLevel, int shopPartHealth);
	_UpdateStatDisplay UpdateStatDisplay;

	void __fastcall UpdateProgressiveDisplays()
	{
		int itemCount =  *(int*)(baseAddress + 0xA295CC);
		int entryIndex = 0;
		int itemTableIndex = 0;
		int cursor = *(int*)(baseAddress + 0xA295D0);
		unsigned char partLevel;
		SWR_PodPartEntry* entry;
		for (int i = 0; i < itemCount; i++)
		{
			entryIndex = (int)*(char*)(baseAddress + 0xA2A6C0 + (0x38 * i));
			entry = &apShopData.entries[entryIndex];

			// Don't overwrite purchased items
			if ((entry->requiredRaces & 0x80) != 0)
				continue;

			partLevel = swrSaveData->parts[entry->itemType];
			if ((entry->requiredRaces & 0x20) != 0) // if entry is a progressive pod part
			{
				if (partLevel != partsCache[entry->itemType])
				{
					partsCache[entry->itemType] = partLevel;
					partLevel++;
					if (partLevel > 5)
						partLevel = 5;

					// Update entry data
					itemTableIndex = 6 + partLevel + (5 * entry->itemType);
					entry->seriesId = partLevel;
					entry->modelId = itemTable[itemTableIndex].modelId;

					UpdateShopModel(i, entry->modelId);

					if (cursor == i)
						UpdateStatDisplay(1, entry->itemType, entry->seriesId, 0xFF);
				}
			}
		}
	}

	void SyncProgress()
	{
		if (!isSaveDataReady())
			return;

		if (invitationalCircuitPass)
		{
			// Unlock any invitational tracks unlocked on a previous load
			swrSaveData->trackUnlocks[3] |= progress.cachedSave.trackUnlocks[3];
			progress.cachedSave.trackUnlocks[3] = swrSaveData->trackUnlocks[3];
		}

		// Racer Unlock Checks
		if (swrSaveData->racerUnlocks != progress.cachedSave.racerUnlocks)
		{
			RacerUnlocks curRacer;
			for (int i = 0; i < RACERS_COUNT; i++)
			{
				curRacer = (RacerUnlocks)(1 << i);
				if ((swrSaveData->racerUnlocks & curRacer) != 0)
				{
					if (racerUnlockTable.contains(curRacer))
					{
						int locID = racerUnlockTable[curRacer];
						SendAPItem(locID);
					}
				}
			}

			progress.cachedSave.racerUnlocks = swrSaveData->racerUnlocks;
		}

		// Progressive circuit passes
		if (sessionProgressivePasses > swrSaveData->progressivePasses)
		{
			swrSaveData->progressivePasses = sessionProgressivePasses;
			Log("Progressive circuit passes: %i", (int)swrSaveData->progressivePasses);
			for (int i = 0; i < swrSaveData->progressivePasses; i++) 
				swrSaveData->trackUnlocks[i+1] |= 0x01;
		}
	}

	void ProcessDeathQueue()
	{
		if ((queuedDeaths > 0) && isPlayerKillable())
			KillPod();
	}

	void InitSaveData()
	{
		// Reset values of progressive/stackable items (except circuit pass and course unlocks)
		// AP will send items on connect so we will recalculate from the base values
		swrSaveData->pitDroids = 1;
		memset(&swrSaveData->parts, 0, 7);

		Log("Save data initialized");
	}

	void GivePart(int type, int part)
	{
		if (!isSaveDataReady())
			return;
		
		auto current = &swrSaveData->parts[type];
		auto curHealth = &swrSaveData->partsHealth[type];
		*curHealth = 0xFF;

		if (*current >= 5)
			return;

		if (part == -1) // Progressive
			++*current;
		else
			if (*current < (char)part)
				*current = (char)part;
	}

	void GiveRacer(int racerID)
	{
		progress.unlockedRacers = (RacerUnlocks)(progress.unlockedRacers | racerID);
	}

	void GivePitDroid()
	{
		swrSaveData->pitDroids++;
	}

	void GiveCircuitPass(int type)
	{
		if (type == -1)
			sessionProgressivePasses++;
		else
			swrSaveData->trackUnlocks[type] |= 0x01;
	}

	void GiveMoney(int amount)
	{
		swrSaveData->money += amount;
	}

	void GiveCourseUnlock(int circuit)
	{
		int curUnlock = swrSaveData->trackUnlocks[circuit];
		int curFlag = 0;
		for (int i = 0; i < 7; i++)
		{
			curFlag = 1 << i;
			if ((curUnlock & curFlag) == 0)
			{
				swrSaveData->trackUnlocks[circuit] |= curFlag;
				return;
			}
		}
	}

	void ProcessItemQueue()
	{
		if (!itemQueue.empty() && isSaveDataReady())
		{
			QueuedItem item = itemQueue.front();
			ItemInfo itemInfo = item.info;
			Log("Received item \'%s\'", itemInfo.name.c_str());

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
				if (item.notify)
					GiveMoney(itemInfo.param1);
				break;
			case ItemType::CourseUnlock:
				if (item.notify)
					GiveCourseUnlock(itemInfo.param1);
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

		AP_SetClientVersion(&version);
		AP_SetDeathLinkSupported(true);

		AP_SetItemClearCallback(&ResetSaveData);
		AP_SetItemRecvCallback(&ReceiveItem);
		AP_SetLocationCheckedCallback(&SetLocationChecked); 
		AP_SetLocationInfoCallback(&RecvLocationInfo);
		AP_SetDeathLinkRecvCallback(&QueueDeath);

		AP_RegisterSlotDataIntCallback("StartingRacers", &SetStartingRacers);
		AP_RegisterSlotDataIntCallback("DisablePartDamage", &SetDisablePartDamage);
		AP_RegisterSlotDataIntCallback("CourseUnlockMode", &SetCourseUnlockMode);
		AP_RegisterSlotDataIntCallback("AIScaling", &SetAIScaling);
		AP_RegisterSlotDataIntCallback("AdditionalAIMultiplier", &SetAdditionalScaling);
		AP_RegisterSlotDataIntCallback("EnableMultiplierControl", &SetEnableMultiplierControl);
		AP_RegisterSlotDataIntCallback("OneLapMode", &SetOneLapMode);
		AP_RegisterSlotDataIntCallback("AutoHintShop", &SetAutoHintShop);
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

		WriteText = (_WriteText)(baseAddress + 0x503E0);

		saveDataPtr = (SWR_SaveData**)(baseAddress + SAVE_DATA_PTR_OFFSET);
		swrSaveData = nullptr;

		menuVal = (int*)(baseAddress + 0xD87A4);
		menuValB = (int*)(baseAddress + 0xA2A67C);

		DrawStats = (_DrawStats)(baseAddress + 0x550D0);
		LoadItemModelO = (_LoadItemModelO)(baseAddress + 0x54BC0);
		UpdateStatDisplay = (_UpdateStatDisplay)(baseAddress + 0x5CF60);

		versionString = std::format("Version {}.{}.{}", version.major, version.minor, version.build), 

		queuedDeaths = 0;
		
		SWR_PodPartTable* originalPodPartTable = (SWR_PodPartTable*)(baseAddress + SHOP_DATA_START);
		apShopData = *originalPodPartTable;

		for (int i = 0; i < 42; i++)
		{
			if (i % 6 == 0)
				apShopData.entries[i].requiredRaces = 0xC0; // mark base items so the shop doesn't display them
		}

		//// Apply patches we don't need an AP callback for
		Patches::HookSaveFiles();
		Patches::HookDraw();
		//Patches::HookInput();
		Patches::FixCourseSelection();
		Patches::RewriteWattoShop();
		Patches::HookRaceRewards();
		Patches::HookDroidShop();
		Patches::DisableJunkyard();
		Patches::DisableAwardsCeremony();
		Patches::SetAPModeString();
		Patches::EnableMirroredCourses();

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

				// Set save directory
				AP_RoomInfo roomInfo;
				AP_GetRoomInfo(&roomInfo);
				fullSeedName = "Seed: " + roomInfo.seed_name;
				std::string partialSeedStr = roomInfo.seed_name.substr(0, 8);
				partialSeed = (uint64_t)strtoll(partialSeedStr.c_str(), nullptr, 10);
			}
		}

		if (gamestate == SWRGameState::AP_Authenticated)
		{
			// Wait for game to load
			if (isSaveDataReady())
			{
				if (swrSaveData->apPartialSeed == partialSeed) 
				{
					InitSaveData();
					gamestate = SWRGameState::Save_Initialized;
				}
			}
		}

		if (gamestate == SWRGameState::Save_Initialized)
		{
			// Scout locations, do any other things we want done before we start the update loop
			ScoutWattoShop();
			GameStatus** statusPtr = (GameStatus**)(SWRGame::baseAddress + 0xBFDB8);
			GameStatus* gameStatus = *statusPtr;
			if (gameStatus != nullptr)
				gameStatus->payoutSetting = PayoutSetting::Fair;

			gamestate = SWRGameState::Ready;
		}
	}
}
#include "SWR.h"
#include "Locations.h"
#include "Items.h"

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
	bool doInitSave = false;
	RacerSaveData* racerSaveData;

	std::vector<ItemShopEntry*> wattoShopData;
	std::vector<std::string> wattoShopItemNames;

	void Update()
	{
		if (isSaveFileLoaded())
		{
			if (doInitSave)
			{
				InitSaveData();
				ScoutWattoShop();
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
		if (racerSaveData == nullptr)
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
		if (racerSaveData == nullptr)
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
		if (racerSaveData == nullptr)
			return;

		racerSaveData->pitDroids++;
	}

	void GiveCircuitPass(int type)
	{
		if (racerSaveData == nullptr)
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
		if (racerSaveData == nullptr)
			return;

		racerSaveData->money += amount;
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

	void ScoutWattoShop()
	{
		std::vector<int64_t> locations;

		for (int i = 100; i < 135; i++)
			locations.push_back(i + SWR_AP_BASE_ID);

		AP_SendLocationScouts(locations, 0);
	}

	void ReceiveItem(int64_t itemID, bool notify)
	{
		int localID = (int)itemID - SWR_AP_BASE_ID;

		if (itemTable.contains(localID))
		{
			ItemInfo* itemInfo = &itemTable[localID];
			Log("Received item \'%s\'", itemInfo->name.c_str());

			switch (itemInfo->type)
			{
			case ItemType::PodPart:
				GivePart(itemInfo->param1, itemInfo->param2);
				break;
			case ItemType::Racer:
				GiveRacer(itemInfo->param1);
				break;
			case ItemType::PitDroid:
				GivePitDroid();
				break;
			case ItemType::CircuitPass:
				GiveCircuitPass(itemInfo->param1);
				break;
			case ItemType::Money:
				GiveMoney(itemInfo->param1);
				break;
			default:
				break;
			}
		}
	}

	void SetLocationChecked(int64_t locID)
	{

	}

	void UpdateProgressiveItemModels()
	{
		// todo
	}

	void RecvLocationInfo(std::vector<AP_NetworkItem> items)
	{
		Log("Received location info: %i items", items.size());

		int curLocId;
		int curItemId;

		for (auto item : items)
		{
			curItemId = item.item - SWR_AP_BASE_ID;
			curLocId = item.location - SWR_AP_BASE_ID;
			if (wattoShopLocationToOffset.contains(curLocId))
			{
				ItemShopEntry* curEntry = wattoShopData[wattoShopLocationToOffset[curLocId]];
				wattoShopItemNames.push_back(std::format("{} [{}]", item.itemName.c_str(), item.playerName.c_str()));
				curEntry->displayText = (char*)wattoShopItemNames.back().c_str();

				if (itemTable.contains(curItemId))
				{
					ItemInfo curItem = itemTable[curItemId];
					if (curItem.modelId != -1)
						curEntry->modelId = curItem.modelId;
					else
					{
						// Item is progressive
						int curPartValue;
						int nextPartId;

						switch (curItem.type)
						{
						case ItemType::PodPart:
							curPartValue = (int)racerSaveData->parts[curItemId];
							nextPartId = 7 + 5 * curItemId + curPartValue;
							if (curPartValue == 5)
								nextPartId--;
							curEntry->modelId = itemTable[nextPartId].modelId;
							break;
						case ItemType::CircuitPass:
							curEntry->modelId = 0x6F; // dewback for undiscovered model ids
							break;
						default:
							break;
						}
					}
				}
				else
					curEntry->modelId = 0x71; // Jabba for AP Items
			}
		}
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

		racerSaveData = (RacerSaveData*)(baseAddress + SAVE_DATA_OFFSET);

		for (int i = 0; i < 42; i++)
			wattoShopData.push_back((ItemShopEntry*)(baseAddress + SHOP_DATA_START + sizeof(ItemShopEntry) * i));

		APSetup();

		Patches::FixCourseSelection();
		Patches::RewriteWattoShop();

		queuedDeaths = 0;

		doInitSave = true;
	}
}
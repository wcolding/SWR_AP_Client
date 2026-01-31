#include "APCallbacks.h"

#include "Locations.h"
#include "Patches.h"

#include <format>
#include <cstdlib>

namespace SWRGame
{
	void ResetSaveData()
	{
		Log("ResetSaveData called by AP");
		SWRGame::saveManager.ResetSaveData();
	}

	void ReceiveItem(int64_t itemID, bool notify)
	{
		int localID = (int)itemID - SWR_AP_BASE_ID;

		if (itemTable.contains(localID))
			itemQueue.push_back({ itemTable[localID], notify });
	}

	void SetLocationChecked(int64_t locID)
	{
		int localID = (int)locID - SWR_AP_BASE_ID;

		if (locationTable.contains(localID))
			Log("Setting location '%s' as checked", locationTable[localID].c_str());

		// For race completions (145 - 169), mark progress
		if ((144 < localID) && (localID < 152))
			SWRGame::saveManager.SetCourseAsCompleted(AMATEUR_CIRCUIT, localID - 145);

		if ((151 < localID) && (localID < 159))
			SWRGame::saveManager.SetCourseAsCompleted(SEMIPRO_CIRCUIT, localID - 145);

		if ((158 < localID) && (localID < 166))
			SWRGame::saveManager.SetCourseAsCompleted(GALACTIC_CIRCUIT, localID - 145);

		if ((165 < localID) && (localID < 170))
			SWRGame::saveManager.SetCourseAsCompleted(INVITATIONAL_CIRCUIT, localID - 145);

		// Watto
		if (wattoShopLocationToOffset.contains(localID))
		{
			apShopData.entries[wattoShopLocationToOffset[localID]].requiredRaces |= 0x80;
		}

		// Pit droid
		if ((141 < localID) && (localID < 145))
			SWRGame::progress.pitDroidCounter++;
	}

	std::vector<int> GetDisguiseItems()
	{
		std::vector<int> items;
		
		// Money items
		for (int i = 75; i < 77; i++)
			items.push_back(i);

		// Progression items
		switch (courseUnlockMode)
		{
		case CourseUnlockMode::ProgressiveCircuits:
			items.push_back(69); // Progressive Circuit Pass x3
			items.push_back(69); // todo: make this nicer
			items.push_back(69);
			break;

		case CourseUnlockMode::Circuits:
			items.push_back(66); // Semi-pro
			items.push_back(67); // Galactic
			items.push_back(68); // Invitational
			break;

		case CourseUnlockMode::FullShuffle:
			for (int i = 78; i < 81; i++)
				items.push_back(i); // Course unlocks
			break;
		default:
			break;
		}

		// Vroom vroom racers
		items.push_back(44); // Sebulba
		items.push_back(61); // Ben Quadrinaros

		return items;
	}

	std::map<ItemType, AP_ItemType> gameToAPTypes
	{
		{ItemType::CircuitPass, AP_ItemType::Progression},
		{ItemType::CourseUnlock, AP_ItemType::Progression},
		{ItemType::Money, AP_ItemType::Useful},
		{ItemType::Racer, AP_ItemType::Filler}
	};

	void DisguiseTrap(AP_NetworkItem& item)
	{
		auto disguises = GetDisguiseItems();
		int index = rand() % disguises.size();
		ItemInfo info = itemTable[disguises[index]];

		item.flags = gameToAPTypes[info.type];
		item.item = disguises[index] + SWR_AP_BASE_ID;
		item.itemName = info.name;
		item.playerName = std::string(serverInfo.player);
	}

	void RecvLocationInfo(std::vector<AP_NetworkItem> items)
	{
		Log("Received location info: %i items", items.size());

		int curLocId;
		int curItemId;

		for (auto item : items)
		{
			AP_NetworkItem itemCopy = item;
			if (itemCopy.flags == AP_ItemType::Trap)
				DisguiseTrap(itemCopy);

			curItemId = itemCopy.item - SWR_AP_BASE_ID;
			curLocId = itemCopy.location - SWR_AP_BASE_ID;
			if (wattoShopLocationToOffset.contains(curLocId))
			{
				SWR_PodPartEntry* curEntry = &apShopData.entries[wattoShopLocationToOffset[curLocId]];
				wattoShopEntries[curLocId] = { 
					(AP_ItemType)itemCopy.flags,
					std::format("{} [{}]", itemCopy.itemName.c_str(), itemCopy.playerName.c_str()),
					wattoShopLocationToOffset[curLocId]
					};

				curEntry->displayText = (char*)wattoShopEntries[curLocId].displayName.c_str();
				if (itemTable.contains(curItemId))
				{
					ItemInfo curItem = itemTable[curItemId];

					switch (curItem.type)
					{
					case ItemType::PodPart:
						if (curItem.param2 == -1)
						{
							// Pod part is progressive
							curEntry->modelId = 0x6E; // Use Watto as temp model. Will be overwritten when we load the shop
							curEntry->itemType = curItem.param1;
							curEntry->seriesId = 1; // Will be overwritten when we load the shop
							curEntry->requiredRaces |= 0x20; // Marker for progressives
						}
						else
						{
							curEntry->modelId = curItem.modelId;
							curEntry->itemType = curItem.param1;
							curEntry->seriesId = curItem.param2;
						}
						break;
					default:
						curEntry->itemType = 7;
						curEntry->seriesId = 0;
						curEntry->modelId = curItem.modelId;
						break;
					}
				}
				else
				{
					curEntry->itemType = 7;
					curEntry->seriesId = 0;
					curEntry->modelId = 0x71; // Jabba for AP Items
				}
			}
		}
	}

	void QueueDeath()
	{
		queuedDeaths++;

		Log("Deathlink received! Queueing death");
		Log("Queued deaths: %i", queuedDeaths);
	}

	void ProcessMessages()
	{
		if (!AP_IsMessagePending())
			return;

		auto msg = AP_GetLatestMessage();
		if (msg == nullptr)

		switch (msg->type)
		{
		case AP_MessageType::ItemSend:
		{
			AP_ItemSendMessage* sendMsg = static_cast<AP_ItemSendMessage*>(msg);
			if (strcmp(sendMsg->recvPlayer.c_str(), serverInfo.player) != 0)
				QueueNotifyMsg("~k~1Sent ~4" + sendMsg->item + "~1 to ~2" + sendMsg->recvPlayer);

			AP_ClearLatestMessage();
			break;
		}
		case AP_MessageType::ItemRecv:
		{
			AP_ItemRecvMessage* recvMsg = static_cast<AP_ItemRecvMessage*>(msg);
			if (strcmp(recvMsg->sendPlayer.c_str(), serverInfo.player) != 0)
				QueueNotifyMsg("~k~1Received ~4" + recvMsg->item + "~1 from ~2" + recvMsg->sendPlayer);
			else
				QueueNotifyMsg("~k~1You found your ~4" + recvMsg->item);

			AP_ClearLatestMessage();
			break;
		}
		default:
			AP_ClearLatestMessage();
			break;
		}
	}

	void SetStartingRacers(int value)
	{
		saveManager.apRacerUnlocks = static_cast<SWRMemTools::RacerUnlocks>(value);
		Patches::LimitAvailableRacers();
	}

	void SetDisablePartDamage(int value)
	{
		if (value)
			Patches::DisablePartDamage();
	}

	void SetCourseUnlockMode(int value)
	{
		courseUnlockMode = static_cast<CourseUnlockMode>(value);
		Patches::DisableVanillaCourseUnlocks();
		Patches::DisableVanillaInvitationalUnlocks();

		if (courseUnlockMode == CourseUnlockMode::ProgressiveCircuits)
			progressiveCircuits = true;
	}

	void SetAIScaling(int value)
	{
		Patches::ScaleAIDifficulty(value);
	}

	void SetAdditionalScaling(int value)
	{
		SWRGame::aiModifier = (float)value / 1000;
		Log("AI modifier set: %f", SWRGame::aiModifier);
	}

	void SetEnableMultiplierControl(int value)
	{
		SWRGame::modifierControl = value != 0;
		if (SWRGame::modifierControl)
			Log("Modifier control enabled");
		else
			Log("Modifier control disabled");
	}

	void SetOneLapMode(int value)
	{
		if (value != 0)
			Patches::EnableOneLapMode();
	}

	void SetAutoHintShop(int value)
	{
		if (value != 0)
		{
			hintShop = true;
			SendShopHints(0); // hint initial shop checks
		}
	}

	void SetDeathLink(int value)
	{
		deathLink = value;
	}

	void SetDeathLinkAmnesty(int value)
	{
		deathLinkAmnesty = value;
		livesRemaining = value;
	}

	void SetCourses(std::map<int, int> courseValues)
	{
		for (int i = 0; i < courseValues.size(); i++)
		{
			if ((courseValues[i] & 0x80) != 0)
				courseLayout[i].mirrored = true;

			courseLayout[i].id = courseValues[i];
			courseLayout[i].id |= 0x80; 
			courseLayout[i].id ^= 0x80;
		
			memcpy((void*)(baseAddress + COURSE_MAPPINGS_OFFSET + 4 * i), &courseLayout[i].id, 4);
		}

		Log("Courses set");
	}

	void SetShopCosts(std::map<int, int> costs)
	{
		for (auto entry : costs)
			apShopData.entries[entry.first].cost = entry.second;
		
		Log("Shop costs set");
	}
}
#include "APCallbacks.h"

#include "Locations.h"
#include "Patches.h"

#include <format>

namespace SWRGame
{
	void ResetSaveData()
	{
		Log("ResetSaveData called by AP");
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

		// For race completions (145 - 169), do nothing

		// Watto
		if (wattoShopLocationToOffset.contains(localID))
		{
			apShopData.entries[wattoShopLocationToOffset[localID]].requiredRaces |= 0x80;
		}

		// Pit droid
		if ((141 < localID) && (localID < 145))
			SWRGame::progress.pitDroidCounter++;
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
				SWR_PodPartEntry* curEntry = &apShopData.entries[wattoShopLocationToOffset[curLocId]];
				wattoShopEntries.push_back({ 
					(AP_ItemType)item.flags, 
					std::format("{} [{}]", item.itemName.c_str(), item.playerName.c_str()),
					wattoShopLocationToOffset[curLocId]
					});
				curEntry->displayText = (char*)wattoShopEntries.back().displayName.c_str();
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
		progress.unlockedRacers = (RacerUnlocks)value;
		Patches::LimitAvailableRacers();
	}

	void SetDisablePartDamage(int value)
	{
		if (value)
			Patches::DisablePartDamage();
	}

	void SetCourseUnlockMode(int value)
	{
		if (value == 1)
		{
			invitationalCircuitPass = true;
			Patches::DisableVanillaInvitationalUnlocks();
		}
		else if (value == 2)
		{
			shuffledCourseUnlocks = true; 
			Patches::DisableVanillaCourseUnlocks();
			Patches::DisableVanillaInvitationalUnlocks();
		}

	}

	void SetAIScaling(int value)
	{
		if (value != 0)
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
}
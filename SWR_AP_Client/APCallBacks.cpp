#include "APCallbacks.h"

#include "Locations.h"
#include "Patches.h"

#include <format>

namespace SWRGame
{
	std::map<int, int> courseLayout;

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
		int localID = (int)itemID - SWR_AP_BASE_ID;

		if (itemTable.contains(localID))
			itemQueue.push_back({ itemTable[localID], notify });
	}

	void SetLocationChecked(int64_t locID)
	{
		int localID = (int)locID - SWR_AP_BASE_ID;

		if (locationTable.contains(localID))
			Log("Setting location '%s' as checked", locationTable[localID].c_str());

		// todo: actually set it as checked
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
							// This needs a rework. Maybe we can patch the game to read the item as progressive and do this work live
							curPartValue = 0; //temporary //(int)racerSaveData->parts[curItemId];
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

	void QueueDeath()
	{
		queuedDeaths++;

		Log("Deathlink received! Queueing death");
		Log("Queued deaths: %i", queuedDeaths);
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
		else
			Patches::HookDroidShop();
	}

	void SetCourses(std::map<int, int> courseValues)
	{
		courseLayout = courseValues;

		for (int i = 0; i < courseLayout.size(); i++)
			memcpy((void*)(baseAddress + COURSE_MAPPINGS_OFFSET + 4 * i), &courseLayout[i], 4);

		Log("Courses set");
	}
}
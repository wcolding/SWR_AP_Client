#pragma once
#include "Structs.h"
#include "Items.h"
#include "APCpp/Archipelago.h"

#define COURSE_MAPPINGS_OFFSET 0xC0018

namespace SWRGame
{
	extern APSaveData saveData;
	extern void Log(const char* format, ...);
	extern std::vector<ItemShopEntry*> wattoShopData;
	extern std::vector<std::string> wattoShopItemNames;
	extern int queuedDeaths;
	extern std::vector <QueuedItem> itemQueue;

	void ResetSaveData();
	void ReceiveItem(int64_t itemID, bool notify);
	void SetLocationChecked(int64_t locID);
	void RecvLocationInfo(std::vector<AP_NetworkItem> items);
	void QueueDeath();

	void SetStartingRacers(int value);
	void SetDisablePartDegradation(int value);
	void SetCourses(std::map<int, int> courseValues);
}
#pragma once
#include "Structs.h"
#include "Items.h"
#include "APCpp/Archipelago.h"

#define COURSE_MAPPINGS_OFFSET 0xC0018

namespace SWRGame
{
	extern AP_ProgressData progress;
	extern AP_ServerInfo serverInfo;
	extern void Log(const char* format, ...);

	std::vector<AP_WattoEntry> wattoShopEntries;
	extern int queuedDeaths;
	extern std::vector <QueuedItem> itemQueue;
	extern void QueueNotifyMsg(std::string _msg);
	extern bool invitationalCircuitPass;
	extern bool shuffledCourseUnlocks;
	extern float aiModifier;
	extern bool modifierControl;
	extern SWR_PodPartTable apShopData;

	void ResetSaveData();
	void ReceiveItem(int64_t itemID, bool notify);
	void SetLocationChecked(int64_t locID);
	void RecvLocationInfo(std::vector<AP_NetworkItem> items);
	void QueueDeath();
	void ProcessMessages();

	void SetStartingRacers(int value);
	void SetDisablePartDamage(int value);
	void SetCourseUnlockMode(int value);
	void SetAIScaling(int value);
	void SetAdditionalScaling(int value);
	void SetEnableMultiplierControl(int value);
	void SetOneLapMode(int value);
	void SetCourses(std::map<int, int> courseValues);
}
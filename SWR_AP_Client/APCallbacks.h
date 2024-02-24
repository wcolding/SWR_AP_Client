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
	extern std::vector<SWR_PodPartEntry*> wattoShopData;
	extern std::vector<std::string> wattoShopItemNames;
	extern int queuedDeaths;
	extern std::vector <QueuedItem> itemQueue;
	extern void QueueNotifyMsg(std::string _msg);
	extern bool invitationalCircuitPass;
	extern float aiModifier;

	void ResetSaveData();
	void ReceiveItem(int64_t itemID, bool notify);
	void SetLocationChecked(int64_t locID);
	void RecvLocationInfo(std::vector<AP_NetworkItem> items);
	void QueueDeath();
	void ProcessMessages();

	void SetStartingRacers(int value);
	void SetDisablePartDamage(int value);
	void SetInvitationalCircuitPass(int value);
	void SetAIScaling(int value);
	void SetAdditionalScaling(int value);
	void SetCourses(std::map<int, int> courseValues);
}
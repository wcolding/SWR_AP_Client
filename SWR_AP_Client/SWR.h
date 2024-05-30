#pragma once

#include "Enums.h"
#include "Structs.h"
#include "Patches.h"
#include "Items.h"
#include "APCpp/Archipelago.h"

#define POD_PARTS_OFFSET 0xA35AA1
#define POD_DATA_PTR_OFFSET 0xA29C44
#define SAVE_DATA_OFFSET 0xA35A60
#define SAVE_DATA_PTR_OFFSET 0xA29BCC
#define RACE_DATA_OFFSET 0xA29C20
#define IS_PAUSED_OFFSET 0xD6B58
#define SHOP_DATA_START 0xC1CB8

#define RACERS_COUNT 23


typedef void(__cdecl* _WriteText)(short xPos, short yPos, int r, int g, int b, int a, const char* text, int unk_00, int unk_01);
typedef void(__cdecl* _DrawStats)(void* obj, float x, float y);

namespace SWRGame
{
	AP_NetworkVersion version = { 0, 5, 3 };

	void Log(const char* format, ...);
	void SyncProgress();

	void ProcessDeathQueue();
	void KillPod();
	void CheckPodKilled();

	void ProcessItemQueue();

	void Init();
	void StartupSequenceLoop();
	void Update();
	void InitSaveData();
	void QueueNotifyMsg(std::string _msg);


	extern void OnDraw();
	extern _WriteText WriteText;

	int* menuVal = nullptr;
	int* menuValB = nullptr;

	void ScoutWattoShop();

	bool isSaveFileLoaded();
	bool isSaveDataReady();
	bool isPlayerInRace();
	bool isPlayerKillable();

	int baseAddress;
	AP_ServerInfo serverInfo;
	AP_ProgressData progress;
	SWR_PodPartTable apShopData;

	SWR_SaveData* swrSaveData;

	std::string fullSeedName = "";
	std::string versionString = "";
	uint64_t partialSeed = 0;

	bool invitationalCircuitPass = false;
	bool shuffledCourseUnlocks = false;

	SWRGameState gamestate;
	std::vector <QueuedItem> itemQueue;
	std::vector <NotifyMsg> notifyQueue;

	_DrawStats DrawStats;

	void __fastcall MarkRaceCompletion(int circuit, int course);
	void __fastcall MarkShopPurchase();
	void __fastcall MarkPitDroidPurchase();
	bool __fastcall isItemPodPart();
	void __fastcall LoadItemModel(int unk_00, int tableOffset);
	int GetShopItemOffset();	
	void __fastcall UpdateProgressiveDisplays();
	extern void UpdateShopModel(int selectedIndex, int modelId);

	// Callbacks
	extern void ResetSaveData();
	extern void ReceiveItem(int64_t itemID, bool notify);
	extern void SetLocationChecked(int64_t locID);
	extern void RecvLocationInfo(std::vector<AP_NetworkItem> items);
	extern void QueueDeath();
	extern void ProcessMessages();

	extern void SetStartingRacers(int value);
	extern void SetDisablePartDamage(int value);
	extern void SetCourseUnlockMode(int value);
	extern void SetAIScaling(int value);
	extern void SetAdditionalScaling(int value);
	extern void SetEnableMultiplierControl(int value);
	extern void SetOneLapMode(int value);
	extern void SetCourses(std::map<int, int> courseValues);
}
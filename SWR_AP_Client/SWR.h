#pragma once

#include "Enums.h"
#include "Structs.h"
#include "Patches.h"
#include "Items.h"
#include "APCpp/Archipelago.h"

#define CLIENT_VERSION { 0, 4, 4 }

#define POD_PARTS_OFFSET 0xA35AA1
#define POD_DATA_PTR_OFFSET 0xA29C44
#define SAVE_DATA_OFFSET 0xA35A60
#define SAVE_DATA_PTR_OFFSET 0xA29BCC
#define RACE_DATA_OFFSET 0xA29C20
#define IS_PAUSED_OFFSET 0xD6B58
#define SHOP_DATA_START 0xC1CB8

#define RACERS_COUNT 23

namespace SWRGame
{
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
	void OnDraw();
	void QueueNotifyMsg(std::string _msg);

	void ScoutWattoShop();

	bool isSaveFileLoaded();
	bool isSaveDataReady();
	bool isPlayerInRace();
	bool isPlayerKillable();

	int baseAddress;
	AP_ServerInfo serverInfo;
	AP_ProgressData progress;

	uint64_t partialSeed = 0;
	bool invitationalCircuitPass = false;

	SWRGameState gamestate;
	std::vector <QueuedItem> itemQueue;
	std::vector <NotifyMsg> notifyQueue;

	void __fastcall MarkRaceCompletion(int circuit, int course);
	void __fastcall MarkShopPurchase(int entryOffset);
	void __fastcall MarkPitDroidPurchase();
	float GetAIScaleFromParts();
	float GetAIScaleByCircuit(float val, int baseCircuit, int targetCircuit);

	// Callbacks
	extern void ResetSaveData();
	extern void ReceiveItem(int64_t itemID, bool notify);
	extern void SetLocationChecked(int64_t locID);
	extern void RecvLocationInfo(std::vector<AP_NetworkItem> items);
	extern void QueueDeath();
	extern void ProcessMessages();

	extern void SetStartingRacers(int value);
	extern void SetDisablePartDegradation(int value);
	extern void SetInvitationalCircuitPass(int value);
	extern void SetAIScaling(int value);
	extern void SetCourses(std::map<int, int> courseValues);
}
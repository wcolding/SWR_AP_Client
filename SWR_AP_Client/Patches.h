#pragma once
#include "Enums.h"
#include "Structs.h"
#include "Locations.h"
#include "SWRMemTools/include/Save.h"

namespace SWRGame
{
	extern int baseAddress;
	extern AP_ProgressData progress;
	extern SWRMemTools::SaveManager saveManager;
	extern void Log(const char* format, ...);
	extern char saveDirectory[64];
	extern void OnDraw();
	extern void __fastcall MarkRaceCompletion(int circuit, int course);
	extern void __fastcall MarkShopPurchase();
	extern void __fastcall MarkPitDroidPurchase();
	extern int __fastcall CalculateRacesCompleted();
	extern uint64_t partialSeed;
	extern float GetAIScaleFromParts();
	extern float GetAIScaleByCircuit(float val, int baseCircuit, int targetCircuit);
	extern float GetAIScaleVanilla(float val);
	extern SWR_PodPartTable apShopData;
	extern void ChangeAIModifier(float amount);
	extern SWR_SaveData* swrSaveData;
	extern int shopDisplayMask;

	extern std::map<int, RandomizedCourseData> courseLayout;

	
	extern void __fastcall PrintItemNameBuyView();
	extern void __fastcall ShopDrawStats(void* obj);
	extern void __fastcall LoadItemModel(int unk_00, int tableOffset);
	extern void __fastcall UpdateProgressiveDisplays();
}
namespace SWRGame::DrawEvents
{
	extern void __fastcall OnDraw();
	extern void __fastcall OnDrawShopFullView();
	extern void __fastcall OnDrawShopBuyView();
	extern void __fastcall OnDrawShopStats(void* obj);
	extern void __fastcall OnDrawCourseSelect();
	extern void __fastcall OnDrawTrackInfo();
	extern void __fastcall OnDrawPreRaceMenu();
	extern void __fastcall OnDrawInRace();
}

static class Patches
{
private:
	static void MakePageWritable(const void* addr);
	static void WritePatch(int offset, const void* patchPtr, size_t len);
	static void HookFunction(int injectOffset, const void* function, size_t trailingNOP = 0); 
	static void NOP(int offset, size_t len);
public:
	static void HookDraw();
	static void HookInput();
	static void LimitAvailableRacers();
	static void DisablePitDroidShop(); // phasing this out
	static void DisablePartDamage();
	static void FixCourseSelection();
	static void RewriteWattoShop();
	static void HookDroidShop();
	static void DisableJunkyard();
	static void HookRaceRewards();
	static void DisableAwardsCeremony();
	static void DisableVanillaInvitationalUnlocks();
	static void DisableVanillaCourseUnlocks();
	static void ScaleAIDifficulty(int option);
	static void EnableOneLapMode();
	static void EnableMirroredCourses();

	static void HookSaveFiles();
	static void SetAPModeString();
};
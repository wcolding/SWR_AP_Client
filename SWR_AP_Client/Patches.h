#pragma once
#include "Enums.h"
#include "Structs.h"
#include "Locations.h"

namespace SWRGame
{
	extern int baseAddress;
	extern AP_ProgressData progress;
	extern void Log(const char* format, ...);
	extern char saveDirectory[64];
	extern void OnDraw();
	extern void __fastcall MarkRaceCompletion(int circuit, int course);
	extern void __fastcall MarkShopPurchase();
	extern void __fastcall MarkPitDroidPurchase();
	extern uint64_t partialSeed;
	extern float GetAIScaleFromParts();
	extern float GetAIScaleByCircuit(float val, int baseCircuit, int targetCircuit);
	extern SWR_PodPartTable apShopData;
	extern void ChangeAIModifier(float amount);

	extern void __fastcall PrintItemNameFullView();
	extern void __fastcall PrintItemNameBuyView();
	extern void __fastcall ShopDrawStats(void* obj);
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
	static void ScaleAIDifficulty(int option);

	static void HookSaveFiles();
	static void SetAPModeString();
};
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
	extern void __fastcall MarkShopPurchase(int entryOffset);
	extern void __fastcall MarkPitDroidPurchase();
	extern uint64_t partialSeed;
	extern float GetAIScaleFromParts();
	extern float GetAIScaleByCircuit(float val, int baseCircuit, int targetCircuit);
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
	static void LimitAvailableRacers();
	static void DisablePitDroidShop();
	static void DisablePartDegradation();
	static void FixCourseSelection();
	static void RewriteWattoShop();
	static void HookDroidShop();
	static void DisableJunkyard();
	static void HookRaceRewards();
	static void DisableAwardsCeremony();
	static void DisableVanillaInvitationalUnlocks();
	static void ScaleAIDifficulty();

	static void HookSaveFiles();
};
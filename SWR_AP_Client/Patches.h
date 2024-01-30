#pragma once
#include "Enums.h"
#include "Structs.h"

namespace SWRGame
{
	extern int baseAddress;
	extern APSaveData saveData;
	extern void Log(const char* format, ...);
	extern char saveDirectory[64];
	extern void OnDraw();
	extern void __fastcall MarkRaceCompletion(int circuit, int course);
	extern void __fastcall MarkShopPurchase(int entryOffset);
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
	static void HookRaceRewards();

	static void RedirectSaveFiles();
};
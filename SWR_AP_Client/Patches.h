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
}

static class Patches
{
private:
	static void MakePageWritable(const void* addr);
	static void WritePatch(int offset, const void* patchPtr, size_t len);
	static void HookFunction(int injectOffset, const void* function, size_t trailingNOP = 0);
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
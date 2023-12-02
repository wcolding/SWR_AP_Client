#pragma once
#include "Enums.h"
#include "Structs.h"

namespace SWRGame
{
	extern int baseAddress;
	extern APSaveData saveData;
	extern void Log(const char* format, ...);
}

static class Patches
{
private:
	static void MakePageWritable(const void* addr);
	static void WritePatch(int offset, const void* patchPtr, size_t len);
public:
	static void LimitAvailableRacers();
	static void DisablePitDroidShop();
	static void DisablePartDegradation();
	static void FixCourseSelection();
	static void RewriteWattoShop();
};
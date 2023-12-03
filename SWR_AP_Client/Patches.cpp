#include "Patches.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using SWRGame::baseAddress;
using SWRGame::saveData;

#define DEFAULT_RACERS_OPCODE 0x3DA37
#define CHECK_PIT_DROIDS_OPCODE 0x3D845
#define CHECK_PITDROID_SHOP_FROM_MENU 0x36A3E
#define CHECK_PITDROID_SHOP_FROM_UPGRADES_MENU 0x39C07
#define DAMAGE_APPLY_OPCODE 0x3D7B6
#define ERROR_CURSOR_OPCODE 0x3B195

#define DEFAULT_FIRST_COURSE_INJECT 0x3B379
#define SKIP_ITEM_INJECT 0x3E8EF

void Patches::MakePageWritable(const void* addr)
{
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery(addr, &mbi, sizeof(mbi));
	DWORD oldProtect;
	VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtect);
}

void Patches::WritePatch(int offset, const void* patchPtr, size_t len)
{
	void* funcPtr = (void*)(SWRGame::baseAddress + offset);
	MakePageWritable(funcPtr);
	memcpy(funcPtr, patchPtr, len);
}

void Patches::HookFunction(int injectOffset, const void* function, size_t trailingNOP)
{
	size_t payloadSize = 5 + trailingNOP;
	char payload[32];
	int functionInt = (int)function;
	int offset = SWRGame::baseAddress + injectOffset + 5;

	memset(payload, 0x90, payloadSize); // fill with NOP
	payload[0] = 0xE8; // call

	void* functionPtr = (void*)(functionInt - offset);
	memcpy(&payload[1], &functionPtr, 4);

	WritePatch(injectOffset, &payload, payloadSize);
}

// Game hardcodes initially unlocked racers in a function before ORing them with the save file's unlocks
// Overwrite the bitfield and change `or` to `mov` to explicitly set who is available
void Patches::LimitAvailableRacers()
{
	SWRGame::Log("Applying patch: Limit Available Racers");
	char limitRacers[6] = { 
		0x8B, 0x35, 0x00, 0x00, 0x00, 0x00 // mov esi, [00000000]
	};                 

	// Overwrite 0 with racers value
	RacerUnlocks* unlockedRacersPtr = &saveData.unlockedRacers;
	memcpy(&limitRacers[2], &unlockedRacersPtr, 4);

	WritePatch(DEFAULT_RACERS_OPCODE, &limitRacers, 6);
}

// Game checks number of owned pit droids to determine whether to show the shop
// Skip over the parts that tell it to
void Patches::DisablePitDroidShop()
{
	SWRGame::Log("Applying patch: Disable Pit Droid Shop");
	char disableFromMenu[2] = {
		0xEB, 0x0C	// jmp 0C
	};

	char disableFromUpgradesMenu[2] = {
		0xEB, 0x40	// jmp 40
	};

	WritePatch(CHECK_PITDROID_SHOP_FROM_MENU, &disableFromMenu, 2);
	WritePatch(CHECK_PITDROID_SHOP_FROM_UPGRADES_MENU, &disableFromUpgradesMenu, 2);
}

// Game does not degrade base pod parts 
// Trick it into thinking every part is a base part
void Patches::DisablePartDegradation()
{
	SWRGame::Log("Applying patch: Disable Part Degredation");
	char forceUnupgraded[6] = {
		0xB1, 0x00, // mov cl, 00
		0x90,       // nop
		0x90,       // nop
		0x90,       // nop
		0x90,       // nop
	};

	WritePatch(DAMAGE_APPLY_OPCODE, &forceUnupgraded, 6);
}

void __declspec(naked) DefaultToFirstCourse()
{
	// eax row # / circuit
	// ecx used to calc offset in course list
	// edx cursor value

	__asm 
	{
		cmp edx, 0xFFFFFFFF;
		jne [Vanilla];
		xor edx, edx;
		ret;

	Vanilla:
		shl ecx, 03;
		sub ecx, eax;
		add edx, ecx;
		ret;
	}
}

// Avoid player being able to select locked courses
// Default to first course on locked circuits
void Patches::FixCourseSelection()
{
	SWRGame::Log("Applying patch: Fix Course Selection");
	char forceSkipErrorCursor[2] = {
		0xEB, 0x05 // jmp 5
	};

	WritePatch(ERROR_CURSOR_OPCODE, &forceSkipErrorCursor, 2);

	HookFunction(DEFAULT_FIRST_COURSE_INJECT, &DefaultToFirstCourse, 2);
}

void __declspec(naked) SkipAcquiredItems()
{
	// Vanilla behavior subtracts 3 from the total unlocked courses to account for the defaults
	// In AP we start with 1 so we will subtract 1
	// We also need to mask ecx to ensure a correct cmp result
	__asm
	{
		movsx eax, bl;
		sub eax, 01; 
		or ecx, 0xFFFFFF00;
		xor ecx, 0xFFFFFF00;
		ret;
	}
}

void Patches::RewriteWattoShop()
{
	SWRGame::Log("Applying patch: Rewrite Watto Shop");

	// Don't show items marked as acquired
	// We're using the flag 0x80 to mark an item
	HookFunction(SKIP_ITEM_INJECT, &SkipAcquiredItems, 1);
}
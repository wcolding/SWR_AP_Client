#include "Patches.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


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
	RacerUnlocks* unlockedRacersPtr = &SWRGame::saveData.unlockedRacers;
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

   //Item shop
   //3EB6D
   //Gets cost of part for shop

   //37862
   //Sets part stat display in bottom left (can be invalid)
   // call +5cf60
   // cdecl (ecx, eax, edx) (?, podPartType, ?) (9, 2, 1)

   //56017
   //Gets model of item to replace (call can be jumped)

   //3EF60
   //->3EF80
   //Gets name of item to replace


// profiles
// +17FF writes string on profile page?


void Patches::RewriteWattoShop()
{
	SWRGame::Log("Applying patch: Rewrite Watto Shop");

	// Don't show items marked as acquired
	// We're using the flag 0x80 to mark an item
	HookFunction(SKIP_ITEM_INJECT, &SkipAcquiredItems, 1);
}

void Patches::RedirectSaveFiles()
{
	SWRGame::Log("Applying patch: Redirect Save Files");
	
	char loadNewDir[5] = {
		0x68, 0x00, 0x00, 0x00, 0x00 // push 0
	};

	void* dirPtr = &SWRGame::saveDirectory;
	memcpy(&loadNewDir[1], &dirPtr, 4);

	// This game is hardcoded to push the pointer of the default save path to the stack in several spots
	std::vector<int> opcodeOffsets = {
		0xC518,
		0x21861,
		0x2195F,
		0x21A14,
		0x21B9D,
		0x21CBE,
		0x21CD4
	};

	for (auto offset : opcodeOffsets)
		WritePatch(offset, &loadNewDir, 5);
}

void MarkRaceCompletion()
{
	SWRGame::Log("Called MarkRaceCompletion function");
}

void __declspec(naked) MarkRaceCompletionAsm()
{
	__asm
	{
		pushad;
		call MarkRaceCompletion;
		popad;
		ret;
	}
}

void Patches::HookRaceRewards()
{
	// +3A35F 
	// eax = circuit index
	// esi = course index

	// nop out +3A3BF (len=6)
	// Overwrite money reward opcode with a call to our function

	HookFunction(0x3A3BF, &MarkRaceCompletionAsm, 1);

	// +3A3C8 compare?

	// +3AA50
	// cmp byte ptr[placement], 3
	// hard-coded placement minimum?
}

// 8DF30 is render func

typedef void(__cdecl* _RenderTexture)(int a, void* b);
_RenderTexture OriginalRenderTexture;
const DWORD oRenderTextureCallOffset = 0x8DD28;

void __cdecl HookedRenderTexture(int a, void* b)
{
	__asm pushad;
	SWRGame::OnDraw();
	__asm popad;
	OriginalRenderTexture(a, b);
}

void Patches::HookDraw()
{
	OriginalRenderTexture = (_RenderTexture)(SWRGame::baseAddress + 0x8DF30);
	HookFunction(oRenderTextureCallOffset, &HookedRenderTexture, 0);
}
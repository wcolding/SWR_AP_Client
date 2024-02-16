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

void Patches::NOP(int offset, size_t len)
{
	void* funcPtr = (void*)(SWRGame::baseAddress + offset);
	MakePageWritable(funcPtr);
	memset(funcPtr, 0x90, len);
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
	RacerUnlocks* unlockedRacersPtr = &SWRGame::progress.unlockedRacers;
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

void __declspec(naked) MarkShopPurchaseWrapper()
{
	// eax table offset
	__asm
	{
		pushad;
		mov ecx, eax; // __fastcall reads from ecx
		call SWRGame::MarkShopPurchase;
		popad;
		ret;
	}
}

const char* shopPurchaseTitle = "  BUY";

// models
// 0x62 items
// 0x8E trade ins
void Patches::RewriteWattoShop()
{
	SWRGame::Log("Applying patch: Rewrite Watto Shop");

	// Don't show items marked as acquired
	// We're using the flag 0x80 to mark an item
	HookFunction(SKIP_ITEM_INJECT, &SkipAcquiredItems, 1);

	// Write part
	// +40914
	// can NOP
	// eax has the offset in the table
	//NOP(0x40914, 6);
	HookFunction(0x40914, &MarkShopPurchaseWrapper, 1);

	// Hide models for trade in items
	// 3E9D6 move model id into eax
	// value of 0x49 is invalid?
	// result is an invisible object
	char hideTradeInModels[6] = {
		0xB8, 0x49, 0x00, 0x00, 0x00, // mov eax, 0x49
		0x90                          // nop    
	};

	WritePatch(0x3E9D6, &hideTradeInModels, 6);

	// 3EC10 draws purchase window elements
	// Change title of right window
	char newShopPurchaseTitle[5] = {
		0x68, 0x00, 0x00, 0x00, 0x00 // push 0
	};

	memcpy(&newShopPurchaseTitle[1], &shopPurchaseTitle, 4);
	WritePatch(0x3EEB7, &newShopPurchaseTitle, 5);

	// Center title
	char adjustPurchaseTitleXPos[5] = {
		0x68, 0xC6, 0x00, 0x00, 0x00 // push 0xC6
	};

	WritePatch(0x3EED1, &adjustPurchaseTitleXPos, 5);

	// Disable rendering of unnecessary info
	NOP(0x3EFBA, 5); // Name of trade-in item
	NOP(0x3F029, 5); // Health bar of trade-in item
	NOP(0x3F0DF, 5); // "Trade" string 
	NOP(0x3F11D, 5); // "Cost" string 
	NOP(0x3F170, 5); // "Trade" value
	NOP(0x3F1CC, 5); // "Cost" value
	
	// Disable swapping models on purchase
	NOP(0x4091A, 5);

	// Disable copy entry from replacement data to shop data
	NOP(0x40891, 2); // obsolete?

	// Sets trade cost
	// +3EBC1
	// mov 0 works
	char disableTradeValue[13] = {
		0xC7, 0x05, 0x5C, 0x93, 0xE9, 0x00, 0x00, 0x00, 0x00, 0x00, // mov [SWEP1RCR.EXE+A9935C],0
		0x90, 0x90, 0x90                                            // NOP
	};

	WritePatch(0x3EBC1, &disableTradeValue, 13);

	//Item shop
	//3EB6D
	//Gets cost of part for shop

	//56017
	//Gets model of item to replace (call can be jumped)

	// Overwrite functions to allow part type values out of range
	// The display for the right side is being hidden anyway so it's fine to make these hardcoded
	// 3EF60 index for name
	char tradeInName[7] = {
		0xBE, 0x00, 0x00, 0x00, 0x00, // mov esi, 0
		0x90, 0x90                    // nop
	};

	// 56017 index for model
	char tradeInModel[7] = {
		0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, 0
		0x90, 0x90                    // nop
	};

	char* tradeInIndex = tradeInModel;

	/*WritePatch(0x3EF60, &tradeInName, 7);
	WritePatch(0x56017, &tradeInModel, 7);
	WritePatch(0x3EB6D, &tradeInIndex, 7);*/

	// todo: adapt this
	//37862
	//Sets part stat display in bottom left (can be invalid)
	// call +5cf60
	// cdecl (ecx, eax, edx) (?, podPartType, ?) (9, 2, 1)
	
	//->3EF80
	//Gets name of item to replace
}

void __declspec(naked) MarkPitDroidPurchaseWrapper()
{
	__asm
	{
		pushad;
		call SWRGame::MarkPitDroidPurchase;
		popad;
		ret;
	}
}

void Patches::HookDroidShop()
{
	SWRGame::Log("Applying patch: Hook Droid Shop");
	std::vector<int> offsets = {
		0x58DDE,
		0x58E09,
		0x59815,
		0x598DB,
		0x3771A,
		0x36A37
	};

	void* redirect = &SWRGame::progress.pitDroidCounter;

	for (auto offset : offsets)
		WritePatch(offset, &redirect, 4);

	HookFunction(0x37B88, &MarkPitDroidPurchaseWrapper, 0);
}

void Patches::DisableJunkyard()
{
	SWRGame::Log("Applying patch: Disable Junkyard");
	NOP(0x36A2C, 9);
}

void __fastcall WritePlanetsVisited(int offset)
{
	int* planetsVisitedAddr = (int*)(SWRGame::baseAddress + 0xA35A9C + offset);
	*planetsVisitedAddr = -1;
}

void __fastcall WriteAPPartialSeed(int offset)
{
	uint64_t* seedAddr = (uint64_t*)(SWRGame::baseAddress + 0xA35A78 + offset);
	*seedAddr = SWRGame::partialSeed;
}

void __declspec(naked) APSavePatch()
{
	__asm
	{
		pushad;
		mov ecx, eax;
		call WritePlanetsVisited;
		call WriteAPPartialSeed;
		popad;
		ret;
	}
}

void Patches::HookSaveFiles()
{
	SWRGame::Log("Applying patch: Hook Save Files");
	
	// Default racers to 0
	char defaultRacers[10] = {
		0xC7, 0x80, 0x94, 0x5A, 0xE3, 0x00, 0x00, 0x00, 0x00, 0x00 // mov [eax + E35A94], 0
	};

	WritePatch(0x3EADC, &defaultRacers, 10);

	// Overwrite planet cutscenes
	HookFunction(0x3EAE6, &APSavePatch, 1);

	// Lock semi-pro and galactic circuits
	// Just change a cl to a bl to lock both like invitational
	char blOpcode = 0x98;
	WritePatch(0x3EB13, &blOpcode, 1); // semi-pro
	WritePatch(0x3EB19, &blOpcode, 1); // galactic

	// Limit name to 24 bytes to avoid overwriting partial seed
	char limitName[5] = {
		0xB9, 0x06, 0x00, 0x00, 0x00 // mov ecx, 6
	};

	WritePatch(0x3EB41, &limitName, 5);
}

void __declspec(naked) MarkRaceCompletionWrapper()
{
	// eax = circuit index
	// edx = course index
	__asm
	{
		pushad;
		mov ecx, eax; // __fastcall reads from ecx
		call SWRGame::MarkRaceCompletion;
		popad;
		ret;
	}
}

void Patches::HookRaceRewards()
{
	SWRGame::Log("Applying patch: Hook Race Rewards");
	
	HookFunction(0x3A363, &MarkRaceCompletionWrapper, 3);

	// Our hook hijacks the same args as this call, so we'll overwrite it and rewrite it 8 bytes later
	char pushedOriginalCall[8] = {
		0xE8, 0xB0, 0x66, 0x00, 0x00, // call SWEP1RCR.EXE+40A20
		0x83, 0xC4, 0x08              // add esp, 08
	};

	WritePatch(0x3A36B, &pushedOriginalCall, 8);

	// Force reward to be from "Fair" payout pool
	char giveFairReward[7] = {
		0xB9, 0x01, 0x00, 0x00, 0x00, // mov ecx, 1
		0x90, 0x90                    // nop
	};

	WritePatch(0x3A373, &giveFairReward, 7);
}

void Patches::DisableAwardsCeremony()
{
	SWRGame::Log("Applying patch: Disable Awards Ceremony");
	char alwaysSkip[2] = {
		0xEB, 0x27 // jmp SWEP1RCR.EXE + 3AA89
	};

	WritePatch(0x3AA60, &alwaysSkip, 2);

	// todo: prevent credits on Galactic completion
}

void Patches::DisableVanillaInvitationalUnlocks()
{
	SWRGame::Log("Applying patch: Disable Vanilla Invitational Unlocks");
	NOP(0x3A40B, 5);
}

typedef void(__fastcall* _SetAIDifficulty)();
_SetAIDifficulty SetAIDifficulty;

void __fastcall AIScaleByCircuit()
{
	GameStatus** statusPtr = (GameStatus**)(SWRGame::baseAddress + 0xBFDB8);
	GameStatus* gameStatus = *statusPtr;
	float* aiMultiplier = (float*)(SWRGame::baseAddress + 0xC707C);
	int vanillaCircuit = courseIdToVanillaCircuit[gameStatus->selectedCourseId];
	if (vanillaCircuit != gameStatus->selectedCircuit)
	{
		float value = SWRGame::GetAIScaleByCircuit(*aiMultiplier, vanillaCircuit, gameStatus->selectedCircuit);
		*aiMultiplier = value;
	}
}

void __fastcall AIScaleFromParts()
{
	float* aiMultiplier = (float*)(SWRGame::baseAddress + 0xC707C);
	*aiMultiplier = SWRGame::GetAIScaleFromParts();
}

void __declspec(naked) SetAIDifficultyWrapper()
{
	__asm
	{
		mov edx,[eax+0x1AC]; // vanilla call
		pushad;
		call SetAIDifficulty;
		popad;
		ret;
	}
}

void Patches::ScaleAIDifficulty(int option)
{
	SWRGame::Log("Applying patch: Scale AI Difficulty");
	if (option == 1)
		SetAIDifficulty = AIScaleByCircuit;
	else
		SetAIDifficulty = AIScaleFromParts;

	HookFunction(0x66ABD, &SetAIDifficultyWrapper, 1);
}

// 8DF30 is render func
// +17FF writes string on profile page?

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
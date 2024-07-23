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
void Patches::DisablePartDamage()
{
	SWRGame::Log("Applying patch: Disable Part Damage");
	char forceUnupgraded[6] = {
		0xB1, 0x00, // mov cl, 00
		0x90,       // nop
		0x90,       // nop
		0x90,       // nop
		0x90,       // nop
	};

	WritePatch(DAMAGE_APPLY_OPCODE, &forceUnupgraded, 6);
}

int GetFirstAvailableInvitational()
{
	int flag = 0;
	int invUnlocks = (int)SWRGame::progress.swrSaveData->trackUnlocks[3];
	for (int i = 0; i < 4; i++)
	{
		flag = 1 << i;
		if ((invUnlocks & flag) != 0)
			return i;
	}

	return -1;
}

void __fastcall HandleCircuitChange(int nextCircuit, int structPtr)
{
	char* selectedCircuit = (char*)(structPtr + 0x5E);
	char nextCircuitUnlocks;
	int* cursor = (int*)(SWRGame::baseAddress + 0xA295D0);

	if ((nextCircuit > *selectedCircuit) && (*selectedCircuit < 3))
	{
		while (nextCircuit < 3)
		{

			nextCircuitUnlocks = SWRGame::progress.swrSaveData->trackUnlocks[nextCircuit];
			if ((nextCircuitUnlocks & 0x01) != 0)
			{
				*selectedCircuit = nextCircuit;
				*cursor = 0; // Force selection of the first course on change
				break;
			}
			nextCircuit++;
		}

		if (nextCircuit == 3)
		{
			int invAvailable = GetFirstAvailableInvitational();
			if (invAvailable != -1)
			{
				*selectedCircuit = nextCircuit;
				*cursor = 0; // Index zero is always the first selectable course on this screen
			}
		}
	}
	else if ((nextCircuit < *selectedCircuit) && (*selectedCircuit > 0))
	{
		while (nextCircuit >= 0)
		{
			nextCircuitUnlocks = SWRGame::progress.swrSaveData->trackUnlocks[nextCircuit];
			if ((nextCircuitUnlocks & 0x01) != 0)
			{
				*selectedCircuit = nextCircuit;
				*cursor = 0; // Force selection of the first course on change
				break;
			}
			nextCircuit--;
		}
	}

}

// call at +3B70C and +3B741
void __declspec(naked) HandleCircuitChangeWrapper()
{
	__asm
	{
		pushad;
		mov edx, esi;
		xor ecx, ecx;
		mov cl, al;
		call HandleCircuitChange;
		popad;

		push eax;
		mov eax, 0x00E295D4;
		mov [eax], ebp;
		pop eax;
		ret;
	}
}

void __fastcall CapCursor()
{
	int* cursor = (int*)(SWRGame::baseAddress + 0xA295D0);
	int numItems= *(int*)(SWRGame::baseAddress + 0xA295CC) - 1;
	++*cursor;

	if (*cursor > numItems)
		*cursor = numItems;
}

void __declspec(naked) CapCursorWrapper()
{
	__asm
	{
		pushad;
		call CapCursor;
		popad;
		ret;
	}
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
// Skip locked circuits and select first course on changing circuits
void Patches::FixCourseSelection()
{
	SWRGame::Log("Applying patch: Fix Course Selection");
	char forceSkipErrorCursor[2] = {
		0xEB, 0x05 // jmp 5
	};

	WritePatch(ERROR_CURSOR_OPCODE, &forceSkipErrorCursor, 2);

	//HookFunction(DEFAULT_FIRST_COURSE_INJECT, &DefaultToFirstCourse, 2);
	HookFunction(0x3B70C, &HandleCircuitChangeWrapper);
	HookFunction(0x3B741, &HandleCircuitChangeWrapper);

	// Replace push instruction we overwrote
	char push[6] = {
		0x6A, 0x58,            // push 0x58
		0x90, 0x90, 0x90, 0x90 // nop
	};

	WritePatch(0x3B711, &push, 6);
	WritePatch(0x3B746, &push, 6);

	// Cap the cursor on course selection screen
	HookFunction(0x3B219, &CapCursorWrapper, 1);
}

void __declspec(naked) SkipAcquiredItems()
{
	// Vanilla behavior subtracts 3 from the total unlocked courses to account for the defaults
	// In AP we'll manually count actual completed courses
	// Mask ecx to ensure a correct cmp result
	__asm
	{
		or ecx, SWRGame::shopDisplayMask;
		xor ecx, SWRGame::shopDisplayMask;
		push ecx;
		push edx;
		call SWRGame::CalculateRacesCompleted;
		mov eax, edx;
		pop edx;
		pop ecx;
		ret;
	}
}

void __declspec(naked) MarkShopPurchaseWrapper()
{
	__asm
	{
		pushad;
		call SWRGame::MarkShopPurchase;
		popad;
		ret;
	}
}

void __declspec(naked) FullShopDrawWrapper()
{
	__asm
	{
		pushad;
		call SWRGame::DrawEvents::OnDrawShopFullView;
		popad;
		ret;
	}
}

void __declspec(naked) ShopBuyWindowDrawWrapper()
{
	__asm
	{
		pushad;
		call SWRGame::DrawEvents::OnDrawShopBuyView;
		popad;
		ret;
	}
}

void __declspec(naked) HookShopDrawStats()
{
	__asm
	{
		pushad;
		mov ecx, ebp;
		call SWRGame::DrawEvents::OnDrawShopStats;
		call SWRGame::UpdateProgressiveDisplays;
		popad;
		ret;
	}
}

void __declspec(naked) LoadItemModelWrapper()
{
	__asm
	{
		pushad;
		call SWRGame::LoadItemModel;
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

	// Stop game from skipping what it thinks are owned parts
	NOP(0x3E8EA, 2);

	// Intercept vanilla call to swap owned and shop items
	// We will have to manually deduct money in our function
	HookFunction(0x37C14, &MarkShopPurchaseWrapper);

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
	NOP(0x56061, 5); // Draws current pod part model
	
	// Disable swapping models on purchase
	NOP(0x4091A, 5);

	// Disable copy entry from replacement data to shop data
	NOP(0x40891, 2); // obsolete?

	// Sets trade cost
	// +3EBC1
	char disableTradeValue[13] = {
		0x89, 0x0D, 0x20, 0x92, 0xE9, 0x00,      // mov [SWEP1RCR.EXE+A99220], ecx
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 // NOP
	};

	WritePatch(0x3EBC1, &disableTradeValue, 13);

	NOP(0x3EBF5, 6); // originally set +A99220 with calculated value

	// Redirect shop functions to use AP shop
	// Leave vanilla item table intact
	//
	// Item names
	HookFunction(0x3797D, &FullShopDrawWrapper);
	HookFunction(0x3ECFB, &ShopBuyWindowDrawWrapper);

	// Model
	void* apShopDataModelPtr = &SWRGame::apShopData.entries[0].modelId;
	WritePatch(0x3E9A4, &apShopDataModelPtr, 4);

	// Races needed - used in flag checking
	void* apShopDataRacesPtr = &SWRGame::apShopData.entries[0].requiredRaces;
	WritePatch(0x3E8CA, &apShopDataRacesPtr, 4);

	// Item type
	void* apShopDataTypePtr = &SWRGame::apShopData.entries[0].itemType;
	WritePatch(0x37865, &apShopDataTypePtr, 4);
	WritePatch(0x3EB70, &apShopDataTypePtr, 4);
	WritePatch(0x3779B, &apShopDataTypePtr, 4);

	// Series Id
	void* apShopDataSeriesPtr = &SWRGame::apShopData.entries[0].seriesId;
	WritePatch(0x3785E, &apShopDataSeriesPtr, 4);

	// Stats window render
	// +378E1 - call +550D0
	HookFunction(0x378D6, &HookShopDrawStats, 14);

	// Handle item models and stat draw
	// Progressive items need to calculate what the next item should be
	// and display the correct model and stat for it
	// On loading the shop we will calculate this
	HookFunction(0x3E9AC, &LoadItemModelWrapper);
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

void __fastcall InitAPSave(int offset)
{
	SWR_SaveData* saveData = (SWR_SaveData*)(SWRGame::baseAddress + 0xA35A60 + offset);
	saveData->cutscenesBitfield = 0xFFFFFFFF;
	saveData->apPartialSeed = SWRGame::partialSeed;
	saveData->progressivePasses = 0;
	saveData->racesCompleted = 0;
}

void __declspec(naked) APSavePatch()
{
	__asm
	{
		pushad;
		mov ecx, eax;
		call InitAPSave;
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

	// Limit name to 20 bytes to avoid overwriting partial seed and race completion data
	char limitName[5] = {
		0xB9, 0x05, 0x00, 0x00, 0x00 // mov ecx, 5
	};

	WritePatch(0x3EB41, &limitName, 5);

	// Limit name entry to 16 characters
	char limitNameEntry[6] = {
		0xB8, 0x10, 0x00, 0x00, 0x00, // mov eax, 0x10
		0x90                          // nop
	};

	WritePatch(0x1838B, &limitNameEntry, 6);
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

	// Disable user ability to change the payout setting to minimize confusion
	NOP(0x3C40E, 6);
	NOP(0x3C4C1, 6);
}

void Patches::DisableAwardsCeremony()
{
	SWRGame::Log("Applying patch: Disable Awards Ceremony");
	char alwaysSkip[2] = {
		0xEB, 0x27 // jmp SWEP1RCR.EXE + 3AA89
	};

	WritePatch(0x3AA60, &alwaysSkip, 2);

	// Prevent credits sequence
	NOP(0x3A497, 5);
}

void Patches::DisableVanillaInvitationalUnlocks()
{
	SWRGame::Log("Applying patch: Disable Vanilla Invitational Unlocks");
	NOP(0x3A40B, 5);
	NOP(0x3A53D, 5); // Prevents writing the unlock
	NOP(0x3A402, 5); // Prevents auto selecting the invitational circuit after finishing amateur or semi-pro circuits
	NOP(0x3A538, 5); // Prevents auto selecting the invitational circuit after finishing galactic circuit
}

void Patches::DisableVanillaCourseUnlocks()
{
	SWRGame::Log("Applying patch: Disable Vanilla Course Unlocks");
	NOP(0x3A461, 6);
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

void __fastcall SetAPString(int menuId, TextPtrStructure* textPtr)
{
	if (menuId == 0x0B) // Main menu
	{
		SWRGame::Log("Set AP string");
		const char* newString = "~sArchipelago Mode";
		memcpy(textPtr->text, newString, 19);
	}

	// Do vanilla operation
	int* vanillaPtr = (int*)(SWRGame::baseAddress + 0xD8798);
	*vanillaPtr = (int)textPtr;
}

void __declspec(naked) SetAPStringWrapper()
{
	__asm
	{
		pushad;
		mov edx, esi;
		call SetAPString;
		popad;
		ret;
	}
}

void Patches::SetAPModeString()
{
	HookFunction(0x1593C, &SetAPStringWrapper, 1);
}

void __declspec(naked) SetLapsToOne()
{
	__asm
	{
		mov[edi + 0x1C8], 1;
		ret;
	}
}

void Patches::EnableOneLapMode()
{
	SWRGame::Log("Applying patch: Enable 1-Lap Mode");

	// Disable setting 3 lap record
	char disableRecord[2]{
		0xEB, 0x3C // jmp SWREP1RCR.EXE + 3A129
	};

	WritePatch(0x3A0EB, &disableRecord, 2);

	HookFunction(0x63ADE, &SetLapsToOne, 1);
}

void __fastcall SetCourse(int raceSettingsStruct, int index)
{
	bool* mirroredSetting = (bool*)(raceSettingsStruct + 0x6E);
	*mirroredSetting = SWRGame::courseLayout[index].mirrored;

	char* courseID = (char*)(raceSettingsStruct + 0x5D);
	int* mappingAddr = (int*)(0x4C0018 + 4 * index);
	*courseID = (char)*mappingAddr;
}

void __declspec(naked) SetCourseHook()
{
	__asm
	{
		pushad;
		mov ecx, esi;
		call SetCourse;
		popad;
		ret;
	}
}

void Patches::EnableMirroredCourses()
{
	SWRGame::Log("Applying patch: Enable Mirrored Courses");
	HookFunction(0x3B380, &SetCourseHook, 5);
}

// 8DF30 is render func
// +17FF writes string on profile page?

typedef void(__cdecl* _RenderTexture)(int a, void* b);
_RenderTexture OriginalRenderTexture;
const DWORD oRenderTextureCallOffset = 0x8DD28;

void __cdecl HookedRenderTexture(int a, void* b)
{
	__asm pushad;
	SWRGame::DrawEvents::OnDraw();
	__asm popad;
	OriginalRenderTexture(a, b);
}

void __declspec(naked) courseSelectHook()
{
	__asm
	{
		pushad;
		call SWRGame::DrawEvents::OnDrawCourseSelect;
		popad;
		movsx eax, byte ptr[esi + 0x5D]; 
		lea eax, [eax + eax * 2];
		shl eax, 02;
		ret;
	}
}

void __declspec(naked) courseInfoHook()
{
	__asm
	{
		pushad;
		call SWRGame::DrawEvents::OnDrawTrackInfo;
		popad;
		mov ebp, 0xA0;
		ret;
	}
}

void __declspec(naked) preRaceMenuHook()
{
	__asm
	{
		pushad;
		call SWRGame::DrawEvents::OnDrawPreRaceMenu;
		popad;
		movsx eax, byte ptr[edi + 0x50C308]
		ret;
	}
}

void __declspec(naked) inRaceHook()
{
	__asm
	{
		pushad;
		call SWRGame::DrawEvents::OnDrawInRace;
		popad;
		mov edx, [0xE27820];
		ret;
	}
}

void Patches::HookDraw()
{
	OriginalRenderTexture = (_RenderTexture)(SWRGame::baseAddress + 0x8DF30);
	HookFunction(oRenderTextureCallOffset, &HookedRenderTexture, 0);
	HookFunction(0x3B3CA, &courseSelectHook, 5);
	HookFunction(0x3B9DB, &courseInfoHook);
	HookFunction(0x36A8C, &preRaceMenuHook, 2);
	HookFunction(0x6096E, &inRaceHook, 1);
}

int prevInput = 0;

void __fastcall ProcessInput(int input, int port)
{
	if ((port > 0) || (input == prevInput))
		return;

	// Left input
	if ((input & 0x00010000) != 0)
		SWRGame::ChangeAIModifier(-0.005);

	// Right input
	if ((input & 0x00020000) != 0)
		SWRGame::ChangeAIModifier(0.005);

	prevInput = input;
}

void __declspec(naked) ProcessInputWrapper()
{
	__asm
	{
		mov eax, [esi*4 + 0x50C908];
		pushad;
		mov ecx, eax;
		mov edx, esi;
		call ProcessInput;
		popad;
		ret;
	}
}

void Patches::HookInput()
{
	HookFunction(0x5A6EF, &ProcessInputWrapper, 2);
}
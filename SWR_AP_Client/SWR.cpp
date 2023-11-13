#include "SWR.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <random>
#include <chrono>

SWRGame::SWRGame()
{
	baseAddress = (int)GetModuleHandleA("SWEP1RCR.EXE");
	
	// Make default characters writeable
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery((LPCVOID)(baseAddress + DEFAULT_RACERS_OPCODE), &mbi, sizeof(mbi));
	DWORD oldProtect;
	VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtect);

	// Limits character selection to a random racer
	void* unlocks = (void*)(baseAddress + DEFAULT_RACERS_OPCODE);
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen(seed);
	std::uniform_int_distribution<int> distribution(0, 22);
	int random = distribution(gen);
	int randomSelection = 1 << random;

	char newOPCode[6] = { 0xBE, 0x00, 0x00, 0x00, 0x00, 0x90 };
	memcpy(&newOPCode[1], &randomSelection, sizeof(randomSelection));
	memcpy(unlocks, &newOPCode, 6);
}
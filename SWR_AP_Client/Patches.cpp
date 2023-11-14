#include "Patches.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "SWR.h"

void Patches::SetBaseAddress(int addr)
{
	cachedBaseAddress = addr;
}

void Patches::MakePageWritable(int offset)
{
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery((LPCVOID)(offset), &mbi, sizeof(mbi));
	DWORD oldProtect;
	VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtect);
}

void Patches::LimitAvailableRacers(RacerUnlocks racers)
{
	int funcPtr = cachedBaseAddress + DEFAULT_RACERS_OPCODE;

	MakePageWritable(funcPtr);
	char newOPCode[6] = { 
		0xBE,                   // mov esi
		0x00, 0x00, 0x00, 0x00, // bitfield, overwritten by racers value
		0x90                    // nop
	};                 

	memcpy(&newOPCode[1], &racers, sizeof(racers));
	memcpy((void*)(funcPtr), &newOPCode, 6);
}
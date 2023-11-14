#include "SWR.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <random>
#include <chrono>

SWRGame::SWRGame()
{
	baseAddress = (int)GetModuleHandleA("SWEP1RCR.EXE");
	Patches::SetBaseAddress(baseAddress);
	
	// Temporary, for testing purposes
	// Limit character selection to a random racer
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen(seed);
	std::uniform_int_distribution<int> distribution(0, 22);
	int random = distribution(gen);
	int randomSelection = 1 << random;

	Patches::LimitAvailableRacers((RacerUnlocks)randomSelection);
}

void SWRGame::KillPod() 
{
	// Function at SWE1RCR.EXE + 0x74970 checks this flag and destroys the pod if it is set

	PodData* podDataAddr = *(PodData**)(baseAddress + POD_DATA_PTR_OFFSET);
	podDataAddr->status |= PodStatus::Destroyed;
}
#pragma once
#include "Enums.h"

#define DEFAULT_RACERS_OPCODE 0x3DA37

static class Patches
{
private:
	static void MakePageWritable(int offset);
	static inline int cachedBaseAddress;
public:
	static void SetBaseAddress(int addr);
	static void LimitAvailableRacers(RacerUnlocks racers);
};
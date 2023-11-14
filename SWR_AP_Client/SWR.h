#pragma once

#include "Enums.h"
#include "Structs.h"
#include "Archipelago.h"

#define POD_PARTS_OFFSET 0xA35AA1
#define DEFAULT_RACERS_OPCODE 0x3DA37

class SWRGame
{
private:
	int baseAddress;

public:
	SWRGame();
};


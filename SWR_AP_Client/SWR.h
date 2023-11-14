#pragma once

#include "Enums.h"
#include "Structs.h"
#include "Archipelago.h"
#include "Patches.h"

#define POD_PARTS_OFFSET 0xA35AA1
#define POD_DATA_PTR_OFFSET 0xD78A8

class SWRGame
{
private:
	int baseAddress;
public:
	SWRGame();
	void KillPod();
};


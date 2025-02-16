#pragma once
#include "Structs.h"

namespace SWRGame
{
	float GetAIScaleFromParts();
	float GetAIScaleByCircuit(float val, int baseCircuit, int targetCircuit);
	float GetAIScaleVanilla(float val);
	void ChangeAIModifier(float amount);

	float aiModifier = 1.0f;
	bool modifierControl = false;

	extern int* menuVal;
	extern int* menuValB;
	extern SWR_SaveData* swrSaveData;
	
	extern void Log(const char* format, ...);
}
#pragma once
#include "Structs.h"

namespace SWRGame
{
	float GetAIScaleFromParts();
	float GetAIScaleByCircuit(float val, int baseCircuit, int targetCircuit);
	void ChangeAIModifier(float amount);

	float aiModifier = 1.0f;
	bool modifierControl = false;

	extern int* menuVal;
	extern int* menuValB;
	extern AP_ProgressData progress;
	
	extern void Log(const char* format, ...);
}
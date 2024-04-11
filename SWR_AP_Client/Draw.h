#pragma once
#include "Enums.h"
#include "Structs.h"

typedef void(__cdecl* _WriteText)(short xPos, short yPos, int r, int g, int b, int a, const char* text, int unk_00, int unk_01);

namespace SWRGame
{
	_WriteText WriteText;
	void OnDraw();
	Color GetColorFromEnum(SWRTextColor textColor);
	void WriteTextWrapper(std::string string, SWRFont font, int x, int y, SWRTextColor color, SWRTextAlign align);
	
	extern SWRGameState gamestate;
	extern int baseAddress;
	extern std::vector <NotifyMsg> notifyQueue;
	extern bool isPlayerInRace();
	extern float aiModifier;
	extern bool modifierControl;

	extern int* menuVal;
	extern int* menuValB; 
	extern std::string fullSeedName;
	extern std::string versionString;
}
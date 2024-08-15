#pragma once
#include "Enums.h"
#include "Structs.h"
#include <map>

typedef void(__cdecl* _WriteText)(short xPos, short yPos, int r, int g, int b, int a, const char* text, int unk_00, int unk_01);
typedef void(__cdecl* _DrawStats)(void* obj, float x, float y);

#define MAX_NOTIFICATIONS 5

namespace SWRGame
{
	_WriteText WriteText;
	Color GetColorFromEnum(SWRTextColor textColor);
	void WriteTextWrapper(std::string string, SWRFont font, int x, int y, SWRTextColor color, SWRTextAlign align);

	void UpdateShopModel(int selectedIndex, int modelId);

	bool displayAPMenu = false;
	void DrawAPMenu();
	
	extern SWRGameState gamestate;
	extern int baseAddress;
	extern std::vector <NotifyMsg> notifyQueue;
	extern bool isPlayerInRace();
	extern float aiModifier;
	extern bool modifierControl;
	extern SWR_PodPartTable apShopData;
	extern int GetShopItemOffset();
	extern std::map<int, AP_WattoEntry> wattoShopEntries;
	extern void Log(const char* format, ...);
	extern _DrawStats DrawStats;
	extern bool __fastcall isItemPodPart();
	extern AP_ProgressData progress;
	extern CourseUnlockMode courseUnlockMode;
	extern bool progressiveCircuits;

	extern int* menuVal;
	extern int* menuValB; 
	extern std::string fullSeedName;
	extern std::string versionString;
}

namespace SWRGame::DrawEvents
{
	void __fastcall OnDraw();
	void __fastcall OnDrawCourseSelect();
	void __fastcall OnDrawTrackInfo();
	void __fastcall OnDrawPreRaceMenu();
	void __fastcall OnDrawShopFullView();
	void __fastcall OnDrawShopBuyView();
	void __fastcall OnDrawShopStats(void* obj);
	void __fastcall OnDrawInRace();
}
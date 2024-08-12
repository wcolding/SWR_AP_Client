#include "Draw.h"
#include <chrono>
#include "Locations.h"

namespace SWRGame 
{
	Color GetColorFromEnum(SWRTextColor textColor)
	{
		Color newColor;
		newColor.r = ((int)textColor & 0xFF000000) >> 24;
		newColor.g = ((int)textColor & 0x00FF0000) >> 16;
		newColor.b = ((int)textColor & 0x0000FF00) >> 8;
		newColor.a = ((int)textColor & 0x000000FF);
		return newColor;
	}

	void WriteTextWrapper(std::string string, SWRFont font, int x, int y, SWRTextColor color = SWRTextColor::White, SWRTextAlign align = SWRTextAlign::Left)
	{
		string = "~s" + string;

		switch (align)
		{
		case SWRTextAlign::Center:
			string = "~c" + string;
			break;
		case SWRTextAlign::Right:
			string = "~r" + string;
			break;
		default:
			break;
		}

		switch (font)
		{
		case SWRFont::ShopItem:
			string = "~f4" + string;
			break;
		case SWRFont::Medium:
			string = "~F5" + string;
			break;
		case SWRFont::Large:
			string = "~F6" + string;
			break;
		default:
			break;
		}

		Color col = GetColorFromEnum(color);

		WriteText(x, y, col.r, col.g, col.b, col.a, string.c_str(), -1, 0);
	}

	AP_WattoEntry cachedEntry;

	AP_WattoEntry* GetItemEntry()
	{
		int index = GetShopItemOffset();

		for (auto offsetMapping : wattoShopLocationToOffset)
		{
			if (index == offsetMapping.second)
				if (wattoShopEntries.contains(offsetMapping.first))
					return &wattoShopEntries[offsetMapping.first];
		}

		return nullptr;
	}

	std::map<AP_ItemType, SWRTextColor> typeColorMap
	{
		{ AP_ItemType::Filler, SWRTextColor::AP_FillerItem },
		{ AP_ItemType::Progression, SWRTextColor::AP_ProgressionItem },
		{ AP_ItemType::Useful, SWRTextColor::AP_UsefulItem }
	};

	std::map<AP_ItemType, std::string> typeTextMap
	{
		{ AP_ItemType::Filler, "Filler" },
		{ AP_ItemType::Progression, "Progression" },
		{ AP_ItemType::Useful, "Useful" }
	};

	typedef int(__cdecl* _LoadModel)(int modelId);
	_LoadModel LoadModel = (_LoadModel)(0x448780);

	void UpdateShopModel(int selectedIndex, int modelId)
	{
		int modelAddr = LoadModel(modelId);
		int tableOffset = (98 + selectedIndex) * 4;

		int* modelTableAddrA = (int*)(SWRGame::baseAddress + 0xA29900 + tableOffset);//900
		*modelTableAddrA = modelAddr;

		int* modelTableAddrB = (int*)(SWRGame::baseAddress + 0xA29160 + tableOffset);//160

		int* modelDataPtr = (int*)*modelTableAddrA;
		*modelTableAddrB = *modelDataPtr;
	}

	void __fastcall DrawEvents::OnDrawShopFullView()
	{
		AP_WattoEntry* entry = GetItemEntry();
		if (entry == nullptr)
			return;

		WriteTextWrapper(entry->displayName, SWRFont::Default, 160, 25, typeColorMap[entry->itemType], SWRTextAlign::Center);
	}
	
	void __fastcall DrawEvents::OnDrawShopBuyView()
	{
		AP_WattoEntry* entry = GetItemEntry();
		if (entry == nullptr)
			return;

		WriteTextWrapper(entry->displayName, SWRFont::ShopItem, 86, 23, typeColorMap[entry->itemType], SWRTextAlign::Center);

		int itemTableOffset = GetShopItemOffset();

		for (auto tableEntry : wattoShopLocationToOffset)
		{
			if (tableEntry.second == itemTableOffset)
			{
				std::string checkString = locationTable[tableEntry.first];
				checkString.erase(0, 15);
				WriteTextWrapper(checkString, SWRFont::ShopItem, 238, 41, SWRTextColor::LightBlue, SWRTextAlign::Center);
				WriteTextWrapper(typeTextMap[entry->itemType], SWRFont::ShopItem, 238, 102, typeColorMap[entry->itemType], SWRTextAlign::Center);

				if ((apShopData.entries[itemTableOffset].requiredRaces & 0x80) != 0)
					WriteTextWrapper("SOLD OUT", SWRFont::Large, 470, 150, SWRTextColor::Red, SWRTextAlign::Center);
			}
		}
	}

	std::chrono::steady_clock::time_point prevTime;
	bool inRace;

	bool trackMirrored()
	{
		int settingsAddr = *(int*)(baseAddress + 0xBFDB8);
		bool mirrored = *(bool*)(settingsAddr + 0x6E);
		return mirrored;
	}

	void DrawNotifyQueue()
	{
		auto curTime = std::chrono::steady_clock::now();

		if (!notifyQueue.empty())
		{
			if (notifyQueue[0].timeRemaining <= 0)
				notifyQueue.erase(notifyQueue.begin());
			else
			{
				const std::chrono::duration<float> deltaTime = curTime - prevTime;
				notifyQueue[0].timeRemaining -= deltaTime.count();
				for (int i = 0; i < MAX_NOTIFICATIONS; i++)
				{
					if (notifyQueue.size() > i)
						WriteTextWrapper(notifyQueue[i].msg, SWRFont::ShopItem, 10, (i * 10) + 10, SWRTextColor::White);
				}
			}
		}

		prevTime = curTime;
	}

	void __fastcall DrawEvents::OnDraw()
	{
		if (isPlayerInRace())
			return;

		DrawNotifyQueue();

		// Connection status
		if (gamestate < SWRGameState::AP_Authenticated)
			WriteTextWrapper("Not connected to AP", SWRFont::Medium, 625, 20, SWRTextColor::Red, SWRTextAlign::Right);
		else
			WriteTextWrapper("Connected to AP", SWRFont::Medium, 625, 20, SWRTextColor::LightBlue, SWRTextAlign::Right);

		// Menu specific draws
		if (menuVal != nullptr)
		{
			switch (*menuVal)
			{
			case 1: // Start Menu
				WriteTextWrapper(versionString, SWRFont::Medium, 625, 0, SWRTextColor::LightBlue, SWRTextAlign::Right);
				WriteTextWrapper(fullSeedName, SWRFont::Medium, 625, 10, SWRTextColor::Yellow, SWRTextAlign::Right);
				break;
			case 2: // Profile select
				WriteTextWrapper("IMPORTANT : Create a new save for each new seed!", SWRFont::Medium, 310, 120, SWRTextColor::LightBlue, SWRTextAlign::Center);
				break;
			default:
				break;
			}
		}
	}

	void __fastcall DrawEvents::OnDrawShopStats(void* obj)
	{
		if (SWRGame::isItemPodPart())
			SWRGame::DrawStats(obj, 20.0, 135.0);
		else
		{
			WriteTextWrapper("Stats will appear here", SWRFont::ShopItem, 100, 155, SWRTextColor::LightBlue, SWRTextAlign::Center);
			WriteTextWrapper("for pod parts only", SWRFont::ShopItem, 100, 165, SWRTextColor::LightBlue, SWRTextAlign::Center);
		}
	}

	int unlocksTextX = 150;
	int unlocksTextY = 320;

	void DrawCircuitAvailable(std::string name, Circuit circuit, int yOffset)
	{
		std::string formatted = name + ": ";
		bool available = false;

		if (progressiveCircuits)
			available = progress.progressivePasses >= (int)circuit;
		else
		{
			switch (circuit)
			{
			case Circuit::SemiPro:
				available = progress.hasSemiProPass;
				break;
			case Circuit::Galactic:
				available = progress.hasGalacticPass;
				break;
			case Circuit::Invitational:
				available = progress.hasInvitationalPass;
				break;
			default:
				break;
			}
		}

		formatted += available ? "open" : "closed";
		SWRTextColor color = available ? SWRTextColor::Green : SWRTextColor::Red;
		WriteTextWrapper(formatted, SWRFont::Medium, unlocksTextX, unlocksTextY + yOffset, color, SWRTextAlign::Left);
	}

	void __fastcall DrawEvents::OnDrawCourseSelect()
	{
		if (trackMirrored())
			WriteTextWrapper("Mirrored", SWRFont::ShopItem, 160, 75, SWRTextColor::Yellow, SWRTextAlign::Center);

		switch (courseUnlockMode)
		{
		case CourseUnlockMode::CircuitPassInvitational:
			DrawCircuitAvailable("Invitational", Circuit::Invitational, 30);
			[[fallthrough]];
		case CourseUnlockMode::CircuitPassNoInv:
			WriteTextWrapper("Circuits available:", SWRFont::Medium, unlocksTextX, unlocksTextY, SWRTextColor::LightBlue, SWRTextAlign::Left);
			DrawCircuitAvailable("Semi-Pro", Circuit::SemiPro, 10);
			DrawCircuitAvailable("Galactic", Circuit::Galactic, 20);
			break;
		case CourseUnlockMode::Shuffle:
			WriteTextWrapper("Course Unlocks:", SWRFont::Medium, unlocksTextX, unlocksTextY, SWRTextColor::LightBlue, SWRTextAlign::Left);
			WriteTextWrapper("Amateur: " + std::to_string(progress.amateurUnlocks), SWRFont::Medium, unlocksTextX, unlocksTextY + 10, SWRTextColor::Green, SWRTextAlign::Left);
			WriteTextWrapper("Semi-Pro: " + std::to_string(progress.semiProUnlocks), SWRFont::Medium, unlocksTextX, unlocksTextY + 20, SWRTextColor::Green, SWRTextAlign::Left);
			WriteTextWrapper("Galactic: " + std::to_string(progress.galacticUnlocks), SWRFont::Medium, unlocksTextX, unlocksTextY + 30, SWRTextColor::Green, SWRTextAlign::Left);
			WriteTextWrapper("Invitational: " + std::to_string(progress.invitationalUnlocks), SWRFont::Medium, unlocksTextX, unlocksTextY + 40, SWRTextColor::Green, SWRTextAlign::Left);
			break;
		default:
			break;
		}
	}

	void __fastcall DrawEvents::OnDrawTrackInfo()
	{
		WriteTextWrapper("Rewards are locked to \"Fair\" but are farmable", SWRFont::Medium, 50, 300);
		if (trackMirrored())
			WriteTextWrapper("Mirrored", SWRFont::ShopItem, 160, 50, SWRTextColor::Yellow, SWRTextAlign::Center);
	}

	void __fastcall DrawEvents::OnDrawPreRaceMenu()
	{
		WriteTextWrapper("AI Modifier: " + std::to_string(aiModifier), SWRFont::Medium, 300, 160);

		if (modifierControl)
		{
			WriteTextWrapper("Unlocked", SWRFont::Medium, 300, 170, SWRTextColor::LightBlue);
			WriteTextWrapper("Increase: Home/Page Up", SWRFont::Medium, 300, 180, SWRTextColor::White);
			WriteTextWrapper("Decrease: End/Page Down", SWRFont::Medium, 300, 190, SWRTextColor::White);
		}
		else
			WriteTextWrapper("Locked", SWRFont::Medium, 300, 170, SWRTextColor::Red);
	}

	void __fastcall DrawEvents::OnDrawInRace()
	{
		DrawNotifyQueue();
	}
}
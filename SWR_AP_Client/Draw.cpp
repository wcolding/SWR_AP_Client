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
		for (int i = 0; i < wattoShopEntries.size(); i++)
		{
			cachedEntry = wattoShopEntries[i];
			if (cachedEntry.tableOffset == index)
				return &cachedEntry;
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

	void __fastcall PrintItemNameFullView()
	{
		AP_WattoEntry* entry = GetItemEntry();
		if (entry == nullptr)
			return;

		WriteTextWrapper(entry->displayName, SWRFont::Default, 160, 25, typeColorMap[entry->itemType], SWRTextAlign::Center);
	}
	
	void __fastcall PrintItemNameBuyView()
	{
		AP_WattoEntry* entry = GetItemEntry();
		if (entry == nullptr)
			return;

		WriteTextWrapper(entry->displayName, SWRFont::ShopItem, 86, 23, typeColorMap[entry->itemType], SWRTextAlign::Center);

		// Other draw events
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

	void OnDraw()
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
				WriteTextWrapper(notifyQueue[0].msg, SWRFont::Large, 10, 16, SWRTextColor::LightBlue);
			}
		}

		prevTime = curTime;
		inRace = isPlayerInRace();

		// Connection status
		if (gamestate < SWRGameState::AP_Authenticated)
			WriteTextWrapper("Not connected to AP", SWRFont::Medium, 625, 20, SWRTextColor::Red, SWRTextAlign::Right);
		else if (!inRace) // only show in menus
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
			case 3: // Everything else?
				if (*menuValB == 13) // Track info screen
					WriteTextWrapper("Rewards are locked to \"Fair\" but are farmable", SWRFont::Medium, 50, 300);

				if (*menuValB == 3) // Pre-race "Main Menu"
				{
					if (inRace) // Don't draw during a race
						return;

					WriteTextWrapper("AI Modifier: " + std::to_string(aiModifier), SWRFont::Medium, 300, 160);

					if (modifierControl)
						WriteTextWrapper("Unlocked", SWRFont::Medium, 300, 170, SWRTextColor::LightBlue);
					else
						WriteTextWrapper("Locked", SWRFont::Medium, 300, 170, SWRTextColor::Red);
				}
				break;
			default:
				break;
			}
		}
	}

	void __fastcall ShopDrawStats(void* obj)
	{
		if (SWRGame::isItemPodPart())
			SWRGame::DrawStats(obj, 20.0, 135.0);
		else
		{
			WriteTextWrapper("Stats will appear here", SWRFont::ShopItem, 100, 155, SWRTextColor::LightBlue, SWRTextAlign::Center);
			WriteTextWrapper("for pod parts only", SWRFont::ShopItem, 100, 165, SWRTextColor::LightBlue, SWRTextAlign::Center);
		}
	}
}
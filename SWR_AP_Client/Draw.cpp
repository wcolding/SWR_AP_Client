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

				if (*menuValB == 7) // Watto's shop
				{
					int itemTableOffset = GetShopItemOffset();

					for (auto entry : wattoShopLocationToOffset)
					{
						
						if (entry.second == itemTableOffset)
						{
							std::string checkString = locationTable[entry.first];
							SWRTextColor checkColor = SWRTextColor::Green;

							if ((apShopData.entries[itemTableOffset].requiredRaces & 0x80) != 0)
							{
								checkString = std::format("{} - SOLD OUT", checkString);
								checkColor = SWRTextColor::Red;
							}

							WriteTextWrapper(checkString, SWRFont::Medium, 625, 0, checkColor, SWRTextAlign::Right);
						}
					}

					WriteTextWrapper(std::format("Offset {}", itemTableOffset), SWRFont::Medium, 625, 10, SWRTextColor::LightBlue, SWRTextAlign::Right);
				}

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
}
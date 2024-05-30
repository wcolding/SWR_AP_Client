#pragma once

#include <map>
#include <string>

#include "Enums.h"

static std::map<int, std::string> locationTable
{
	{100, "Watto's Shop - Traction 1"},
	{101, "Watto's Shop - Traction 2"},
	{102, "Watto's Shop - Traction 3"},
	{103, "Watto's Shop - Traction 4"},
	{104, "Watto's Shop - Traction 5"},

	{105, "Watto's Shop - Turning 1"},
	{106, "Watto's Shop - Turning 2"},
	{107, "Watto's Shop - Turning 3"},
	{108, "Watto's Shop - Turning 4"},
	{109, "Watto's Shop - Turning 5"},

	{110, "Watto's Shop - Acceleration 1"},
	{111, "Watto's Shop - Acceleration 2"},
	{112, "Watto's Shop - Acceleration 3"},
	{113, "Watto's Shop - Acceleration 4"},
	{114, "Watto's Shop - Acceleration 5"},

	{115, "Watto's Shop - Top Speed 1"},
	{116, "Watto's Shop - Top Speed 2"},
	{117, "Watto's Shop - Top Speed 3"},
	{118, "Watto's Shop - Top Speed 4"},
	{119, "Watto's Shop - Top Speed 5"},

	{120, "Watto's Shop - Air Brake 1"},
	{121, "Watto's Shop - Air Brake 2"},
	{122, "Watto's Shop - Air Brake 3"},
	{123, "Watto's Shop - Air Brake 4"},
	{124, "Watto's Shop - Air Brake 5"},

	{125, "Watto's Shop - Cooling 1"},
	{126, "Watto's Shop - Cooling 2"},
	{127, "Watto's Shop - Cooling 3"},
	{128, "Watto's Shop - Cooling 4"},
	{129, "Watto's Shop - Cooling 5"},

	{130, "Watto's Shop - Repair 1"},
	{131, "Watto's Shop - Repair 2"},
	{132, "Watto's Shop - Repair 3"},
	{133, "Watto's Shop - Repair 4"},
	{134, "Watto's Shop - Repair 5"},

	{135, "Junkyard - Item 1"},
	{136, "Junkyard - Item 2"},
	{137, "Junkyard - Item 3"},
	{138, "Junkyard - Item 4"},
	{139, "Junkyard - Item 5"},
	{140, "Junkyard - Item 6"},
	{141, "Junkyard - Item 7"},

	{142, "Pit Droid Shop - 1st Droid"},
	{143, "Pit Droid Shop - 2nd Droid"},
	{144, "Pit Droid Shop - 3rd Droid"},

	// Finishing these courses at the required place grants the location check
	{145, "Amateur Race 1"},
	{146, "Amateur Race 2"},
	{147, "Amateur Race 3"},
	{148, "Amateur Race 4"},
	{149, "Amateur Race 5"},
	{150, "Amateur Race 6"},
	{151, "Amateur Race 7"},

	{152, "Semi-Pro Race 1"},
	{153, "Semi-Pro Race 2"},
	{154, "Semi-Pro Race 3"},
	{155, "Semi-Pro Race 4"},
	{156, "Semi-Pro Race 5"},
	{157, "Semi-Pro Race 6"},
	{158, "Semi-Pro Race 7"},

	{159, "Galactic Race 1"},
	{160, "Galactic Race 2"},
	{161, "Galactic Race 3"},
	{162, "Galactic Race 4"},
	{163, "Galactic Race 5"},
	{164, "Galactic Race 6"},
	{165, "Galactic Race 7"},

	{166, "Invitational Race 1"},
	{167, "Invitational Race 2"},
	{168, "Invitational Race 3"},
	{169, "Invitational Race 4"},

	// First place on these courses unlocks a racer in vanilla, grants a check in AP
	{170, "Racer Unlock - Mon Gazza Speedway"},
	{171, "Racer Unlock - The Boonta Classic"},
	{172, "Racer Unlock - Howler Gorge"},
	{173, "Racer Unlock - Beedo's Wild Ride"},
	{174, "Racer Unlock - Andobi Mountain Run"},

	{175, "Racer Unlock - Bumpy's Breakers"},
	{176, "Racer Unlock - Scrapper's Run"},
	{177, "Racer Unlock - Spice Mine Run"},
	{178, "Racer Unlock - Aquilaris Classic"},
	{179, "Racer Unlock - Baroo Coast"},

	{180, "Racer Unlock - Abyss"},
	{181, "Racer Unlock - Zugga Challenge"},
	{182, "Racer Unlock - Vengeance"},
	{183, "Racer Unlock - Inferno"},
	{184, "Racer Unlock - Ando Prime Centrum"},

	{185, "Racer Unlock - Executioner" },
	{186, "Racer Unlock - Sunken City" },

	// Course Unlock Shuffle
	{ 187, "Amateur Race 1 - Course Unlock" },
	{ 188, "Amateur Race 2 - Course Unlock" },
	{ 189, "Amateur Race 3 - Course Unlock" },
	{ 190, "Amateur Race 4 - Course Unlock" },
	{ 191, "Amateur Race 5 - Course Unlock" },
	{ 192, "Amateur Race 6 - Course Unlock" },

	{ 193, "Semi-Pro Race 1 - Course Unlock" },
	{ 194, "Semi-Pro Race 2 - Course Unlock" },
	{ 195, "Semi-Pro Race 3 - Course Unlock" },
	{ 196, "Semi-Pro Race 4 - Course Unlock" },
	{ 197, "Semi-Pro Race 5 - Course Unlock" },
	{ 198, "Semi-Pro Race 6 - Course Unlock" },

	{ 199, "Galactic Race 1 - Course Unlock" },
	{ 200, "Galactic Race 2 - Course Unlock" },
	{ 201, "Galactic Race 3 - Course Unlock" },
	{ 202, "Galactic Race 4 - Course Unlock" },
	{ 203, "Galactic Race 5 - Course Unlock" },
	{ 204, "Galactic Race 6 - Course Unlock" },

	{ 205, "Invitational Race 1 - Course Unlock" },
	{ 206, "Invitational Race 2 - Course Unlock" },
	{ 207, "Invitational Race 3 - Course Unlock" }
};

// We'll check the unlocked racer flags and map them to the location IDs
static std::map<RacerUnlocks, int> racerUnlockTable
{
	{ RacerUnlocks::TeemtoPagalies, 170 },
	{ RacerUnlocks::Sebulba, 171 },
	{ RacerUnlocks::RattsTyerell, 172 },
	{ RacerUnlocks::AldarBeedo, 173 },
	{ RacerUnlocks::Mawhonic, 174 },

	{ RacerUnlocks::ArkBumpyRoose, 175 },
	{ RacerUnlocks::WanSandage, 176 },
	{ RacerUnlocks::MarsGuo, 177 },
	{ RacerUnlocks::CleggHoldfast, 178 },
	{ RacerUnlocks::NevaKee, 179 },

	{ RacerUnlocks::BozzieBaranta, 180 },
	{ RacerUnlocks::BolesRoor, 181 },
	{ RacerUnlocks::FudSang, 182 },
	{ RacerUnlocks::BenQuadrinaros, 183 },
	{ RacerUnlocks::SlideParamita, 184 },

	{ RacerUnlocks::ToyDampner, 185 },
	{ RacerUnlocks::BullseyeNavior, 186 }
};

static std::map<int, int> courseSlotToId
{
	{ 0, 145 },
	{ 1, 146 },
	{ 2, 147 },
	{ 3, 148 },
	{ 4, 149 },
	{ 5, 150 },
	{ 6, 151 },

	{ 8, 152 },
	{ 9, 153 },
	{ 10, 154 },
	{ 11, 155 },
	{ 12, 156 },
	{ 13, 157 },
	{ 14, 158 },

	{ 16, 159 },
	{ 17, 160 },
	{ 18, 161 },
	{ 19, 162 },
	{ 20, 163 },
	{ 21, 164 },
	{ 22, 165 },

	{ 24, 166 },
	{ 25, 167 },
	{ 26, 168 },
	{ 27, 169 }
};

static std::map<int, int> wattoShopLocationToOffset
{
	{ 100, 1 },
	{ 101, 2 },
	{ 102, 3 },
	{ 103, 4 },
	{ 104, 5 },

	{ 105, 7 },
	{ 106, 8 },
	{ 107, 9 },
	{ 108, 10 },
	{ 109, 11 },

	{ 110, 13 },
	{ 111, 14 },
	{ 112, 15 },
	{ 113, 16 },
	{ 114, 17 },

	{ 115, 19 },
	{ 116, 20 },
	{ 117, 21 },
	{ 118, 22 },
	{ 119, 23 },

	{ 120, 25 },
	{ 121, 26 },
	{ 122, 27 },
	{ 123, 28 },
	{ 124, 29 },

	{ 125, 31 },
	{ 126, 32 },
	{ 127, 33 },
	{ 128, 34 },
	{ 129, 35 },

	{ 130, 37 },
	{ 131, 38 },
	{ 132, 39 },
	{ 133, 40 },
	{ 134, 41 }
};

static std::map<int, int> courseIdToVanillaCircuit
{
	{ 0x00, 0 }, // Boonta Training Course
	{ 0x10, 0 }, // Mon Gazza Speedway
	{ 0x02, 0 }, // Beedo's Wild Ride
	{ 0x06, 0 }, // Aquilaris Classic
	{ 0x16, 0 }, // Malastare 100
	{ 0x13, 0 }, // Vengeance
	{ 0x11, 0 }, // Spice Mine Run

	{ 0x07, 1 }, // Sunken City
	{ 0x03, 1 }, // Howler Gorge
	{ 0x17, 1 }, // Dug Derby
	{ 0x09, 1 }, // Scrapper's Run
	{ 0x12, 1 }, // Zugga Challenge
	{ 0x0C, 1 }, // Baroo Coast
	{ 0x08, 1 }, // Bumpy's Breakers

	{ 0x14, 2 }, // Executioner
	{ 0x18, 2 }, // Sebulba's Legacy
	{ 0x0D, 2 }, // Grabvine Gateway
	{ 0x04, 2 }, // Andobi Mountain Run
	{ 0x0A, 2 }, // Dethro's Revenge
	{ 0x0E, 2 }, // Fire Mountain Rally
	{ 0x01, 2 }, // The Boonta Classic

	{ 0x05, 3 }, // Ando Prime Centrum
	{ 0x0B, 3 }, // Abyss
	{ 0x15, 3 }, // The Gauntlet
	{ 0x0F, 3 }  // Inferno
};

static std::map<int, int> courseClearToUnlock
{
	// Amateur
	{145, 187},
	{146, 188},
	{147, 189},
	{148, 190},
	{149, 191},
	{150, 192},

	// Semi-Pro
	{152, 193},
	{153, 194},
	{154, 195},
	{155, 196},
	{156, 197},
	{157, 198},

	// Galactic
	{159, 199},
	{160, 200},
	{161, 201},
	{162, 202},
	{163, 203},
	{164, 204},

	// Invitational
	{166, 205},
	{167, 206},
	{168, 207}
};
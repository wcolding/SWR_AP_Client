#pragma once

#include <map>
#include <string>

#include "Enums.h"

#define SWR_AP_BASE_ID 113800000

struct ItemInfo
{
	std::string name;
	ItemType type;
	int param1;
	int param2;
	int modelId;
};

struct QueuedItem
{
	ItemInfo info;
	bool notify;
};

static std::map<int, ItemInfo> itemTable
{
	// Pod parts
	// Progressive
	{ 0, ItemInfo("Progressive Traction",     ItemType::PodPart, 0, -1, -1)	},
	{ 1, ItemInfo("Progressive Turning",      ItemType::PodPart, 1, -1, -1)	},
	{ 2, ItemInfo("Progressive Acceleration", ItemType::PodPart, 2, -1, -1)	},
	{ 3, ItemInfo("Progressive Top Speed",    ItemType::PodPart, 3, -1, -1)	},
	{ 4, ItemInfo("Progressive Air Brake",    ItemType::PodPart, 4, -1, -1)	},
	{ 5, ItemInfo("Progressive Cooling",      ItemType::PodPart, 5, -1, -1)	},
	{ 6, ItemInfo("Progressive Repair",       ItemType::PodPart, 6, -1, -1)	},

	// Traction
	{ 7, ItemInfo("R-60 Repulsorgrip",        ItemType::PodPart, 0, 1, 0xCD) },
	{ 8, ItemInfo("R-80 Repulsorgrip",        ItemType::PodPart, 0, 2, 0xCE) },
	{ 9, ItemInfo("R-100 Repulsorgrip",       ItemType::PodPart, 0, 3, 0xE6) },
	{ 10, ItemInfo("R-300 Repulsorgrip",      ItemType::PodPart, 0, 4, 0xF1) },
	{ 11, ItemInfo("R-600 Repulsorgrip",      ItemType::PodPart, 0, 5, 0xF2) },

	// Turning
	{ 12, ItemInfo("Control Shift Plate",     ItemType::PodPart, 1, 1, 0x33) },
	{ 13, ItemInfo("Control Vectro Jet",      ItemType::PodPart, 1, 2, 0x34) },
	{ 14, ItemInfo("Control Coupling",        ItemType::PodPart, 1, 3, 0x35) },
	{ 15, ItemInfo("Control Nozzle",          ItemType::PodPart, 1, 4, 0x36) },
	{ 16, ItemInfo("Control Stabilizer",      ItemType::PodPart, 1, 5, 0xF0) },

	// Acceleration
	{ 17, ItemInfo("44 PCX Injector",         ItemType::PodPart, 2, 1, 0xC7) },
	{ 18, ItemInfo("Dual 32PCX Injector",     ItemType::PodPart, 2, 2, 0xC8) },
	{ 19, ItemInfo("Quad 32PCX Injector",     ItemType::PodPart, 2, 3, 0xC9) },
	{ 20, ItemInfo("Quad 44PCX Injector",     ItemType::PodPart, 2, 4, 0xCA) },
	{ 21, ItemInfo("Mag-6 Injector",          ItemType::PodPart, 2, 5, 0xCB) },

	// Top Speed
	{ 22, ItemInfo("Plug3 Thrust Coil",       ItemType::PodPart, 3, 1, 0xAA) },
	{ 23, ItemInfo("Plug5 Thrust Coil",       ItemType::PodPart, 3, 2, 0xAB) },
	{ 24, ItemInfo("Plug8 Thrust Coil",       ItemType::PodPart, 3, 3, 0xAC) },
	{ 25, ItemInfo("Block5 Thrust Coil",      ItemType::PodPart, 3, 4, 0xAD) },
	{ 26, ItemInfo("Block6 Thrust Coil",      ItemType::PodPart, 3, 5, 0xAE) },

	// Air Brake
	{ 27, ItemInfo("Mark III Air Brake",      ItemType::PodPart, 4, 1, 0x9E) },
	{ 28, ItemInfo("Mark IV Air Brake",       ItemType::PodPart, 4, 2, 0x9F) },
	{ 29, ItemInfo("Mark V Air Brake",        ItemType::PodPart, 4, 3, 0xA0) },
	{ 30, ItemInfo("Tri-Jet Air Brake",       ItemType::PodPart, 4, 4, 0xA1) },
	{ 31, ItemInfo("Quadrijet Air Brake",     ItemType::PodPart, 4, 5, 0xA2) },

	// Cooling
	{ 32, ItemInfo("Stack-3 Radiator",        ItemType::PodPart, 5, 1, 0xA4) },
	{ 33, ItemInfo("Stack-6 Radiator",        ItemType::PodPart, 5, 2, 0xA5) },
	{ 34, ItemInfo("Rod Coolant Pump",        ItemType::PodPart, 5, 3, 0xA6) },
	{ 35, ItemInfo("Dual Coolant Pump",       ItemType::PodPart, 5, 4, 0xA7) },
	{ 36, ItemInfo("Turbo Coolant Pump",      ItemType::PodPart, 5, 5, 0xA8) },

	// Repair
	{ 37, ItemInfo("Dual Power Cell",         ItemType::PodPart, 6, 1, 0xEB) },
	{ 38, ItemInfo("Quad Power Cell",         ItemType::PodPart, 6, 2, 0xED) },
	{ 39, ItemInfo("Cluster Power Plug",      ItemType::PodPart, 6, 3, 0xEE) },
	{ 40, ItemInfo("Rotary Power Plug",       ItemType::PodPart, 6, 4, 0xEF) },
	{ 41, ItemInfo("Cluster2 Power Plug",     ItemType::PodPart, 6, 5, 0xF3) },
	
	
	// Racers
	{ 42, ItemInfo("Anakin Skywalker",     ItemType::Racer, RacerUnlocks::AnakinSkywalker, NULL, 0x57) },
	{ 43, ItemInfo("Teemto Pagalies",      ItemType::Racer, RacerUnlocks::TeemtoPagalies,  NULL, 0x56) },
	{ 44, ItemInfo("Sebulba",              ItemType::Racer, RacerUnlocks::Sebulba,         NULL, 0x5B) },
	{ 45, ItemInfo("Ratts Tyerell",        ItemType::Racer, RacerUnlocks::RattsTyerell,    NULL, 0x5D) },
	{ 46, ItemInfo("Aldar Beedo",          ItemType::Racer, RacerUnlocks::AldarBeedo,      NULL, 0x69) },

	{ 47, ItemInfo("Mawhonic",             ItemType::Racer, RacerUnlocks::Mawhonic,        NULL, 0x59) },
	{ 48, ItemInfo("Ark 'Bumpy' Roose",    ItemType::Racer, RacerUnlocks::ArkBumpyRoose,   NULL, 0x60) },
	{ 49, ItemInfo("Wan Sandage",          ItemType::Racer, RacerUnlocks::WanSandage,      NULL, 0x63) },
	{ 50, ItemInfo("Mars Guo",             ItemType::Racer, RacerUnlocks::MarsGuo,         NULL, 0x5C) },
	{ 51, ItemInfo("Ebe Endocott",         ItemType::Racer, RacerUnlocks::EbeEndocott,     NULL, 0x5F) },

	{ 52, ItemInfo("Dud Bolt",             ItemType::Racer, RacerUnlocks::DudBolt,         NULL, 0x62) },
	{ 53, ItemInfo("Gasgano",              ItemType::Racer, RacerUnlocks::Gasgano,         NULL, 0x58) },
	{ 54, ItemInfo("Clegg Holdfast",       ItemType::Racer, RacerUnlocks::CleggHoldfast,   NULL, 0x61) },
	{ 55, ItemInfo("Elan Mak",             ItemType::Racer, RacerUnlocks::ElanMak,         NULL, 0x64) },
	{ 56, ItemInfo("Neva Kee",             ItemType::Racer, RacerUnlocks::NevaKee,         NULL, 0x67) },

	{ 57, ItemInfo("Bozzie Baranta",       ItemType::Racer, RacerUnlocks::BozzieBaranta,   NULL, 0x6A) },
	{ 58, ItemInfo("Boles Roor",           ItemType::Racer, RacerUnlocks::BolesRoor,       NULL, 0x6B) },
	{ 59, ItemInfo("Ody Mandrell",         ItemType::Racer, RacerUnlocks::OdyMandrell,     NULL, 0x5A) },
	{ 60, ItemInfo("Fud Sang",             ItemType::Racer, RacerUnlocks::FudSang,         NULL, 0x66) },
	{ 61, ItemInfo("Ben Quadrinaros",      ItemType::Racer, RacerUnlocks::BenQuadrinaros,  NULL, 0x5E) },

	{ 62, ItemInfo("Slide Paramita",       ItemType::Racer, RacerUnlocks::SlideParamita,   NULL, 0x68) },
	{ 63, ItemInfo("Toy Dampner",          ItemType::Racer, RacerUnlocks::ToyDampner,      NULL, 0x65) },
	{ 64, ItemInfo("'Bullseye' Navior",    ItemType::Racer, RacerUnlocks::BullseyeNavior,  NULL, 0x6C) },

	// Misc
	{ 65, ItemInfo("Pit Droid",                 ItemType::PitDroid, NULL,  NULL, 0x6D) },
	{ 66, ItemInfo("Semi Pro Circuit Pass",     ItemType::CircuitPass, 1,  NULL, 0xF4) },
	{ 67, ItemInfo("Galactic Circuit Pass",     ItemType::CircuitPass, 2,  NULL, 0xF4) },
	{ 68, ItemInfo("Invitational Circuit Pass", ItemType::CircuitPass, 3,  NULL, 0xF4) },
	{ 69, ItemInfo("Progressive Circuit Pass",  ItemType::CircuitPass, -1, NULL, 0xF4) },


	// Money
	{ 70, ItemInfo("400 Truguts",   ItemType::Money, 100,  NULL, 0x6F) },
	{ 71, ItemInfo("600 Truguts",   ItemType::Money, 600,  NULL, 0x6F) },
	{ 72, ItemInfo("800 Truguts",   ItemType::Money, 800,  NULL, 0x6F) },
	{ 73, ItemInfo("1000 Truguts",  ItemType::Money, 1000,  NULL, 0x6F) },
	{ 74, ItemInfo("1400 Truguts",  ItemType::Money, 1400,  NULL, 0x6F) },

	{ 75, ItemInfo("2100 Truguts",  ItemType::Money, 2100,  NULL, 0x6F) },
	{ 76, ItemInfo("2800 Truguts",  ItemType::Money, 2800,  NULL, 0x6F) },
	{ 77, ItemInfo("3500 Truguts",  ItemType::Money, 3500,  NULL, 0x6F) },

	{ 78, ItemInfo("Amateur Course Unlock", ItemType::CourseUnlock, 0, NULL, 0xF4) },
	{ 79, ItemInfo("Amateur Course Unlock", ItemType::CourseUnlock, 1, NULL, 0xF4) },
	{ 80, ItemInfo("Amateur Course Unlock", ItemType::CourseUnlock, 2, NULL, 0xF4) },
	{ 81, ItemInfo("Amateur Course Unlock", ItemType::CourseUnlock, 3, NULL, 0xF4) }
};


// possible models for AP Items
// watto = 0x6E
// dewback = 0x6F
// brotosaurus thing = 0x70
// jabba = 0x71
// ithorian = 0xf4
// very tiny c3p0 = 0xf5
// very tiny jar jar = 0xf6
// very tiny jawa = 0xf7
// very tiny r2d2 = 0xf8

// invalid/invisible 0x49
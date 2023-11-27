#pragma once

#include <map>
#include <string>

static std::map<int, std::string> itemNameTable
{
	// Pod parts
	// Progressive
	{0, "Progressive Traction"},
	{1, "Progressive Turning"},
	{2, "Progressive Acceleration"},
	{3, "Progressive Top Speed"},
	{4, "Progressive Air Brake"},
	{5, "Progressive Cooling"},
	{6, "Progressive Repair"},

	// Traction
	{7, "R-60 Repulsorgrip"},
	{8, "R-80 Repulsorgrip"},
	{9, "R-100 Repulsorgrip"},
	{10, "R-300 Repulsorgrip"},
	{11, "R-600 Repulsorgrip"},

	// Turning
	{12, "Control Shift Plate"},
	{13, "Control Vectro Jet"},
	{14, "Control Coupling"},
	{15, "Control Nozzle"},
	{16, "Control Stabilizer"},

	// Acceleration
	{17, "44 PCX Injector"},
	{18, "Dual 32PCX Injector"},
	{19, "Quad 32PCX Injector"},
	{20, "Quad 44PCX Injector"},
	{21, "Mag-6 Injector"},

	// Top Speed
	{22, "Plug3 Thrust Coil"},
	{23, "Plug5 Thrust Coil"},
	{24, "Plug8 Thrust Coil"},
	{25, "Block5 Thrust Coil"},
	{26, "Block6 Thrust Coil"},

	// Air Brake
	{27, "Mark III Air Brake"},
	{28, "Mark IV Air Brake"},
	{29, "Mark V Air Brake"},
	{30, "Tri-Jet Air Brake"},
	{31, "Quadrijet Air Brake"},

	// Cooling
	{32, "Stack-3 Radiator"},
	{33, "Stack-6 Radiator"},
	{34, "Rod Coolant Pump"},
	{35, "Dual Coolant Pump"},
	{36, "Turbo Coolant Pump"},

	// Repair
	{37, "Dual Power Cell"},
	{38, "Quad Power Cell"},
	{39, "Cluster Power Plug"},
	{40, "Rotary Power Plug"},
	{41, "Cluster2 Power Plug"},


	// Racers
	{42, "Anakin Skywalker"},
	{43, "Teemto Pagalies"},
	{44, "Sebulba"},
	{45, "Ratts Tyerell"},
	{46, "Aldar Beedo"},

	{47, "Mawhonic"},
	{48, "Ark 'Bumpy' Roose"},
	{49, "Wan Sandage"},
	{50, "Mars Guo"},
	{51, "Ebe Endocott"},

	{52, "Dud Bolt"},
	{53, "Gasgano"},
	{54, "Clegg Holdfast"},
	{55, "Elan Mak"},
	{56, "Neva Kee"},

	{57, "Bozzie Baranta"},
	{58, "Boles Roor"},
	{59, "Ody Mandrell"},
	{60, "Fud Sang"},
	{61, "Ben Quadrinaros"},

	{62, "Slide Paramita"},
	{63, "Toy Dampner"},
	{64, "Bullseye Navior"},


	// Misc
	{65, "Pit Droid"},
	{66, "Semi Pro Circuit Pass"},
	{67, "Galactic Circuit Pass"},
	{68, "Invitational Circuit Pass"},
	{69, "Progressive Circuit Pass"},


	// Money
	{ 70, "100 Truguts" },
	{ 71, "150 Truguts" },
	{ 72, "200 Truguts" },
	{ 73, "300 Truguts" },
	{ 74, "450 Truguts" },

	{ 75, "600 Truguts" },
	{ 76, "700 Truguts" },
	{ 77, "750 Truguts" },
	{ 78, "800 Truguts" },
	{ 79, "900 Truguts" },

	{ 80, "1050 Truguts" },
	{ 81, "1200 Truguts" },
	{ 82, "1400 Truguts" },
	{ 83, "1500 Truguts" },
	{ 84, "1600 Truguts" },

	{ 85, "1750 Truguts" },
	{ 86, "1800 Truguts" },
	{ 87, "2000 Truguts" },
	{ 88, "2200 Truguts" },
	{ 89, "2400 Truguts" },

	{ 90, "3000 Truguts" },
	{ 91, "3300 Truguts" },
	{ 92, "4400 Truguts" },
	{ 93, "5500 Truguts" }
};
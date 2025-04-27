#pragma once

enum PodStatus : int
{
	Unknown    = 0x08, // cheat related? causes "KA-POW" to be written to the screen
	Destroyed  = 0x10,
	Invincible = 0x20, // used on respawn
	Autopilot  = 0x40 // used on respawn
};

enum DeathState : int
{
	Alive     = 0,
	Local     = 1,
	Deathlink = 2	
};

enum ItemType : int
{
	PodPart      = 0,
	Racer        = 1,
	PitDroid     = 2,
	CircuitPass  = 3,
	Money        = 4,
	CourseUnlock = 5
};

enum SWRGameState : int
{
	Starting = 0,
	AP_Authenticated = 1,
	Save_Initialized = 2,
	Ready = 3,
	Closed = 4
};

enum PayoutSetting : char
{
	Fair = 1,
	Balanced = 2,
	WTA = 3
};

enum SWRFont
{
	Default,
	Medium,
	ShopItem,
	Large
};

enum SWRTextAlign
{
	Left,
	Center,
	Right
};

enum AP_ItemType : int
{
	Filler      = 0x00,
	Progression = 0x01,
	Useful      = 0x02,
	Trap        = 0x04
};

enum SWRTextColor : int
{
	White     = 0xFFFFFFFF,
	LightBlue = 0xB7F5FFFF,
	Red       = 0xFF0000FF,
	Yellow    = 0xFFFF00FF,
	Green     = 0x00FF00FF,
	
	AP_ProgressionItem = 0xAF99EFFF,
	AP_UsefulItem      = 0x6D8BE8FF,
	AP_FillerItem      = 0x00EEEEFF
};

enum CourseUnlockMode : int
{
	CircuitPassNoInv = 0,
	CircuitPassInvitational = 1,
	Shuffle = 2
};
#pragma once

enum RacerUnlocks : int
{
	None            = 0x00000000,
	AnakinSkywalker = 0x00000001,
	TeemtoPagalies  = 0x00000002,
	Sebulba         = 0x00000004,
	RattsTyerell    = 0x00000008,
	AldarBeedo      = 0x00000010,
	Mawhonic        = 0x00000020,
	ArkBumpyRoose   = 0x00000040,
	WanSandage      = 0x00000080,
	MarsGuo         = 0x00000100,
	EbeEndocott     = 0x00000200,
	DudBolt         = 0x00000400,
	Gasgano         = 0x00000800,
	CleggHoldfast   = 0x00001000,
	ElanMak         = 0x00002000,
	NevaKee         = 0x00004000,
	BozzieBaranta   = 0x00008000,
	BolesRoor       = 0x00010000,
	OdyMandrell     = 0x00020000,
	FudSang         = 0x00040000,
	BenQuadrinaros  = 0x00080000,
	SlideParamita   = 0x00100000,
	ToyDampner      = 0x00200000,
	BullseyeNavior  = 0x00400000
};

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

enum Circuit : int
{
	Amateur      = 0,
	SemiPro      = 1,
	Galactic     = 2,
	Invitational = 3
};
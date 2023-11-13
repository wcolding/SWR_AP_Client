#pragma once

#define POD_PARTS_OFFSET 0xA35AA1
#define DEFAULT_RACERS_OPCODE 0x3DA37

class SWRGame
{
private:
	int baseAddress;

public:
	SWRGame();
};

struct PodParts
{
	char traction;
	char turning;
	char acceleration;
	char topspeed;
	char airbrake;
	char cooling;
	char repair;
};

enum RacerUnlocks : int
{
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
	ElenMak         = 0x00002000,
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
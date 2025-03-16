#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "Enums.h"

struct AP_ServerInfo
{
	char server[64];
	char player[64];
	char pw[64];
};

#pragma pack(push)
#pragma pack(1)

typedef unsigned char PodParts[7];

struct PodHealth
{
	float leftFront;
	float leftMid;
	float leftBack;
	float rightFront;
	float rightMid;
	float rightBack;
};

struct PodData
{
private:
	char unk_00[0x61];
public:
	char status;
private:
	char unk_01[0x226];
public:
	PodHealth healthVals;
};

struct RaceData
{
public:
	float timerLap1; 
	float timerLap2; 
	float timerLap3;
private:
	float unk_00;
	float unk_01;
public:
	float timer;
};

typedef char TrackUnlocks[4];

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3& operator=(const Vector3& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}
};

struct SWR_PodPartEntry
{
	char globalId;
	char seriesId;
	char requiredRaces;
	char itemType;
	int cost;
	int modelId;
	char* displayText;
};

struct SWR_PodPartTable
{
	SWR_PodPartEntry entries[42];
};

struct ShopInventoryEntry
{
public:
	char partId;
	char partHealth;
private:
	short unk_00;
public:
	Vector3 position;
	Vector3 prevPosition;
private:
	Vector3 positionCopy;
public:
	float rotation;
	float swing;
private:
	char unk_01[8];
};

// Extremely incomplete
// Static and Skeletal meshes have some different fields
struct MeshData
{
private:
	char unk_00[0x14];
public:
	bool visible; // 0x14
private:
	char unk_01[0x2B];
public:
	Vector3 position;
};

// Not entirely sure what this structure is
struct GameStatus
{
private:
	char unk_00[0x5D];
public:
	char selectedCourseId; // 0x5D
	char selectedCircuit;  // 0x5E
private:
	char unk_01[0x32];
public:
	char payoutSetting;    // 0x91
};

// Also unsure what this is
// Holds pointer to stored text for menu options?
struct TextPtrStructure
{
private:
	char unk_00[0x4D4];
public:
	char* text;
};

#pragma pack(pop)

struct AP_WattoEntry
{
	AP_ItemType itemType;
	std::string displayName;
	int tableOffset;
};

struct NotifyMsg
{
	std::string msg;
	float timeRemaining;
};

struct Color
{
	int r, g, b, a;
};

struct RandomizedCourseData
{
	int id;
	bool mirrored = false;
};
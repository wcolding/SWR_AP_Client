#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "Enums.h"

struct APServerInfo
{
	char server[64];
	char player[64];
	char pw[64];
};

struct CourseData
{
	int slot;
	bool completed;
};

#pragma pack(push)
#pragma pack(1)

typedef char PodParts[7];

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

struct ItemShopEntry
{
	char globalId;
	char seriesId;
	char requiredRaces;
	char itemType;
	int cost;
	int modelId;
	char* displayText;
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

struct RacerSaveData
{
public:
	char profileName[0x18];    // 0x00
private:
	char unk_00[0x0C];         // 0x18
public:
	char selectedRacer;        // 0x24
	TrackUnlocks trackUnlocks; // 0x25
private:
	char unk_01;               // 0x29
public:
	uint64_t racePlacements;   // 0x2A
private:
	char unk_02[0x02];         // 0x32
public:
	RacerUnlocks racerUnlocks; // 0x34
	int money;                 // 0x38
	int cutscenesBitfield;     // 0x3C
	char pitDroids;            // 0x40
	PodParts parts;            // 0x41
	PodParts partsHealth;      // 0x48
private:
	char pad;                  // 0x4F
};

#pragma pack(pop)

struct APSaveData
{
	RacerUnlocks unlockedRacers;
	std::vector<CourseData> completedCourses;
	RacerSaveData racerSaveDataCopy;
};

struct NotifyMsg
{
	std::string msg;
	float timeRemaining;
};
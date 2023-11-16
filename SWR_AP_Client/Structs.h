#pragma once

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
#pragma once

#include <iostream>
#include <map>

struct player
{
	float fX = 0.0f;
	float fY = 0.0f;
	float fZ = 0.0f;
	float fAngle = 0.0f;
};

struct room
{
	int nHeight;
	int nWidth;
	std::wstring cells;
};
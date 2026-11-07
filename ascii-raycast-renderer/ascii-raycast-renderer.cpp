// ascii-raycast-renderer.cpp

#include <iostream>
#include <string>
#include <Windows.h>
#include <chrono>
#include "render.h"
#include "game.h"

using namespace std;

//Rendering
screen Screen; 
camera Camera;

//Game
player Player;
room Room;

void Raycast(wchar_t* screenBuffer)
{
	for (int x = 0; x < Screen.nWidth; x++)
	{
		/*
		* Calculate the current increment of the angle across the width of the screen.
		*/
		float fRayAngle = (Player.fAngle - Camera.fFov / 2) + ((x / (float)Screen.nWidth) * Camera.fFov);

		float fDistance = 0;
		bool isWallHit = false;

		while (!isWallHit)
		{
			fDistance += 0.5f;

			/* 
			* Calculate X and Y coordinates for:
			* 1. Distance (fDistance) 
			* 2. In direction (fRayAngle) 
			* 3. From position (Player.fX, Player.fY)
			*/
			int nStepX = (int)(fDistance * cos(fRayAngle) + Player.fX);
			int nStepY = (int)(fDistance * sin(fRayAngle) + Player.fY);

			if (nStepX < 0 || nStepX >= Room.nWidth || nStepY < 0 || nStepY >= Room.nHeight)
			{
				isWallHit = true;
				fDistance = 16.0f;
			}
			else if (Room.cells[nStepY * Room.nWidth + nStepX] == L'#')
			{
				isWallHit = true;
			}
		};

		/*
		* Calculate the height of the ceiling and floor (cutoff) based on distance from wall hit.
		*/
		float fCeiling = ((float)Screen.nHeight / 2) - ((float)Screen.nHeight / fDistance * 0.5f);
		float fFloor = (float)Screen.nHeight - fCeiling;

		/*
		* Assign character 'y' at current column index ' based on distance; 
		* set ceiling or floor based on if 'y' is greater than the ceiling cutoff.
		*/
		for (int y = 0; y < Screen.nHeight; y++)
		{
			int nBufferIndex = y * Screen.nWidth + x;

			if (y <= fCeiling)
			{
				screenBuffer[nBufferIndex] = L' ';
			}
			else if (y > fCeiling && y <= fFloor)
			{
				if (fDistance < 4)
				{
					screenBuffer[nBufferIndex] = L'\u2588';
				}
				else if (fDistance < 6)
				{
					screenBuffer[nBufferIndex] = L'\u2593';
				}
				else if (fDistance < 8)
				{
					screenBuffer[nBufferIndex] = L'\u2592';
				}
				else
				{
					screenBuffer[nBufferIndex] = L'\u2591';
				}
			}
			else
			{
				screenBuffer[nBufferIndex] = L'.';//L'\u2581';
			}
		}
	}
}

int main()
{
	//Initialise attributes
	Screen.nWidth = 150;
	Screen.nHeight = 50;

	Room.nHeight = 16;
	Room.nWidth = 16;

	Room.cells += L"################";
	Room.cells += L"#..............#";
	Room.cells += L"######.........#";
	Room.cells += L"#.......##.....#";
	Room.cells += L"#.......##.....#";
	Room.cells += L"#...........####";
	Room.cells += L"#..............#";
	Room.cells += L"#..............#";
	Room.cells += L"#..............#";
	Room.cells += L"####...........#";
	Room.cells += L"#.........#....#";
	Room.cells += L"#.........#....#";
	Room.cells += L"#.........#....#";
	Room.cells += L"#.........#....#";
	Room.cells += L"#.........#....#";
	Room.cells += L"################";

	Player.fX = 8;
	Player.fY = 8;

	//Initialise buffer
	wchar_t* screenBuffer = new wchar_t[Screen.nHeight * Screen.nWidth];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	//Initialise time vars
	auto lastFrameTime = chrono::system_clock::now();
	float deltaTime = 0;

	//Render loop
	while (true)
	{
		//Calculate delta time
		deltaTime = chrono::duration<float>(chrono::system_clock::now() - lastFrameTime).count();
		lastFrameTime = chrono::system_clock::now();

		//Inpput and Movement
		if (GetAsyncKeyState('W') & 0x8000)
		{
			Player.fX += cos(Player.fAngle) * 5 * deltaTime;
			Player.fY += sin(Player.fAngle) * 5 * deltaTime;
		};

		if (GetAsyncKeyState('S') & 0x8000)
		{
			Player.fX -= cos(Player.fAngle) * 5 * deltaTime;
			Player.fY -= sin(Player.fAngle) * 5 * deltaTime;
		};

		if (GetAsyncKeyState('A') & 0x8000)
		{
			Player.fAngle -= 2.5 * deltaTime;
		}

		if (GetAsyncKeyState('D') & 0x8000)
		{
			Player.fAngle += 2.5 * deltaTime;
		}

		//Raycast
		Raycast(screenBuffer);

		//Draw
		screenBuffer[Screen.nWidth * Screen.nHeight - 1] = '\0';
		WriteConsoleOutputCharacterW(hConsole, screenBuffer, Screen.nWidth * Screen.nHeight, { 0, 0 }, &dwBytesWritten);
	}
	
	return 0;
}

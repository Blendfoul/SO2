#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <tchar.h>
#include <fcntl.h>
#include "../MappedDLL/header.h"

//Global vars

HANDLE hConsole, hBallControl;
bool LIVE, keypressed;
PLAYERS aux;
GAMEDATA game;
GAMEDATAPIPE gameP;

// Function headers
DWORD WINAPI ConsoleInput(LPVOID param);
DWORD WINAPI Ball(LPVOID param);
void gotoxy(int x, int y);
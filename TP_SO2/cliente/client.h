#pragma once

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <tchar.h>
#include <fcntl.h>
#include "../MappedDLL/header.h"

//Global vars

HANDLE hConsole, hBallControl;
bool LIVE, keypressed;
PLAYERS aux;
GAMEDATA game;

// Function headers
DWORD WINAPI ConsoleInput();
DWORD WINAPI Ball();
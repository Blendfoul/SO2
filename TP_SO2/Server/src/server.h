#pragma once

#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>

#define MAX 20
#define BUFFERS 10
#define MAX_PLAYERS 20
#define USRVALID 1
#define USRINVALID 2

//Global Variables
int nPlayers;
BOOL LIVE;
BOOL gameOn;
PLAYERS players[MAX_PLAYERS];
HANDLE hCanWrite, hCanRead, hMem, hFile;
HANDLE hMutex;
HANDLE hCons;
HANDLE hLogin;
PLAYERS *pBuf = NULL;

//Function prototype
DWORD WINAPI ServerInput();
BOOL HandleAction(PLAYERS pAction);
BOOL AddPlayerToArray(int PlayerId, wchar_t * username);
int getPlayerId(int pid);
int getPlayerUsername(TCHAR * nome);
BOOL DenyPlayerAcess();
BOOL BuildBroadcast();
DWORD BallMovement();
#pragma once

#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include "../MappedDll/header.h"

#define MAXT 256
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
HANDLE hMutex, hMutexCanWrite;
HANDLE hCons;
HANDLE hLogin;
SHAREDMEM *pBuf = NULL;

//Function prototype
DWORD WINAPI ServerInput();
SHAREDMEM RecieveRequest();
BOOL HandleAction(SHAREDMEM pAction);
PLAYERS AddPlayerToArray(PLAYERS pAction);
int getPlayerId(int pid);
int getPlayerUsername(TCHAR * nome);
BOOL DenyPlayerAcess();
BOOL BuildBroadcast(SHAREDMEM *pAction);
DWORD WINAPI BallMovement();

void SaveTopTen();
void loadTopTen();
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
#define MAX_PLAYERS 5

//Global Variables
int nPlayers;
BOOL LIVE;
BOOL gameOn;
HANDLE hCanWrite, hCanRead, hMem, hFile;
HANDLE hMutex, hMutexCanWrite;
HANDLE hCons;
HANDLE hLogin;
SHAREDMEM *pBuf = NULL;

//Function prototype
DWORD WINAPI ServerInput();
PLAYERS RecieveRequest();
BOOL HandleAction(PLAYERS pAction);
PLAYERS AddPlayerToArray(PLAYERS *pAction);
BOOL RemovePlayerFromArray(PLAYERS *pPlayers);
int getPlayerId(int pid);
int getPlayerUsername(TCHAR * nome);
BOOL BuildReply(PLAYERS *pAction);
DWORD WINAPI BallMovement();

void SaveTopTen();
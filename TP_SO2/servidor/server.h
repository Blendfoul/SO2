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
#define MAX_PLAYERS 5
#define MAX_SCREEN_WIDTH 1024
#define MAX_SCREEN_HEIGHT 768
#define MOVE_BALL_UPRIGHT 0
#define MOVE_BALL_UPLEFT 1
#define MOVE_BALL_DOWNRIGHT 2
#define MOVE_BALL_DOWNLEFT 3

//Global Variables
int nPlayers;
bool LIVE;
BOOL gameOn;
HANDLE hCanWrite, hCanRead, hCanReadBroad, hCanWriteBroad, hMemPlayers, hFilePlayers, hMemGame, hFileGame;
HANDLE hMutex, hMutexBroad;
HANDLE hCons, hMovBola, hInput;
HANDLE hLogin;
SHAREDMEM *pBuf = NULL;
GAMEDATA *pGameDataShared;

//Function prototype
DWORD WINAPI ServerInput();
PLAYERS RecieveRequest();
BOOL HandleAction(PLAYERS pAction);
PLAYERS AddPlayerToArray(PLAYERS *pAction);
BOOL RemovePlayerFromArray(PLAYERS *pPlayers);
int getPlayerId(int pid);
int getPlayerUsername(TCHAR * nome);
BOOL BuildReply(PLAYERS *pAction);
DWORD WINAPI BallMovement(LPVOID lparam);

PLAYERS SaveTopTen(PLAYERS *pAction);

BOOL SendBroadcast();
DWORD WINAPI ServerConsole();
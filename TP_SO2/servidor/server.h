#pragma once

#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
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
int nBalls;
int *ballThreadId = NULL;
bool LIVE;
BOOL gameOn;
HANDLE hCanWrite, hCanRead, hCanReadBroad, hCanWriteBroad, hMemPlayers, hFilePlayers, hMemGame, hFileGame;
HANDLE hMutex = NULL, hMutexBroad = NULL;
HANDLE hCons = NULL, hInput = NULL;
HANDLE *hMovBola = NULL;
HANDLE hLogin = NULL;
SHAREDMEM *pBuf = NULL;
GAMEDATA *pGameDataShared = NULL;

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

BOOL SendBroadcast(BALL *ball);
DWORD WINAPI ServerConsole();
void PrintPlayers();
HANDLE * CreateHandleArray(HANDLE* handle, int* tam);
int * ballIdArray(int* threadId, int* tam);
BALL* CreateBallArray(BALL* ball, int* tam);
BOOL AddBall();
BOOL RemoveBall();
BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo);

typedef struct
{
	OVERLAPPED oOverlap;
	HANDLE hPipeInst;
	PLAYERS player;
	DWORD cbRead;
	PLAYERS playerReply;
	DWORD cbToWrite;
	DWORD dwState;
	BOOL fPendingIO;
} PIPEINST, * LPPIPEINST;

//PLAYERS ShowTop10(PLAYERS* pAction);
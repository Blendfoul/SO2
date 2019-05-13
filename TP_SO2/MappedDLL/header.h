// header.h : include file for standard system include files,
// or project specific include files
//
#pragma once

#ifdef MAPPEDDLL_EXPORTS  
#define MAPPEDDLL_IMP_API __declspec(dllexport)   
#else  
#define MAPPEDDLL_IMP_API __declspec(dllimport)   
#endif  

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>

#define BufferSize 100
#define BUFFERS 10
#define MAX 256
#define USRVALID 1
#define USRINVALID 2
#define LOGOUTSUCCESS 9
#define SERVERCLOSE 8

typedef struct {
	TCHAR names[10][MAX];
	int points[10];
}TOP10;

typedef struct
{
    int id;
    int code;
    TCHAR username[MAX];
	TCHAR command[MAX];
    int pos;
    int score;
	TOP10 top;
} PLAYERS;

typedef struct {
	int in, out;
	PLAYERS	players[10];
}SHAREDMEM;

typedef struct
{
	int x, y;
	int trajectory;
	int id;
}BALL;

typedef struct
{
	int in, out;
	int nBalls;
	BALL ball[10][5];
}GAMEDATA;

// TODO: reference additional headers your program requires here

	MAPPEDDLL_IMP_API BOOL TesteDLL();

	MAPPEDDLL_IMP_API BOOL Login(PLAYERS *client);

	MAPPEDDLL_IMP_API PLAYERS RecieveMessage(PLAYERS * client);
	MAPPEDDLL_IMP_API BOOL SendMessages(PLAYERS * client);

	MAPPEDDLL_IMP_API GAMEDATA RecieveBroadcast(GAMEDATA *pGame);

	MAPPEDDLL_IMP_API void CloseVars();
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

#define BufferSize 100
#define Buffers 10
#define MAX 256
#define USRVALID 1
#define USRINVALID 2
#define LOGOUTSUCCESS 9

typedef struct
{
    int id;
    int code;
    TCHAR username[MAX];
	TCHAR command[MAX];
    int pos;
    int score;
} PLAYERS;

typedef struct {
	int in, out;
	PLAYERS	players[10];
}SHAREDMEM;

typedef struct
{
	int x, y;
	int id;
}BALL;

typedef struct
{
	int in, out;
	BALL ball;
	
}GAMEDATA;

// TODO: reference additional headers your program requires here

	MAPPEDDLL_IMP_API BOOL TesteDLL();

	MAPPEDDLL_IMP_API BOOL Login(PLAYERS *client);

	MAPPEDDLL_IMP_API PLAYERS RecieveMessage(PLAYERS * client);
	MAPPEDDLL_IMP_API BOOL SendMessages(PLAYERS * client);

	MAPPEDDLL_IMP_API BOOL RecieveBroadcast();

	MAPPEDDLL_IMP_API void CloseVars();
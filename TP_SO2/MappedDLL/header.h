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
#include <memory.h>

#define BufferSize 100
#define Buffers 10
#define MAX 20

typedef struct
{
    int id;
    int code;
    TCHAR username[MAX];
    int in, out;
    int pos;
    int score;
} PLAYERS;

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

extern "C" {
	MAPPEDDLL_IMP_API BOOL TesteDLL();

	MAPPEDDLL_IMP_API BOOL Login(PLAYERS *client);

	MAPPEDDLL_IMP_API BOOL RecieveMessage(PLAYERS * client);
	MAPPEDDLL_IMP_API BOOL SendMessages(PLAYERS * client);

    MAPPEDDLL_IMP_API BOOL RecieveBroadcast();
}
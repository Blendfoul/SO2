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

//structs to use in shared mem
    typedef struct
    {
        int id;
        wchar_t username[MAX];
        int in, out;
    } PLAYERS;

//Global Variables
    int nPlayers;
    BOOL LIVE;
    PLAYERS players[MAX_PLAYERS];
    HANDLE hCanWrite, hCanRead, hMem, hFile;
    HANDLE hMutex;
    HANDLE hCons;
    PLAYERS *pBuf = NULL;

//Function prototype
    DWORD WINAPI ServerInput();
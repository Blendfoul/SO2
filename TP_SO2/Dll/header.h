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


typedef struct
    {
        int id;
        wchar_t username[20];
        int in, out;
    } PLAYER;

#define BufferSize 100
#define Buffers 10

// TODO: reference additional headers your program requires here

extern "C" {
	MAPPEDDLL_IMP_API BOOL TesteDLL();

	MAPPEDDLL_IMP_API BOOL LoginSequence(PLAYER *client);

	MAPPEDDLL_IMP_API BOOL ReadBuffer(PLAYER * client);
	MAPPEDDLL_IMP_API BOOL WriteBuffer(PLAYER * client);
}
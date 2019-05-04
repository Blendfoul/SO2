// MappedDLL.cpp : Defines the exported functions for the DLL application.
//
#include <windows.h>
#include "header.h"

SHAREDMEM player;
SHAREDMEM *pShared;

TCHAR NameCanWrite[] = TEXT("Semaphore_1");
TCHAR NameCanRead[] = TEXT("Semaphore_2");

HANDLE hCanWrite, hCanRead;
HANDLE hMem, hFile;
HANDLE mutex_1, mutex_2;

BOOL TesteDLL(PLAYERS *client)
{
	printf("TesteServer");
	//printf("\nIN: %d\n", client->id);
	return true;
}

BOOL RecieveBroadcast(){
	return true;
}

PLAYERS RecieveMessage(PLAYERS *client)
{
	if (pShared != NULL) {
		WaitForSingleObject(hCanRead, INFINITE);
		WaitForSingleObject(mutex_1, INFINITE);

		player = *pShared;
		if(client->id = player.players[player.out].id)
			client = &player.players[player.out];
		(player.out)++;

		ReleaseMutex(mutex_1);
		ReleaseSemaphore(hCanWrite, 1, NULL);
	}
	return *client;
}

BOOL SendMessages(PLAYERS *client)
{
	_tprintf(TEXT("%d\n"), client->id);
	WaitForSingleObject(hCanWrite, INFINITE);
	WaitForSingleObject(mutex_1, INFINITE);

	(player.in)++ % Buffers;
	player.players[player.in] = *client;
	CopyMemory(pShared, &player, sizeof(SHAREDMEM));

	ReleaseMutex(mutex_1);
	ReleaseSemaphore(hCanRead, 1, NULL);

	return true;
}

BOOL Login(PLAYERS *client)
{
	mutex_1 = OpenMutex(MUTEX_ALL_ACCESS, 0, TEXT("Mutex_1"));
	if (mutex_1 == NULL)
	{
		CloseHandle(mutex_1);
		return EXIT_FAILURE;
	}
	hCanRead = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, NameCanRead);
	hCanWrite = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, NameCanWrite);
	
	hFile = CreateFile(NULL, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	hMem = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(SHAREDMEM), TEXT("Shared_1"));

	pShared = (SHAREDMEM *)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SHAREDMEM));
	if (pShared == NULL)
	{
		_tprintf_s(TEXT("Erro de criação da view of file %d\n"), _tcslen(TEXT("Erro de criação da view of file %d\n")),GetLastError());

		CloseHandle(hFile);
		CloseHandle(hMem);
		_gettchar();
		return EXIT_FAILURE;
	}
	
	player.in = 0;
	player.out = 0;
		
	WaitForSingleObject(hCanWrite, INFINITE);
	WaitForSingleObject(mutex_1, INFINITE);

	player.players[player.in] = *client;
	CopyMemory(pShared, &player, sizeof(SHAREDMEM));

	ReleaseMutex(mutex_1);
	ReleaseSemaphore(hCanRead, 1, NULL);

	return true;
}

void CloseVars() {
	UnmapViewOfFile(pShared);
	CloseHandle(mutex_1);
	CloseHandle(hCanRead);
	CloseHandle(hCanWrite);
	CloseHandle(hMem);
	CloseHandle(hFile);
}

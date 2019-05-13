// MappedDLL.cpp : Defines the exported functions for the DLL application.
//
#include <windows.h>
#include "header.h"

SHAREDMEM player;
SHAREDMEM *pShared;
GAMEDATA *pSharedGame;

TCHAR NameCanWrite[] = TEXT("Semaphore_1");
TCHAR NameCanRead[] = TEXT("Semaphore_2");

HANDLE hCanWrite, hCanRead, hCanWriteBroad, hCanReadBroad;
HANDLE hMem, hMemGame;
HANDLE hFile, hFileGame;
HANDLE mutex_1, mutex_2;

BOOL TesteDLL(PLAYERS *client)
{
	printf("TesteServer");
	//printf("\nIN: %d\n", client->id);
	return true;
}

GAMEDATA RecieveBroadcast(GAMEDATA *pGame){

	WaitForSingleObject(hCanReadBroad, INFINITE);
	WaitForSingleObject(mutex_2, INFINITE);

	pGame = pSharedGame;

	ReleaseMutex(mutex_2);
	ReleaseSemaphore(hCanWriteBroad, 1, NULL);

	return *pGame;
}

PLAYERS RecieveMessage(PLAYERS *client)
{
	if (pShared != NULL) {
		WaitForSingleObject(hCanRead, INFINITE);
		WaitForSingleObject(mutex_1, INFINITE);

		player = *pShared;
		
		if (player.out == 10)
			player.out = 0;
		
		client = &player.players[player.out];
		
		if (player.players[player.out].code != SERVERCLOSE)
			(player.out)++;

		

		ReleaseMutex(mutex_1);
		ReleaseSemaphore(hCanWrite, 1, NULL);
	}
	return *client;
}

BOOL SendMessages(PLAYERS *client)
{
	_tprintf(TEXT("ID: %d IN: %d\n"), client->id, player.in);
	WaitForSingleObject(hCanWrite, INFINITE);
	WaitForSingleObject(mutex_1, INFINITE);

	player.players[player.in] = *client;
	CopyMemory(pShared, &player, sizeof(SHAREDMEM));
	if (player.in == 10)
		player.in = 0;
	else
		(player.in)++;

	ReleaseMutex(mutex_1);
	ReleaseSemaphore(hCanRead, 1, NULL);

	return true;
}

BOOL Login(PLAYERS *client)
{
	mutex_1 = OpenMutex(MUTEX_ALL_ACCESS, 0, TEXT("Mutex_1"));
	if (mutex_1 == NULL)
	{
		//CloseHandle(mutex_1);
		return EXIT_FAILURE;
	}

	mutex_2 = OpenMutex(MUTEX_ALL_ACCESS, 0, TEXT("Mutex_2"));
	if (mutex_2 == NULL)
	{
		CloseHandle(mutex_1);
		//CloseHandle(mutex_2);
		return EXIT_FAILURE;
	}

	hCanRead = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, NameCanRead);
	hCanWrite = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, NameCanWrite);
	hCanWriteBroad = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, TEXT("Semaphore_3"));
	hCanReadBroad = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, TEXT("Semaphore_4"));

	if (hCanRead == NULL || hCanWrite == NULL || hCanReadBroad == NULL || hCanWriteBroad == NULL) {
		
		return EXIT_FAILURE;
	}

	hFile = CreateFile(TEXT("Shared"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL)
		return EXIT_FAILURE ;

	hMem = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(SHAREDMEM), TEXT("Shared_1"));
	if (hMem == NULL)
		return EXIT_FAILURE;

	hFileGame = CreateFile(TEXT("Shared2"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileGame == NULL)
		return EXIT_FAILURE;

	hMemGame = CreateFileMapping(hFileGame, NULL, PAGE_READWRITE, 0, sizeof(GAMEDATA), TEXT("Shared_2"));
	if (hMemGame == NULL)
		return EXIT_FAILURE;

	pShared = (SHAREDMEM *)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SHAREDMEM));
	if (pShared == NULL)
	{
		_tprintf_s(TEXT("Erro de criação da view of file %lu\n"), GetLastError());

		CloseHandle(hMem);
		return EXIT_FAILURE;
	}

	pSharedGame = (GAMEDATA *)MapViewOfFile(hMemGame, FILE_MAP_READ, 0, 0, sizeof(GAMEDATA));
	if (pSharedGame == NULL)
	{
		_tprintf_s(TEXT("Erro de criação da view of file %lu\n"), GetLastError());

		CloseHandle(hMem);
		CloseHandle(hMemGame);
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
	UnmapViewOfFile(pSharedGame);
	UnmapViewOfFile(pShared);
	//CloseHandle(mutex_1);
	//CloseHandle(mutex_2);
	CloseHandle(hCanRead);
	CloseHandle(hCanWrite);
	CloseHandle(hMem);
	CloseHandle(hMemGame);
}

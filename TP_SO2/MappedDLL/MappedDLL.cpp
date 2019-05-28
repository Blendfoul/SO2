// MappedDLL.cpp : Defines the exported functions for the DLL application.
//
#include <windows.h>
#include "header.h"

SHAREDMEM player;
SHAREDMEM* pShared;
GAMEDATA* pSharedGame;

TCHAR NameCanWrite[] = TEXT("Semaphore_1");
TCHAR NameCanRead[] = TEXT("Semaphore_2");

//SHAREDMEM
HANDLE hCanWrite, hCanRead, hCanWriteBroad, hCanReadBroad;
HANDLE hMem, hMemGame;
HANDLE hFile, hFileGame;
HANDLE mutex_1, mutex_2;

//PIPES
HANDLE hPipe = NULL;
HANDLE hThread;

//PIPE_NAME
wchar_t PIPE_SEND_SERVER_NAME[100];
wchar_t PIPE_RECEIVE_SERVER_NAME[100];

BOOL InitPipes(TCHAR* ipAdress);
BOOL InitSharedMem();

BOOL TesteDLL(PLAYERS* client)
{
	printf("TesteServer");
	//printf("\nIN: %d\n", client->id);
	return true;
}

GAMEDATA RecieveBroadcast(GAMEDATA* pGame)
{

	WaitForSingleObject(hCanReadBroad, INFINITE);
	WaitForSingleObject(mutex_2, INFINITE);

	pGame = pSharedGame;

	ReleaseMutex(mutex_2);
	ReleaseSemaphore(hCanWriteBroad, 1, NULL);

	return *pGame;
}

PLAYERS RecieveMessage(PLAYERS* client)
{
	if (pShared != NULL)
	{
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

PLAYERS RecieveMessage(PLAYERS* client, TCHAR* ipAdress)
{
	BOOL fSuccess;
	DWORD numberOfReadBytes;

	_tprintf(TEXT("ID: %d IN: %d\n"), client->id, player.in);

	do
	{
		fSuccess = ReadFile(
			hPipe,              // pipe handle
			client,             // buffer to receive reply
			sizeof(PLAYERS),    // size of buffer
			&numberOfReadBytes, // number of bytes read
			NULL);              // not overlapped

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess); // repeat loop if ERROR_MORE_DATA

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return *client;
	}

	return *client;
}

BOOL SendMessages(PLAYERS* client)
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

BOOL SendMessages(PLAYERS* client, TCHAR* ipAdress)
{
	DWORD cbToWrite, cbWritten;
	BOOL fSuccess;


	cbToWrite = sizeof(PLAYERS);
	//_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		client,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	return true;
}

BOOL Login(PLAYERS* client)
{

	InitSharedMem();
	SendMessages(client);

	return true;
}

BOOL Login(PLAYERS* client, TCHAR* ipAdress)
{

	_tprintf(TEXT("IP Recebido: %s\n"), ipAdress);
	InitPipes(ipAdress);
	SendMessages(client, ipAdress);

	return TRUE;
}

void CloseVars()
{
	UnmapViewOfFile(pSharedGame);
	UnmapViewOfFile(pShared);
	//CloseHandle(mutex_1);
	//CloseHandle(mutex_2);
	CloseHandle(hCanRead);
	CloseHandle(hCanWrite);
	CloseHandle(hMem);
	CloseHandle(hMemGame);
}

BOOL InitPipes(TCHAR* ipAdress)
{

	DWORD dwMode;
	BOOL fSuccess;

	swprintf(PIPE_SEND_SERVER_NAME, 100, TEXT("\\\\%s\\Arkanoid\\pipe"), (wchar_t*)ipAdress);
	_tprintf(TEXT("%s\n"), PIPE_SEND_SERVER_NAME);

	hPipe = CreateFile(
		PIPE_SEND_SERVER_NAME, // pipe name
		GENERIC_READ |         // read and write access
		GENERIC_WRITE,
		0,             // no sharing
		NULL,          // default security attributes
		OPEN_EXISTING, // opens existing pipe
		0,             // default attributes
		NULL);         // no template file

	// Break if the pipe handle is valid.

	if (hPipe != INVALID_HANDLE_VALUE)
		return TRUE;

	// Exit if an error other than ERROR_PIPE_BUSY occurs.

	if (GetLastError() != ERROR_PIPE_BUSY)
	{
		_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
		return TRUE;
	}

	// All pipe instances are busy, so wait for 20 seconds.

	if (!WaitNamedPipe(PIPE_SEND_SERVER_NAME, 20000))
	{
		printf("Could not open pipe: 20 second wait timed out.");
		return TRUE;
	}

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,   // pipe handle
		&dwMode, // new pipe mode
		NULL,    // don't set maximum bytes
		NULL);   // don't set maximum time
	if (!fSuccess)
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return TRUE;
	}

	return FALSE;
}

void DisconnectPipes()
{
	CloseHandle(hPipe);
}

BOOL InitSharedMem()
{
	mutex_1 = OpenMutex(MUTEX_ALL_ACCESS, 0, TEXT("Mutex_1"));
	if (mutex_1 == NULL)
	{
		_tprintf(TEXT("Servidor não está ligado!\n"));
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

	if (hCanRead == NULL || hCanWrite == NULL || hCanReadBroad == NULL || hCanWriteBroad == NULL)
	{

		return EXIT_FAILURE;
	}

	hFile = CreateFile(TEXT("Shared"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL)
		return EXIT_FAILURE;

	hMem = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(SHAREDMEM), TEXT("Shared_1"));
	if (hMem == NULL)
		return EXIT_FAILURE;

	hFileGame = CreateFile(TEXT("Shared2"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileGame == NULL)
		return EXIT_FAILURE;

	hMemGame = CreateFileMapping(hFileGame, NULL, PAGE_READWRITE, 0, sizeof(GAMEDATA), TEXT("Shared_2"));
	if (hMemGame == NULL)
		return EXIT_FAILURE;

	pShared = (SHAREDMEM*)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SHAREDMEM));
	if (pShared == NULL)
	{
		_tprintf_s(TEXT("Erro de criação da view of file %lu\n"), GetLastError());

		CloseHandle(hMem);
		return EXIT_FAILURE;
	}

	pSharedGame = (GAMEDATA*)MapViewOfFile(hMemGame, FILE_MAP_READ, 0, 0, sizeof(GAMEDATA));
	if (pSharedGame == NULL)
	{
		_tprintf_s(TEXT("Erro de criação da view of file %lu\n"), GetLastError());

		CloseHandle(hMem);
		CloseHandle(hMemGame);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
// MappedDLL.cpp : Defines the exported functions for the DLL application.
//
#include <windows.h>
#include "header.h"

PLAYERS player, *pShared;

TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semaphore_1");
TCHAR NomeSemaforoPodeLer[] = TEXT("Semaphore_2");

HANDLE PodeEscreverBuffer, PodeLerBuffer;
HANDLE hMem, hFile;
HANDLE mutex_1, mutex_2;

BOOL TesteDLL(PLAYERS *client)
{
	printf("TesteServer");
	printf("\nIN: %d\n", client->id);
	return true;
}

BOOL RecieveBroadcast(){
	return true;
}

BOOL RecieveMessage(PLAYERS *client)
{
	int pos;

	WaitForSingleObject(PodeLerBuffer, INFINITE);
	WaitForSingleObject(mutex_1, INFINITE);

	pos = client->out;
	client->out = (client->out + 1) % Buffers;
	CopyMemory(client, client, sizeof(PLAYERS));

	ReleaseMutex(mutex_1);
	ReleaseSemaphore(PodeEscreverBuffer, 1, NULL);

	return true;
}

BOOL SendMessages(PLAYERS *client)
{
	int pos;

	WaitForSingleObject(PodeEscreverBuffer, INFINITE);
	WaitForSingleObject(mutex_2, INFINITE);

	pos = client->in;
	client->in = (client->in + 1) % Buffers;
	CopyMemory(client, client, sizeof(PLAYERS));

	ReleaseMutex(mutex_2);
	ReleaseSemaphore(PodeLerBuffer, 1, NULL);

	return true;
}

BOOL Login(PLAYERS *client)
{
	hFile = CreateFile(NULL, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	hMem = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(PLAYERS), NULL);

	pShared = (PLAYERS *)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(PLAYERS));
		
	int pos;

	client->id = GetCurrentProcessId();
	_tprintf(TEXT("Username -> "));
	_tscanf_s(TEXT("%[^\n]s"), 19, client->username);

	WaitForSingleObject(PodeEscreverBuffer, INFINITE);
	WaitForSingleObject(mutex_2, INFINITE);

	pos = client->in;
	client->in = (client->in + 1) % Buffers;
	CopyMemory(pShared, client, sizeof(PLAYERS));

	ReleaseMutex(mutex_2);
	ReleaseSemaphore(PodeEscreverBuffer, 1, NULL);

	return true;
}

// MappedDLL.cpp : Defines the exported functions for the DLL application.
//
#include <windows.h>
#include "header.h"

PLAYERS player, *pJogador;

TCHAR (*PtrMemoria)
[Buffers][BufferSize];
TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semaphore_1");
TCHAR NomeSemaforoPodeLer[] = TEXT("Semaphore_2");

HANDLE PodeEscreverBuffer, PodeLerBuffer;
HANDLE hMemoria, hBuffer;
HANDLE mutex_1, mutex_2;

BOOL TesteDLL()
{
	printf("TesteServer");
	printf("\nIN: %d\n", pJogador->in);
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

	pos = pJogador->out;
	pJogador->out = (pJogador->out + 1) % Buffers;
	CopyMemory(client, pJogador, sizeof(PLAYERS));

	ReleaseMutex(mutex_1);
	ReleaseSemaphore(PodeEscreverBuffer, 1, NULL);

	return true;
}

BOOL SendMessages(PLAYERS *client)
{
	int pos;

	WaitForSingleObject(PodeEscreverBuffer, INFINITE);
	WaitForSingleObject(mutex_2, INFINITE);

	pos = pJogador->in;
	pJogador->in = (pJogador->in + 1) % Buffers;
	CopyMemory(pJogador, client, sizeof(PLAYERS));

	ReleaseMutex(mutex_2);
	ReleaseSemaphore(PodeLerBuffer, 1, NULL);

	return true;
}

BOOL Login(PLAYERS *client)
{

	int pos;

	client->id = GetCurrentProcessId();
	_tprintf(TEXT("Username -> "));
	_tscanf_s(TEXT("%[^\n]s"), 19, client->username);

	WaitForSingleObject(PodeEscreverBuffer, INFINITE);
	WaitForSingleObject(mutex_2, INFINITE);

	pos = pJogador->in;
	pJogador->in = (pJogador->in + 1) % Buffers;
	CopyMemory(pJogador, client, sizeof(PLAYERS));

	ReleaseMutex(mutex_2);
	ReleaseSemaphore(PodeEscreverBuffer, 1, NULL);

	return true;
}

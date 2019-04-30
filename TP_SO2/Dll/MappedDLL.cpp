// MappedDLL.cpp : Defines the exported functions for the DLL application.
//
#include <windows.h>
#include "header.h"

PLAYER player, *pJogador;

TCHAR (*PtrMemoria)
[Buffers][BufferSize];
TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semaphore_1");
TCHAR NomeSemaforoPodeLer[] = TEXT("Semaphore_2");

HANDLE PodeEscrever, PodeLer, PodeEscreverBuffer, PodeLerBuffer;
HANDLE hMemoria, hBuffer;
HANDLE mOut, mIn, mOutBuff, mInBuff;

BOOL TesteDLL()
{
	printf("TesteServer");
	printf("\nIN: %d\n", pJogador->in);
	return true;
}

BOOL ReadBuffer(PLAYER *client)
{
	int pos;

	WaitForSingleObject(PodeLerBuffer, INFINITE);
	WaitForSingleObject(mOutBuff, INFINITE);

	pos = pJogador->out;
	pJogador->out = (pJogador->out + 1) % Buffers;
	CopyMemory(client, pJogador, sizeof(PLAYER));

	ReleaseMutex(mOutBuff);
	ReleaseSemaphore(PodeEscreverBuffer, 1, NULL);

	return true;
}

BOOL WriteBuffer(PLAYER *client)
{
	int pos;

	WaitForSingleObject(PodeEscreverBuffer, INFINITE);
	WaitForSingleObject(mInBuff, INFINITE);

	pos = pJogador->in;
	pJogador->in = (pJogador->in + 1) % Buffers;
	CopyMemory(pJogador, client, sizeof(PLAYER));

	ReleaseMutex(mInBuff);
	ReleaseSemaphore(PodeLerBuffer, 1, NULL);

	return true;
}

BOOL LoginSequence(PLAYER *client)
{

	int pos;

	client->id = GetCurrentProcessId();
	_tprintf(TEXT("Username -> "));
	_tscanf_s(TEXT("%[^\n]s"), 19, client->username);

	WaitForSingleObject(PodeEscreverBuffer, INFINITE);
	WaitForSingleObject(mInBuff, INFINITE);

	pos = pJogador->in;
	pJogador->in = (pJogador->in + 1) % Buffers;
	CopyMemory(pJogador, client, sizeof(PLAYER));

	ReleaseMutex(mInBuff);
	ReleaseSemaphore(PodeEscreverBuffer, 1, NULL);

	return true;
}

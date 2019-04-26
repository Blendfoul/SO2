// MappedDLL.cpp : Defines the exported functions for the DLL application.
//
#include <windows.h>
#include "header.h"

PLAYER player, *pJogador;
DATACLI data, *pData;

TCHAR NomeMemoria[] = TEXT("Nome da Memória Partilhada");
TCHAR NomeBuffer[] = TEXT("Nome do Buffer");
TCHAR (*PtrMemoria)
[Buffers][BufferSize];
TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semáforo Pode Escrever");
TCHAR NomeSemaforoPodeLer[] = TEXT("Semáforo Pode Ler");
TCHAR NomePodeEscreverBuffer[] = TEXT("Semáforo Pode Escrever Buffer");
TCHAR NomePodeLerBuffer[] = TEXT("Semáforo Pode Ler Buffer");

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

BOOL ReadData(DATACLI *dataClient)
{
	int pos;
	
	WaitForSingleObject(PodeLer, INFINITE);
	WaitForSingleObject(mOut, INFINITE);
	
	pos = pData->out;
	pData->out = (pData->out + 1) % Buffers;
	CopyMemory(dataClient, pData, sizeof(DATACLI));
	
	ReleaseMutex(mOut);
	ReleaseSemaphore(PodeEscrever, 1, NULL);
	
	return true;
}

BOOL WriteData(DATACLI *dataClient)
{
	int pos;
	
	WaitForSingleObject(PodeEscrever, INFINITE);
	WaitForSingleObject(mIn, INFINITE);
	
	pos = pData->in;
	pData->in = (pData->in + 1) % Buffers;
	CopyMemory(pData, dataClient, sizeof(DATACLI));
	
	ReleaseMutex(mIn);
	ReleaseSemaphore(PodeLer, 1, NULL);
	
	return true;
}

BOOL LoginSequence(PLAYER *client) {
	
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

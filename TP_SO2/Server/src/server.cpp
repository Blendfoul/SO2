#define _UNICODE

#include "Server.h"
#include "../../Dll/header.h"

using namespace std;

int _tmain(int argc, LPSTR argv[])
{
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
#endif

    nPlayers = 0;
    LIVE = true;
    DWORD threadID;

    hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_1"));
    hCanWrite = CreateSemaphore(NULL, BUFFERS, BUFFERS, TEXT("Semaphore_1"));
    hCanRead = CreateSemaphore(NULL, 0, BUFFERS, TEXT("Semaphore_2"));
    hFile = CreateFile(NULL, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    hMem = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(PLAYERS), NULL);

    if (hCanWrite == NULL || hCanRead == NULL || hMem == NULL)
    {
        wcout << TEXT("Erro de criação de objectos do windows ") << GetLastError() << endl;
        _gettchar();
        return EXIT_FAILURE;
    }

    pBuf = (PLAYERS *)MapViewOfFile(hMem, FILE_MAP_WRITE, 0, 0, sizeof(PLAYERS));

    if (pBuf == NULL)
    {
        wcout << TEXT("Erro de criação da view of file ") << GetLastError() << endl;

        CloseHandle(hCanRead);
        CloseHandle(hCanWrite);
        CloseHandle(hMem);
        _gettchar();
        return EXIT_FAILURE;
    }

    hCons = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerInput, NULL, 0, &threadID);
    if (hCons != NULL)
        _tprintf(TEXT("Lancei uma thread com id %d"), threadID);
    else
    {
        _tprintf(TEXT("Erro ao criar Thread\n"));
        return -1;
    }

    WaitForSingleObject(hCons, INFINITE);
    WaitForSingleObject(hLogin, INFINITE);

    UnmapViewOfFile(pBuf);

    CloseHandle(hLogin);
    CloseHandle(hCons);
    CloseHandle(hCanRead);
    CloseHandle(hCanWrite);
    CloseHandle(hMem);
    CloseHandle(hFile);
    return EXIT_SUCCESS;
}

//TODO: Implementar consola para o servidor

DWORD WINAPI ServerInput()
{
    PLAYERS pAction;

    while (LIVE)
    {
        RecieveMessage(&pAction);
        HandleAction(pAction);
    };

    return 0;
}

//TODO: Comunicação servidor-cliente

BOOL HandleAction(PLAYERS pAction){
    BOOL validID = getPlayerId(pAction.id);
    BOOL validUsername = getPlayerUsername(pAction.username);

    if(!validID && !validUsername && !gameOn && nPlayers < MAX_PLAYERS){
        AddPlayerToArray(pAction.id, pAction.username);
        nPlayers++;
    }
    else if(validID && validUsername && !gameOn && nPlayers >= MAX_PLAYERS){
        DenyPlayerAcess();
    }

    BuildBroadcast();
}

//TODO: Adicionar Jogador ao Array de jogadores

BOOL AddPlayerToArray(int PlayerId, wchar_t * username) {
	players[nPlayers].id = PlayerId;
    _tcscpy(players[nPlayers].username, username);
	players[nPlayers].score = 0;
    players[nPlayers].code = USRVALID;

	return true;
}

int getPlayerId(int pid) {
	for (int i = 0; i < nPlayers; i++) {
		if (players[i].id == pid)
			return 1;
	}
	return 0;
}

int getPlayerUsername(TCHAR * nome) {
	for (int i = 0; i < nPlayers; i++) {
		if (_tcscmp(players[i].username, nome) == 0)
			return 1;
	}
	return 0;
}

//TODO: Negar acesso a um jogador

BOOL DenyPlayerAcess(){

}

//TODO: Envio de Broadcast

BOOL BuildBroadcast(){

}



//TODO: Lógica Jogo

DWORD BallMovement(){

}
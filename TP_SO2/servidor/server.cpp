#include "Server.h"

using namespace std;

int _tmain(int argc, TCHAR * argv[])
{
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
#endif

	nPlayers = 0;
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

    pBuf = (PLAYERS *)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(PLAYERS));

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

    CloseHandle(hCons);
    CloseHandle(hCanRead);
    CloseHandle(hCanWrite);
    CloseHandle(hMem);
    CloseHandle(hFile);

	SaveTopTen();
	
	return EXIT_SUCCESS;
}

//TODO: Implementar consola para o servidor

DWORD WINAPI ServerInput()
{
    PLAYERS pAction;

    while (1)
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
    return true;
}

//TODO: Adicionar Jogador ao Array de jogadores

BOOL AddPlayerToArray(int PlayerId, TCHAR * username) {
	players[nPlayers].id = PlayerId;
    _tcscpy_s(players[nPlayers].username, sizeof(players[nPlayers].username),username);
	players[nPlayers].score = 0;
    players[nPlayers].code = USRVALID;
	_tprintf(TEXT("Sucess\n"));
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

    return true;
}

//TODO: Envio de Broadcast

BOOL BuildBroadcast(){
    return true;
}



//TODO: Lógica Jogo

DWORD WINAPI BallMovement(){
    
    return 0;
}

//TODO: Top 10

void SaveTopTen() {
	int iResult, iNpreenchidos = 10, iSize, iValues, values[10] = { 0,10,20,30,40,50,60,70,80,90};
	HKEY hkChave;
	TCHAR nome[10][MAXT] = { TEXT("User 1"), TEXT("User 2"), TEXT("User 3"), TEXT("User 4"), TEXT("User 5"), TEXT("User 6"), TEXT("User 7"), TEXT("User 8"), TEXT("User 9"), TEXT("User 10") };
	TCHAR nomeAutor[MAXT];
	TCHAR tp[10][MAXT] = { TEXT("T1"), TEXT("T2"), TEXT("T3") , TEXT("T4") , TEXT("T5") , TEXT("T6") , TEXT("T7") , TEXT("T8") , TEXT("T9") , TEXT("T10") };

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Arkanoid"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkChave, (LPDWORD)&iResult) == ERROR_SUCCESS) {
		if (iResult == REG_CREATED_NEW_KEY) {
			_tprintf(TEXT("Chave criada com sucesso!"));
			//!Cria a nova chave do registro com os dados especificados
			for (int i = 0; i < iNpreenchidos; i++) {
				RegSetValueEx(hkChave, tp[i], 0, REG_SZ, (LPBYTE)&nome[i], (MAXT * sizeof(TCHAR)));
				RegSetValueEx(hkChave, nome[i], 0, REG_BINARY, (LPBYTE)&values[i], sizeof(int));
			}
			}
		else {
			for (int i = 0; i < iNpreenchidos; i++)
			{
				iSize = MAXT * sizeof(TCHAR);
				RegQueryValueEx(hkChave, tp[i], NULL, NULL, (LPBYTE)&nomeAutor, (LPDWORD)&iSize);
					iSize = sizeof(int);
				RegQueryValueEx(hkChave, nome[i], NULL, NULL, (LPBYTE)&iValues, (LPDWORD)&iSize);

				_tprintf(__T("Top %d -> Autor: %s Pontuação: %d\n"), i + 1,nomeAutor, iValues);
			}
		}
		RegCloseKey(hkChave);
	}
}


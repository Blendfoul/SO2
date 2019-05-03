#include "Server.h"

using namespace std;

SHAREDMEM message;
vector<PLAYERS> players(MAX_PLAYERS);

int _tmain(int argc, TCHAR *argv[])
{
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    nPlayers = 0;
    DWORD threadID;

    hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, TEXT("Mutex_1"));

    if (hMutex == NULL)
    {
        // no duplicate instances found
        hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_1"));
    }
    else
    {
        _tprintf(TEXT("There is an instance of the server already running. Closing server..."));

        _gettchar();

        return EXIT_FAILURE;
    }
	hMutexCanWrite = CreateMutex(NULL, FALSE, TEXT("Mutex_2"));
    hCanWrite = CreateSemaphore(NULL, BUFFERS, BUFFERS, TEXT("Semaphore_1"));
    hCanRead = CreateSemaphore(NULL, 0, BUFFERS, TEXT("Semaphore_2"));
    hFile = CreateFile(NULL, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    hMem = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(SHAREDMEM), TEXT("Shared_1"));

    if (hCanWrite == NULL || hCanRead == NULL || hMem == NULL)
    {
        wcout << TEXT("Erro de criação de objectos do windows ") << GetLastError() << endl;
        _gettchar();
        return EXIT_FAILURE;
    }

    pBuf = (SHAREDMEM *)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SHAREDMEM));

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
        _tprintf(TEXT("Lancei uma thread com id %d\n"), threadID);
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
	CloseHandle(hMutex);
	CloseHandle(hMutexCanWrite);

    SaveTopTen();

    return EXIT_SUCCESS;
}

//TODO: Implementar consola para o servidor

void PrintPlayers() {
	for (int i = 0; i < nPlayers; i++)
	{
		_tprintf(TEXT("%s\t%d\t%d\n"), players.at(i).username, players.at(i).id, players.at(i).code);
	}
}

DWORD WINAPI ServerInput()
{
	PLAYERS pAction;

    while (1)
    {
		PrintPlayers();
        pAction = RecieveRequest();
        HandleAction(pAction);
    };

    return 0;
}

//TODO: Comunicação servidor-cliente

BOOL HandleAction(PLAYERS pAction)
{
    BOOL validID = getPlayerId(pAction.id);
    BOOL validUsername = getPlayerUsername(pAction.username);

    if (!validID && !validUsername && !gameOn && nPlayers < MAX_PLAYERS)
    {
		pAction = AddPlayerToArray(&pAction);
		_tprintf(TEXT("%d"), pAction.code);
		nPlayers++;
    }
    else if (!validID && !validUsername && nPlayers >= MAX_PLAYERS)
    {
        DenyPlayerAcess();
    }
	else if (validID && validUsername && _tcscmp(pAction.command, TEXT("top10")) == 0) {
		SaveTopTen();
		return true;
	}

    BuildBroadcast(&pAction);
    return true;
}

PLAYERS RecieveRequest() {
	_tprintf(TEXT("Waiting for connection or requests\n"));
	WaitForSingleObject(hCanRead, INFINITE);
	WaitForSingleObject(hMutex, INFINITE);
	
	message = *pBuf;
	
	ReleaseMutex(hMutex);
	ReleaseSemaphore(hCanWrite, 1, NULL);
	
	_tprintf(TEXT("Client or request request received from %s %d!\n"), message.players[message.out].username, message.players[message.out].id);
	return message.players[message.in];
}

//TODO: Adicionar Jogador ao Array de jogadores

PLAYERS AddPlayerToArray(PLAYERS *pAction)
{
    pAction->score = 0;
    pAction->code = USRVALID;
	players[nPlayers] = *pAction;
    _tprintf(TEXT("Sucess %d\n"), pAction->code);
    return *pAction;
}

int getPlayerId(int pid)
{
    for (int i = 0; i < nPlayers; i++)
    {
        if (players[i].id == pid)
            return 1;
    }
    return 0;
}

int getPlayerUsername(TCHAR *nome)
{
    for (int i = 0; i < nPlayers; i++)
    {
        if (_tcscmp(players[i].username, nome) == 0)
            return 1;
    }
    return 0;
}

//TODO: Negar acesso a um jogador

BOOL DenyPlayerAcess()
{

    return true;
}

//TODO: Envio de Broadcast

BOOL BuildBroadcast(PLAYERS * pAction)
{	
	WaitForSingleObject(hCanWrite, INFINITE);
	WaitForSingleObject(hMutex, INFINITE);

	message.players[message.in] = *pAction;
	_tprintf(TEXT("MAD %s\t%d\n"), message.players[message.in].username, message.players[message.in].code);
	(message.in)++;
	CopyMemory(pBuf, &message, sizeof(SHAREDMEM));

	ReleaseMutex(hMutex);
	ReleaseSemaphore(hCanRead, 1, NULL);
    return true;
}

//TODO: Lógica Jogo

DWORD WINAPI BallMovement()
{

    return 0;
}

//TODO: Top 10

void SaveTopTen()
{
	int iSize, val;
    int iResult, iNpreenchidos = 10, values[10] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
    HKEY hkChave;
    TCHAR nome[10][MAXT] = {TEXT("User 1"), TEXT("User 2"), TEXT("User 3"), TEXT("User 4"), TEXT("User 5"), TEXT("User 6"), TEXT("User 7"), TEXT("User 8"), TEXT("User 9"), TEXT("User 10")};
    TCHAR tp[10][MAXT] = {TEXT("T1"), TEXT("T2"), TEXT("T3"), TEXT("T4"), TEXT("T5"), TEXT("T6"), TEXT("T7"), TEXT("T8"), TEXT("T9"), TEXT("T10")};
	TCHAR nomeAutor[MAXT];

    if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Arkanoid"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkChave, (LPDWORD)&iResult) == ERROR_SUCCESS)
    {
        if (iResult == REG_CREATED_NEW_KEY)
        {
            _tprintf(TEXT("Chave criada com sucesso!"));
            //!Cria a nova chave do registro com os dados especificados
            for (int i = 0; i < iNpreenchidos; i++)
            {
                RegSetValueEx(hkChave, tp[i], 0, REG_SZ, (LPBYTE)&nome[i], (MAXT * sizeof(TCHAR)));
                RegSetValueEx(hkChave, nome[i], 0, REG_BINARY, (LPBYTE)&values[i], sizeof(int));
            }
        }
		else if (iResult == REG_OPENED_EXISTING_KEY)
		{
			for (int i = 0; i < iNpreenchidos; i++)
			{
				iSize = MAXT * sizeof(TCHAR);
				RegQueryValueEx(hkChave, tp[i], NULL, NULL, (LPBYTE)&nomeAutor, (LPDWORD)&iSize);
				iSize = sizeof(int);
				RegQueryValueEx(hkChave, nome[i], NULL, NULL, (LPBYTE)&val, (LPDWORD)&iSize);

				_tprintf(__T("Top %d -> Autor: %s Pontuação: %d\n"), i + 1, nomeAutor, val);
			}
		}
        RegCloseKey(hkChave);
    }
}
#include "Server.h"

using namespace std;

SHAREDMEM message;
GAMEDATA gamedata;
PLAYERS *players;

int _tmain(int argc, TCHAR *argv[])
{
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    nPlayers = 0;
    DWORD threadID[3];
	LIVE = true;
	gamedata.in = 0;
	gamedata.out = 0;

    hMutex = OpenMutex(SYNCHRONIZE, false, TEXT("Mutex_1"));

    if (hMutex == NULL)
    {
        hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_1"));
    }
    else
    {
        _tprintf(TEXT("There is an instance of the server already running. Closing server..."));
		fgetwc(stdin);
        return EXIT_FAILURE;
    }

	players = (PLAYERS *) malloc(sizeof(PLAYERS) * MAX_PLAYERS);

	if (players == NULL) {
		_tprintf(TEXT("Error alocating array!\n"));
		return EXIT_FAILURE;
	}

    hMutexBroad = CreateMutex(NULL, FALSE, TEXT("Mutex_2"));
    hCanWrite = CreateSemaphore(NULL, BUFFERS, BUFFERS, TEXT("Semaphore_1"));
    hCanRead = CreateSemaphore(NULL, 0, BUFFERS, TEXT("Semaphore_2"));
    hCanWriteBroad = CreateSemaphore(NULL, BUFFERS, BUFFERS, TEXT("Semaphore_3"));
    hCanReadBroad = CreateSemaphore(NULL, 0, BUFFERS, TEXT("Semaphore_4"));
    hFilePlayers = CreateFile(NULL, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFilePlayers == NULL)
		return EXIT_FAILURE;
    hMemPlayers = CreateFileMapping(hFilePlayers, NULL, PAGE_READWRITE, 0, sizeof(SHAREDMEM), TEXT("Shared_1"));
	
	hFileGame = CreateFile(NULL, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	hMemGame = CreateFileMapping(hFileGame, NULL, PAGE_READWRITE, 0, sizeof(GAMEDATA), TEXT("Shared_2"));

    if (hCanWrite == NULL || hCanRead == NULL || hMemPlayers == NULL || hFileGame == NULL || hMemGame == NULL)
    {
		_tprintf(TEXT("Erro de criação de objectos do windows %lu\n"), GetLastError());
       //_gettchar();
        return EXIT_FAILURE;
    }

    pBuf = (SHAREDMEM *)MapViewOfFile(hMemPlayers, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SHAREDMEM));

    if (pBuf == NULL)
    {
        _tprintf(TEXT("Erro de criação da view of file %lu\n"), GetLastError());

        CloseHandle(hCanRead);
        CloseHandle(hCanWrite);
        CloseHandle(hMemPlayers);
		CloseHandle(hMemGame);
        return EXIT_FAILURE;
	}

	pGameDataShared = (GAMEDATA *)MapViewOfFile(hMemGame, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(GAMEDATA));

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Erro de criação da view of file %lu\n"), GetLastError());

		CloseHandle(hCanRead);
		CloseHandle(hCanWrite);
		CloseHandle(hMemPlayers);
		CloseHandle(hMemGame);
		_gettchar();
		return EXIT_FAILURE;
	}

	hInput = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerConsole, NULL, 0, &threadID[0]);

	if (hInput != NULL)
		_tprintf(TEXT("Lancei uma thread com id %d\n"), threadID[0]);
	else
	{
		_tprintf(TEXT("Erro ao criar Thread\n"));
		return EXIT_FAILURE;
	}

    hCons = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerInput, NULL, 0, &threadID[1]);

    if (hCons != NULL)
        _tprintf(TEXT("Lancei uma thread com id %d\n"), threadID[1]);
    else
    {
        _tprintf(TEXT("Erro ao criar Thread\n"));
        return EXIT_FAILURE;
    }

	hMovBola = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BallMovement, NULL, 0, &threadID[2]);

	if (hMovBola != NULL)
		_tprintf(TEXT("Lancei uma thread com id %d\n"), threadID[2]);
	else
	{
		_tprintf(TEXT("Erro ao criar Thread BallMovement"));
		return EXIT_FAILURE;
	}
	
	WaitForSingleObject(hInput, INFINITE);
    WaitForSingleObject(hCons, INFINITE);
    WaitForSingleObject(hMovBola, INFINITE);

    UnmapViewOfFile(pBuf);

    CloseHandle(hCons);
	CloseHandle(hMovBola);
	CloseHandle(hCanRead);
    CloseHandle(hCanWrite);
    CloseHandle(hMemPlayers);
    CloseHandle(hFilePlayers);
	CloseHandle(hMemGame);
	CloseHandle(hFileGame);
    CloseHandle(hMutex);
    CloseHandle(hMutexBroad);
	CloseHandle(hCanReadBroad);
	CloseHandle(hCanWriteBroad);

	free(players);

    return EXIT_SUCCESS;
}

//TODO: Implementar consola para o servidor

DWORD WINAPI ServerConsole() {
	TCHAR local[MAX];

	while (1) {
		fgetwc(stdin);

		_tprintf(TEXT("Command -> "));
		_tscanf_s(TEXT("%[^\n]s"), local, MAX - 1);

		if (_tcscmp(local, TEXT("close")) == 0) {
			message.players[message.in].code = SERVERCLOSE;
			BuildReply(&message.players[message.in]);
			ReleaseSemaphore(hCanWriteBroad, 1, NULL);
			LIVE = false;
			break;
		}
		else if(_tcscmp(local, TEXT("ball")) == 0)
				_tprintf(__T("BALL -> x: %d y: %d\n"), gamedata.ball[gamedata.in].x, gamedata.ball[gamedata.in].y);
		else if(_tcscmp(local, TEXT("users")) == 0)
			PrintPlayers();
	};

	//_tprintf(TEXT("%s %d\n"), local, LIVE);
	
	return 0;
}

void PrintPlayers()
{
    for (int i = 0; i < nPlayers; i++)
    {
        _tprintf(TEXT("%s\t%d\t%d\n"), players[i].username, players[i].id, players[i].code);
    }
}

DWORD WINAPI ServerInput()
{
    PLAYERS pAction;

    while (LIVE == true)
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

    //_tprintf(TEXT("ID: %d\tCommand: %s\n"), pAction.id, pAction.command);

    if (!validID && !validUsername && !gameOn && nPlayers < MAX_PLAYERS)
    {
        pAction = AddPlayerToArray(&pAction);
        //_tprintf(TEXT("%d"), pAction.code);
        nPlayers++;
    }
    else if (!validID && !validUsername && nPlayers >= MAX_PLAYERS)
    {
        pAction.code = USRINVALID;
    }
    else if (validID && validUsername && _tcscmp(pAction.command, TEXT("top10")) == 0)
    {
       pAction = SaveTopTen(&pAction);
	   //_tprintf(TEXT("%d\n"), pAction.top.points[2]);
    }

    else if (validID && validUsername && _tcscmp(pAction.command, TEXT("logout")) == 0)
    {
        RemovePlayerFromArray(&pAction);
        _tprintf(TEXT("Removed player: %s\n"), pAction.username);
        nPlayers--;
        pAction.code = LOGOUTSUCCESS;
    }

    BuildReply(&pAction);
    return true;
}

PLAYERS RecieveRequest()
{
    _tprintf(TEXT("Waiting for connection or requests\n"));
    WaitForSingleObject(hCanRead, INFINITE);
    WaitForSingleObject(hMutex, INFINITE);
	
	message = *pBuf;

	if (message.out == 10)
		message.out = 0;

	_tprintf(TEXT("ID: %d OUT: %d COMMAND: %s\n"), message.players[message.out].id, message.out, message.players[message.out].command);

    ReleaseMutex(hMutex);
    ReleaseSemaphore(hCanWrite, 1, NULL);

    _tprintf(TEXT("Client or request request received from %s %d!\n"), message.players[message.out].username, message.players[message.out].id);
    return message.players[message.in];
}

//TODO: Manage de Jogadores no Array de jogadores

BOOL RemovePlayerFromArray(PLAYERS *pPlayers)
{
    int pos = -1;

    for (int i = 0; i < MAX_PLAYERS; i++)
        if (pPlayers->id == players[i].id)
            pos = i;

    if (pos != -1)
    {
        //players.erase(players.begin() + pos);
		for (int i = pos; i < nPlayers; i++)
		{
			if ((i + 1) != nPlayers)
				players[i] = players[i + 1];
		}

        return true;
    }
    return false;
}

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

//TODO: Envio de respostas

BOOL BuildReply(PLAYERS *pAction)
{
    WaitForSingleObject(hCanWrite, INFINITE);
    WaitForSingleObject(hMutex, INFINITE);

	if(message.players[message.in].code == SERVERCLOSE)
		message.players[message.in + 2] = *pAction;
	else
		message.players[message.in] = *pAction;
	message.in = (message.in)++ % BUFFERS;
	if (message.in == 10)
		message.in = 0;
	CopyMemory(pBuf, &message, sizeof(SHAREDMEM));

	
	
    ReleaseMutex(hMutex);
    ReleaseSemaphore(hCanRead, 1, NULL);
    return true;
}

BOOL SendBroadcast(BALL *ball) {

	WaitForSingleObject(hCanWriteBroad, INFINITE);
	WaitForSingleObject(hMutexBroad, INFINITE);

	gamedata.ball[gamedata.in] = *ball;
	//_tprintf(__T("BALL -> x: %d y: %d IN: %d OUT: %d\n"), gamedata.ball[gamedata.in].x, gamedata.ball[gamedata.in].y, gamedata.in, gamedata.out);

	if (gamedata.out == 10)
		gamedata.out = 0;
	
	if (gamedata.in == 10)
		gamedata.in = 0;
	else
		gamedata.in++;

	CopyMemory(pGameDataShared, &gamedata, sizeof(GAMEDATA));

		gamedata.out++;

	ReleaseMutex(hMutexBroad);
	ReleaseSemaphore(hCanReadBroad, 1, NULL);
	return true;
}

//TODO: Lógica Jogo

//Tem controlo de esclusao mutua para a var Trajectory, que pode nao pode ser alterada
//no momento em que a thread está a ler o seu valor.
//
// para a versão meta 2, fazer consulta das posicoes dos blocos.
DWORD WINAPI BallMovement(LPVOID lparam)
{
	BALL ball = {};
	ball.x = MAX_SCREEN_WIDTH / 2;  // metade de 1 ecra HD, fica ao centro.
	ball.y = MAX_SCREEN_HEIGHT / 2; // metade de 1 ecra HD, fica ao centro.
	ball.trajectory = MOVE_BALL_UPRIGHT;
	ball.id = 0;

	while (LIVE == true) {

		// TODO: Ver com a posição dos tijolos.
		/*
		*	o Tricky disto é que mudamos a posição atual da bola e mudamos a trajetoria (para fazermos o proximo check, and so on...)
		*/
		switch (ball.trajectory)
		{
		case MOVE_BALL_UPRIGHT:
			
			if (ball.y - 1 == 0) 
			{
				ball.y++;//descer
				if (ball.x + 1 == MAX_SCREEN_WIDTH || 0) {
					ball.x--;							// obstaculo superior direito
					ball.trajectory = MOVE_BALL_DOWNLEFT;
				}
				else {
					ball.x++;							// obstaculo superior
					ball.trajectory = MOVE_BALL_DOWNRIGHT;
				}
				//_tprintf(__T("BALL -> X: %d Y: %d Traj: %d ID: %d\n"), ball.x, ball.y, ball.trajectory, ball.id);
				SendBroadcast(&ball);
			}
			else{
				ball.y--;//subir
				if (ball.x + 1 == MAX_SCREEN_WIDTH || 0) {
					ball.x--;							// obstaculo direito
					ball.trajectory = MOVE_BALL_UPLEFT;
				}
				else {									// sem obstaculo
					ball.x++;
					ball.trajectory = MOVE_BALL_UPRIGHT;
				}
				//_tprintf(__T("BALL -> X: %d Y: %d Traj: %d ID: %d\n"), ball.x, ball.y, ball.trajectory, ball.id);
				SendBroadcast(&ball);
			}
		break; 
		
		case MOVE_BALL_UPLEFT:
			
			if (ball.y - 1 == 0 || 0){ 
				ball.y++;
				if (ball.x - 1 == 0 || 0 ) {			// obstaculo superior esquerdo
					ball.x++;
					ball.trajectory = MOVE_BALL_DOWNRIGHT;
				}
				else {									// obstaculo superior
					ball.x--;
					ball.trajectory = MOVE_BALL_DOWNLEFT;
				}
				//_tprintf(__T("BALL -> X: %d Y: %d Traj: %d ID: %d\n"), ball.x, ball.y, ball.trajectory, ball.id);
				SendBroadcast(&ball);
			}
			else  
			{
				ball.y--;
				if (ball.x - 1 == 0 || 0) {			
					ball.x++;							// obstaculo esquerdo
					ball.trajectory = MOVE_BALL_UPRIGHT;
				}
				else {
					ball.x--;							// sem obstaculo
					ball.trajectory = MOVE_BALL_UPLEFT;
				}
				//_tprintf(__T("BALL -> X: %d Y: %d Traj: %d ID: %d\n"), ball.x, ball.y, ball.trajectory, ball.id);
				SendBroadcast(&ball);
			}

		case MOVE_BALL_DOWNRIGHT:
			
			if (ball.y + 1 == MAX_SCREEN_HEIGHT || 0) {
				ball.y--;
				if (ball.x + 1 == MAX_SCREEN_WIDTH || 0) {
					ball.x--;
					ball.trajectory = MOVE_BALL_UPLEFT;
				}
				else {
					ball.x++;
					ball.trajectory = MOVE_BALL_UPRIGHT;
				}
				//_tprintf(__T("BALL -> X: %d Y: %d Traj: %d ID: %d\n"), ball.x, ball.y, ball.trajectory, ball.id);
				SendBroadcast(&ball);
			}
			else {
				ball.y++;
				if (ball.x + 1 == MAX_SCREEN_WIDTH || 0) {
					ball.x--;
					ball.trajectory = MOVE_BALL_DOWNLEFT;
				}
				else {
					ball.x++;
					ball.trajectory = MOVE_BALL_DOWNRIGHT;
				}
				//_tprintf(__T("BALL -> X: %d Y: %d Traj: %d ID: %d\n"), ball.x, ball.y, ball.trajectory, ball.id);
				SendBroadcast(&ball);
			}
			break;

		case MOVE_BALL_DOWNLEFT:
			
			if (ball.y + 1 == MAX_SCREEN_HEIGHT || 0) {
				ball.y--;
				if (ball.x - 1 == 0 || 0) {
					ball.x++;
					ball.trajectory = MOVE_BALL_UPRIGHT;
				}
				else {
					ball.x--;
					ball.trajectory = MOVE_BALL_UPLEFT;
				}
				//_tprintf(__T("BALL -> X: %d Y: %d Traj: %d ID: %d\n"), ball.x, ball.y, ball.trajectory, ball.id);
				SendBroadcast(&ball);
			}
			else{
				ball.y++;
				if (ball.x - 1 == 0 || 0) {
					ball.x++;
					ball.trajectory = MOVE_BALL_DOWNRIGHT;
				}
				else {
					ball.x--;
					ball.trajectory = MOVE_BALL_DOWNLEFT;
				}
				//_tprintf(__T("BALL -> X: %d Y: %d Traj: %d ID: %d\n"), ball.x, ball.y, ball.trajectory, ball.id);
				SendBroadcast(&ball);
			}

		default:
			break;
		}

		
	}
	return 0;
}

//TODO: Top 10

PLAYERS SaveTopTen(PLAYERS *pAction)
{
    int iSize;
    int iResult, iNpreenchidos = 10, values[10] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
    HKEY hkChave;
    TCHAR nome[10][MAXT] = {TEXT("User 1"), TEXT("User 2"), TEXT("User 3"), TEXT("User 4"), TEXT("User 5"), TEXT("User 6"), TEXT("User 7"), TEXT("User 8"), TEXT("User 9"), TEXT("User 10")};
    TCHAR tp[10][MAXT] = {TEXT("T1"), TEXT("T2"), TEXT("T3"), TEXT("T4"), TEXT("T5"), TEXT("T6"), TEXT("T7"), TEXT("T8"), TEXT("T9"), TEXT("T10")};

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
                RegQueryValueEx(hkChave, tp[i], NULL, NULL, (LPBYTE)&pAction->top.names[i], (LPDWORD)&iSize);
                iSize = sizeof(int);
                RegQueryValueEx(hkChave, nome[i], NULL, NULL, (LPBYTE) &pAction->top.points[i], (LPDWORD) &iSize);
			}
        }
        RegCloseKey(hkChave);
    }

	return *pAction;
}
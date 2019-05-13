#include "client.h"

int _tmain() {

#ifdef UNICODE
	if (_setmode(_fileno(stdin), _O_WTEXT) &&
		_setmode(_fileno(stdout), _O_WTEXT) &&
		_setmode(_fileno(stderr), _O_WTEXT)) {
		_tprintf(TEXT("UNICODE ON!\n"));
	}
#endif

	DWORD threadID[2];
	LIVE = true;
	keypressed = false;
	
	aux.id = GetCurrentProcessId();
	aux.score = 0;
	_tprintf(TEXT("Username -> "));
	_tscanf_s(TEXT("%[^\n]s"),aux.username ,MAX);

	Login(&aux);
		
	aux = RecieveMessage(&aux);
	
	//_tprintf(TEXT("%s\t%d\t%d\n"), aux.username, aux.id, aux.code);

	if (aux.code == USRVALID)
		_tprintf(TEXT("Utilizador Válido!\n"));
	else {
		_tprintf(TEXT("Utilizador Inválido!\n"));
		return EXIT_FAILURE;
	}
	hConsole = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConsoleInput, NULL, 0, &threadID[0]);
	if (hConsole == NULL) {
		_tprintf(TEXT("Error creating thread\n"));
		return EXIT_FAILURE;
	}

	hBallControl = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Ball, NULL, 0, &threadID[1]);
	if (hBallControl == NULL) {
		_tprintf(TEXT("Error creating thread\n"));
		return EXIT_FAILURE;
	}

	WaitForSingleObject(hConsole, INFINITE);
	WaitForSingleObject(hBallControl, INFINITE);

	if (aux.code == LOGOUTSUCCESS)
		_tprintf(TEXT("Logout com sucesso Code: %d!\n"), aux.code);
	else
		_tprintf(TEXT("Logout Sem sucesso"));

	CloseVars();
	CloseHandle(hConsole);
	CloseHandle(hBallControl);
	
	return 0;
}

DWORD WINAPI ConsoleInput() {
	bool key = false;

	while (LIVE == true) {
	
		fgetwc(stdin);
		_tprintf_s(TEXT("Command -> "));
		_tscanf_s(TEXT("%[^\n]s"), aux.command, MAX);

		SendMessages(&aux);
		aux = RecieveMessage(&aux);

		if (aux.code == SERVERCLOSE) {
			_tprintf(TEXT("Server shutdown!\n"));
			LIVE = false;
			break;
		}

		if (_tcscmp(aux.command, TEXT("top10")) == 0) {
			for (int i = 0; i < 10; i++)
				_tprintf(__T("Top %d -> Autor: %s Pontuação: %d\n"), i + 1, aux.top.names[i], aux.top.points[i]);
		}
		else if (_tcscmp(aux.command, TEXT("logout")) == 0) {
			LIVE = false;
			break;
		}
		else if (_tcscmp(aux.command, TEXT("score")) == 0) {
			_tprintf_s(TEXT("Score -> "));
			_tscanf_s(TEXT("%d"), &aux.score);
			SendMessages(&aux);
			aux = RecieveMessage(&aux);
		}
		else if (_tcscmp(aux.command, TEXT("ball")) == 0) {
			//_tprintf(TEXT("AQUI!"));
		}
			
	};

	return 0;
}

DWORD WINAPI Ball() {
	TCHAR clean[MAX];
	clean[0] = '\0';

	while (LIVE == true) {
		game = RecieveBroadcast(&game);
		
		if (GetAsyncKeyState(VK_ESCAPE))
			_tcscpy_s(aux.command, _countof(clean), clean);
		else if (_tcscmp(aux.command, TEXT("ball")) == 0) {
			
			system("CLS");
			
			for (int i = 0; i < game.nBalls; i++) {
				gotoxy((int) 55, (int)5 + i);
				_tprintf(__T("BALL -> x: %d y: %d ID: %d\n"), game.ball[game.out][i].x, game.ball[game.out][i].y, game.ball[game.out][i].id);
			}
				Sleep(50);
			
		}
	};
	
	return 0;
}

void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
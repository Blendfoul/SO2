#include "client.h"

int tipo;

int _tmain()
{

#ifdef UNICODE
	if (_setmode(_fileno(stdin), _O_WTEXT) &&
		_setmode(_fileno(stdout), _O_WTEXT) &&
		_setmode(_fileno(stderr), _O_WTEXT))
	{
		_tprintf(TEXT("UNICODE ON!\n"));
	}
#endif

	DWORD threadID[2];
	LIVE = true;
	keypressed = false;
	

	aux.id = GetCurrentProcessId();
	aux.score = 0;
	_tprintf(TEXT("Username -> "));
	_tscanf_s(TEXT("%[^\n]s"), aux.username, MAX);

	_tprintf(TEXT("Tipo de conecção: "));
	_tscanf_s(TEXT("%d"), &tipo);

	switch (tipo)
	{
	case 0:
		Login(&aux);

		aux = RecieveMessage(&aux);

		//_tprintf(TEXT("%s\t%d\t%d\n"), aux.username, aux.id, aux.code);

		if (aux.code == USRVALID)
			_tprintf(TEXT("Utilizador Válido!\n"));
		else
		{
			_tprintf(TEXT("Utilizador Inválido!\n"));
			return EXIT_FAILURE;
		}
		hConsole = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConsoleInput, NULL, 0, &threadID[0]);
		if (hConsole == NULL)
		{
			_tprintf(TEXT("Error creating thread\n"));
			return EXIT_FAILURE;
		}

		hBallControl = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Ball, NULL, 0, &threadID[1]);
		if (hBallControl == NULL)
		{
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
		break;
	case 1:
		_tprintf(TEXT("Introduza endereço de IP: "));
		_tscanf_s(TEXT(" %[^\n]"), aux.ipAdress, IPSIZE);
		_tprintf(TEXT("IP: %s\n"), aux.ipAdress);

		Login(&aux, aux.ipAdress);
		aux = RecieveMessage(&aux, aux.ipAdress);

		if (aux.code == USRVALID)
			_tprintf(TEXT("Utilizador Válido!\n"));
		else
		{
			_tprintf(TEXT("Utilizador Inválido!\n"));
			return EXIT_FAILURE;
		}

		hConsole = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConsoleInput, NULL, 0, &threadID[0]);
		if (hConsole == NULL)
		{
			_tprintf(TEXT("Error creating thread\n"));
			return EXIT_FAILURE;
		}

		hBallControl = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Ball, NULL, 0, &threadID[1]);
		if (hBallControl == NULL)
		{
			_tprintf(TEXT("Error creating thread\n"));
			return EXIT_FAILURE;
		}

		WaitForSingleObject(hConsole, INFINITE);
		WaitForSingleObject(hBallControl, INFINITE);

		if (aux.code == LOGOUTSUCCESS)
			_tprintf(TEXT("Logout com sucesso Code: %d!\n"), aux.code);
		else
			_tprintf(TEXT("Logout Sem sucesso"));

		DisconnectPipes();
		break;
	}

	system("pause");

	CloseHandle(hConsole);
	CloseHandle(hBallControl);

	return 0;
}

DWORD WINAPI ConsoleInput(LPVOID param)
{
	bool key = false;
	//int tipo = (int)param;

	while (LIVE == true)
	{

		fgetwc(stdin);
		_tprintf_s(TEXT("Command -> "));
		_tscanf_s(TEXT("%[^\n]s"), aux.command, MAX);

		switch (tipo)
		{
		case 0:
			SendMessages(&aux);
			aux = RecieveMessage(&aux);
			break;
		case 1:
			SendMessages(&aux, aux.ipAdress);
			aux = RecieveMessage(&aux, aux.ipAdress);
			break;
		}

		if (aux.code == SERVERCLOSE)
		{
			_tprintf(TEXT("Server shutdown!\n"));
			LIVE = false;
			break;
		}

		if (_tcscmp(aux.command, TEXT("top10")) == 0)
		{
			for (int i = 0; i < 10; i++)
				_tprintf(__T("Top %d -> Autor: %s Pontuação: %d\n"), i + 1, aux.top.names[i], aux.top.points[i]);
		}
		else if (_tcscmp(aux.command, TEXT("logout")) == 0)
		{
			LIVE = false;
			break;
		}
		else if (_tcscmp(aux.command, TEXT("score")) == 0)
		{
			_tprintf_s(TEXT("Score -> "));
			_tscanf_s(TEXT("%d"), &aux.score);
			switch (tipo)
			{
			case 0:
				SendMessages(&aux);
				aux = RecieveMessage(&aux);
				break;
			case 1:
				SendMessages(&aux, aux.ipAdress);
				aux = RecieveMessage(&aux, aux.ipAdress);
			}
		}
		else if (_tcscmp(aux.command, TEXT("ball")) == 0)
		{
			//_tprintf(TEXT("AQUI!"));
		}
	};

	return 0;
}

DWORD WINAPI Ball(LPVOID param)
{
	//int tipo = (int)param;
	TCHAR clean[MAX];
	clean[0] = '\0';

	while (LIVE == true)
	{
		switch (tipo)
		{
		case 0:
			game = RecieveBroadcast(&game);
			break;
		case 1:
			//gameP = RecieveBroadcastPipe(&gameP, aux.ipAdress);
			break;
		}
		
		if (GetAsyncKeyState(VK_ESCAPE))
			_tcscpy_s(aux.command, _countof(clean), clean);
		else if (_tcscmp(aux.command, TEXT("ball")) == 0) {

			//system("CLS");

			switch (tipo) {
			case 0:
				for (int i = 0; i < game.nBalls; i++) {
					gotoxy((int)55, (int)5 + i);
					_tprintf(__T("BALL -> x: %d y: %d ID: %d\n"), game.ball[game.out][i].x, game.ball[game.out][i].y, game.ball[game.out][i].id);
				}
				Sleep(50);
				break;
			case 1:
				for (int i = 0; i < gameP.nBalls; i++) {
					gotoxy((int)55, (int)5 + i);
					_tprintf(__T("BALL -> x: %d y: %d ID: %d\n"), gameP.ball[i].x, gameP.ball[i].y, gameP.ball[i].id);
				}
				Sleep(50);
				break;
			}
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
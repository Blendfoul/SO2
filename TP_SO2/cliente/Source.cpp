#include "client.h"

int _tmain() {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	DWORD threadID[2];
	LIVE = true;
	
	aux.id = GetCurrentProcessId();
	_tprintf_s(TEXT("Username -> "), _tcslen(TEXT("Username -> ")));
	_tscanf_s(TEXT("%[^\n]s"),aux.username ,MAX);

	Login(&aux);
		
	aux = RecieveMessage(&aux);
	
	//_tprintf(TEXT("%s\t%d\t%d\n"), aux.username, aux.id, aux.code);

	if (aux.code == USRVALID)
		_tprintf(TEXT("Utilizador Válido!\n"));
	else {
		_tprintf(TEXT("Utilizador Inválido!\n"));
		_gettchar();
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
	
	while (LIVE == true) {
		if (aux.code == SERVERCLOSE) {
			_tprintf(TEXT("Server shutdown!\n"));
			LIVE = false;
			break;
		}

		fgetwc(stdin);
		_tprintf_s(TEXT("Command -> "), _tcslen(TEXT("Command -> ")));
		_tscanf_s(TEXT("%[^\n]s"), aux.command, MAX);

		SendMessages(&aux);
		aux = RecieveMessage(&aux);

		if (_tcscmp(aux.command, TEXT("top10")) == 0)
			for (int i = 0; i < 10; i++)
				_tprintf(__T("Top %d -> Autor: %s Pontuação: %d\n"), i + 1, aux.top.names[i], aux.top.points[i]);
		else if (_tcscmp(aux.command, TEXT("logout")) == 0) {
			LIVE = false;
			break;
		}
	};

	return 0;
}

DWORD WINAPI Ball() {
	while (LIVE == true) {
		game = RecieveBroadcast(&game);
	};
	
	return 0;
}
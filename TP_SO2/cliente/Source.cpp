#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "../MappedDLL/header.h"



int _tmain() {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	PLAYERS aux;
	
	aux.id = GetCurrentProcessId();
	_tprintf_s(TEXT("Username -> "), _tcslen(TEXT("Username -> ")));
	_tscanf_s(TEXT("%[^\n]s"),aux.username ,MAX);
	aux.command[0] = (TCHAR) TEXT("\0");
	Login(&aux);
		
	aux = RecieveMessage(&aux);
	
	fflush(stdin);
	_gettchar();

	_tprintf_s(TEXT("Command -> "), _tcslen(TEXT("Command -> ")));
	_tscanf_s(TEXT("%[^\n]s"), aux.command, MAX);

	_tprintf(TEXT("%s\t%d\t%d\n"), aux.username, aux.id, aux.code);

	if (aux.code == USRVALID)
		_tprintf(TEXT("Utilizador Válido!\n"));
	else {
		_tprintf(TEXT("Utilizador Inválido!\n"));
		return -1;
	}
	
	SendMessages(&aux);

	aux = RecieveMessage(&aux);

	for (int i = 0; i < 10; i++)
		_tprintf(__T("Top %d -> Autor: %s Pontuação: %d\n"), i + 1, aux.top.names[i], aux.top.points[i]);

	_gettchar();
	fgetwc(stdin);
	//_tcscpy_s(aux.command, sizeof(TEXT("logout")),TEXT("logout"));
	_tprintf_s(TEXT("Command -> "), _tcslen(TEXT("Command -> ")));
	_tscanf_s(TEXT("%[^\n]s"), aux.command, MAX);

	SendMessages(&aux);

	aux = RecieveMessage(&aux);

	if (aux.code == LOGOUTSUCCESS)
		_tprintf(TEXT("Logout com sucesso Code: %d!\n"), aux.code);
	else
		_tprintf(TEXT("Logout Sem sucesso"));


	CloseVars();
	Sleep(10000);

	
	return 0;
}
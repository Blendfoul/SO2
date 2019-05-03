#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "../MappedDLL/header.h"



int _tmain() {
	
	PLAYERS aux;
	
	aux.id = GetCurrentProcessId();
	_tprintf_s(TEXT("Username -> "), _tcslen(TEXT("Username -> ")));
	_tscanf_s(TEXT("%[^\n]s"),aux.username ,MAX);
	Login(&aux);
		
	aux = RecieveMessage(&aux);
	
	fflush(stdin);
	_gettchar();

	_tprintf(TEXT("%s\t%d\t%d\n"), aux.username, aux.id, aux.code);

	_tprintf_s(TEXT("Command -> "), _tcslen(TEXT("Command -> ")));
	_tscanf_s(TEXT("%[^\n]s"), aux.command, MAX);

	SendMessages(&aux);

	if (aux.code == USRVALID)
		_tprintf(TEXT("Utilizador Válido!\n"));
	else
		_tprintf(TEXT("Utilizador Inválido!\n"));

	Sleep(10000);

	_gettchar();
	fgetwc(stdin);
	return 0;
}
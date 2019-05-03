#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "../MappedDLL/header.h"

int _tmain() {
	
	SHAREDMEM player;
	
	_tprintf_s(TEXT("Username -> "), _tcslen(TEXT("Username -> ")));
	_tscanf_s(TEXT("%[^\n]s"), player.players[0].username ,MAX);
	Login(&player);
		
	Sleep(10000);

	_gettchar();
	return 0;
}
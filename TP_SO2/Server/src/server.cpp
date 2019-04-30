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

    wstring strLocal;
    nPlayers = 0;

    hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_1"));
    hCanWrite = CreateSemaphore(NULL, BUFFERS, BUFFERS, TEXT("Semaphore_1"));
    hCanRead = CreateSemaphore(NULL, 0, BUFFERS, TEXT("Semaphore_2"));
    hFile = CreateFile(NULL, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    hMem = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(PLAYER), NULL);

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

    hCons = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerInput, NULL, 0, NULL);

    WaitForSingleObject(hCons, INFINITE);

    UnmapViewOfFile(pBuf);
    CloseHandle(hCanRead);
    CloseHandle(hCanWrite);
    CloseHandle(hMem);
    CloseHandle(hFile);
    return EXIT_SUCCESS;
}

//TODO: Implementar consola para o servidor

DWORD WINAPI ServerInput()
{
    wstring cmd;
    do
    {
        fflush(stdin);

        wcout << TEXT("Input -> ");
        getline(wcin, cmd);

        if (cmd == TEXT("exit"))
        {
            LIVE = false;
            break;
        }

    } while (true);

    return 0;
}

//TODO: Comunicação servidor-cliente

//TODO: Logins

DWORD WINAPI LoginHandler(){

    do{


    
    }while(LIVE == true);

    return 0;
}

//TODO: Lógica Jogo

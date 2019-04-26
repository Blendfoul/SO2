#define _UNICODE

#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>

using namespace std;

#define MAX 256
#define BUFFERS 10

//structs to use in shared mem
typedef struct
{
    wchar_t msg[MAX];
} MEMSG;

typedef struct
{
    int in;
    int out;
    MEMSG arr[10];
} MEMPART;

//Global Variables
HANDLE hCanWrite, hCanRead, hMem, hFile;
HANDLE hMutex;
MEMPART *pBuf;

int _tmain(int argc, LPSTR *argv[])
{
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
#endif

    wstring strLocal;

    hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_1"));
    hCanWrite = CreateSemaphore(NULL, BUFFERS, BUFFERS, TEXT("Semaphore_1"));
    hCanRead = CreateSemaphore(NULL, 0, BUFFERS, TEXT("Semaphore_2"));
    hFile = CreateFile(TEXT("Shared_1.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    hMem = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(MEMPART), TEXT("Shared_1.txt"));

    if (hCanWrite == NULL || hCanRead == NULL || hMem == NULL)
    {
        wcout << TEXT("Erro de criação de objectos do windows ") << GetLastError() << endl;
        _gettchar();
        return EXIT_FAILURE;
    }

    pBuf = (MEMPART *)MapViewOfFile(hMem, FILE_MAP_WRITE, 0, 0, sizeof(MEMPART));

    if (pBuf == NULL)
    {
        wcout << TEXT("Erro de criação da view of file ") << GetLastError() << endl;

        CloseHandle(hCanRead);
        CloseHandle(hCanWrite);
        CloseHandle(hMem);
        _gettchar();
        return EXIT_FAILURE;
    }

    wcout << TEXT("[Produtor]Sou o produtor ") << GetCurrentThreadId() << (" e vou começar a trabalhar ...\n Prima \'fim\' para terminar...\n");
    MEMPART local;
    local.in = 0;
    local.out = 0;

    do
    {
        if (local.in == 10)
            local.in = 0;
        if (local.out == 10)
            local.out = 0;
        getline(wcin, strLocal);
        _tcscpy(local.arr[local.in].msg, strLocal.c_str());
        fflush(stdin);
        WaitForSingleObject(hCanWrite, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        local.in++;
        CopyMemory((PVOID)pBuf, (const void *)&local, sizeof(MEMPART));
        local.out++;
        ReleaseMutex(hMutex);
        ReleaseSemaphore(hCanRead, 4, NULL);

    } while (strLocal != TEXT("fim"));

    FlushViewOfFile(hFile, 0);

    UnmapViewOfFile(pBuf);
    CloseHandle(hCanRead);
    CloseHandle(hCanWrite);
    CloseHandle(hMem);
    CloseHandle(hFile);
    return EXIT_SUCCESS;
}
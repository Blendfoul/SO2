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

//Thread function
DWORD WINAPI ThreadConsumidor(LPVOID param);

//Global Variables
HANDLE hCanWrite, hCanRead, hMem;
HANDLE hMutex;
MEMPART *pBuf;

int _tmain(int argc, LPTSTR argv[])
{
    TCHAR resp;
    DWORD threadId;
    vector<HANDLE> hThreadCons(4);
    //UNICODE: Por defeito, a consola Windows não processe caracteres wide.
    //A maneira mais fácil para ter esta funcionalidade é chamar _setmode:

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
#endif

    hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_1"));
    hCanWrite = CreateSemaphore(NULL, BUFFERS, BUFFERS, TEXT("Semaphore_1"));
    hCanRead = CreateSemaphore(NULL, 0, BUFFERS, TEXT("Semaphore_2"));
    hMem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(MEMPART), TEXT("Shared_1.txt"));

    if (hCanWrite == NULL || hCanRead == NULL || hMem == NULL)
    {
        wcout << TEXT("Erro de criação de objectos do windows ") << GetLastError() << endl;
        _gettchar();
        return EXIT_FAILURE;
    }

    pBuf = (MEMPART *)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MEMPART));

    if (pBuf == NULL)
    {
        wcout << TEXT("Erro de criação da view of file ") << GetLastError() << endl;
        CloseHandle(hCanRead);
        CloseHandle(hCanWrite);
        CloseHandle(hMem);
        _gettchar();
        return EXIT_FAILURE;
    }

    wcout << TEXT("Lançar threads Consumidores?");
    wcin >> &resp;

    if (resp == 'S' || resp == 's')
    {

        for (int i = 0; i < hThreadCons.size(); i++)
        {
            hThreadCons[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadConsumidor, NULL, 0, &threadId);
            if (hThreadCons[i] != NULL)
                wcout << TEXT("Lancei uma thread com id ") << threadId << endl;
            else
            {
                wcout << TEXT("Erro ao criar Thread\n");
                return -1;

            }
        }

        WaitForMultipleObjects(hThreadCons.size(), hThreadCons.data(), TRUE, INFINITE);
        //CloseHandle(hThreadProd);
        for (int i = 0; i < hThreadCons.size(); i++)
        {
            CloseHandle(hThreadCons[i]);
        }
    }
    wcout << TEXT("[Thread Principal ") << GetCurrentThreadId() << TEXT("Finalmente vou terminar...");

    UnmapViewOfFile(pBuf);
    CloseHandle(hCanRead);
    CloseHandle(hCanWrite);
    CloseHandle(hMem);
    return 0;
}

DWORD WINAPI ThreadConsumidor(LPVOID param)
{
    wstring strLocal;

    wcout << TEXT("[Consumidor]Sou a thread ") << GetCurrentThreadId() << (" e vou começar a trabalhar ...\n");

    do
    {
        WaitForSingleObject(hCanRead, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        strLocal.assign(pBuf->arr[pBuf->out].msg);

        wcout << TEXT("[Consumidor ") << GetCurrentThreadId() << (" ]: ") << strLocal << endl;

        ReleaseMutex(hMutex);
        ReleaseSemaphore(hCanWrite, 1, NULL);
    } while (strLocal != TEXT("fim"));

    return EXIT_SUCCESS;
}
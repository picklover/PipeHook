#define _CRT_SECURE_NO_WARNINGS
#include <MinHook.h>
#include <Windows.h>
#include <stdio.h>
#include <shlobj.h>
#include <process.h>
#pragma comment(lib, "shell32.lib") 

HANDLE pipe_hnd;

typedef HANDLE (WINAPI*CREATENAMEDPIPEW)(
LPCWSTR               lpName,
DWORD                 dwOpenMode,
DWORD                 dwPipeMode,
DWORD                 nMaxInstances,
DWORD                 nOutBufferSize,
DWORD                 nInBufferSize,
DWORD                 nDefaultTimeOut,
LPSECURITY_ATTRIBUTES lpSecurityAttributes
);
CREATENAMEDPIPEW fpCreateNamedPipeW = NULL;

typedef  BOOL (WINAPI*READFILE)(
HANDLE       hFile,
LPVOID       lpBuffer,
DWORD        nNumberOfBytesToRead,
LPDWORD      lpNumberOfBytesRead,
LPOVERLAPPED lpOverlapped
);
READFILE fpReadFile = NULL;

typedef BOOL (WINAPI*WRITEFILE)(
HANDLE       hFile,
LPCVOID      lpBuffer,
DWORD        nNumberOfBytesToWrite,
LPDWORD      lpNumberOfBytesWritten,
LPOVERLAPPED lpOverlapped
);
WRITEFILE fpWriteFile;


BOOL WINAPI DetourReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
) {
	BOOL res = fpReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	if (pipe_hnd && pipe_hnd == hFile) {
		//log
		printf("Read[size:%d]\n", nNumberOfBytesToRead);
		for (int i = 0; i < nNumberOfBytesToRead; i++) {
			printf("%02x ", *((BYTE*)lpBuffer+i));
		}
		printf("\n\n");
		fflush(stdout);
	}
	return res;
}

BOOL WINAPI DetourWriteFile(
	HANDLE       hFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
) {
	BOOL res = fpWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	if (pipe_hnd && pipe_hnd == hFile) {
		//log
		printf("Write[size:%d]\n", nNumberOfBytesToWrite);
		for (int i = 0; i < nNumberOfBytesToWrite; i++) {
			printf("%02x ", *((BYTE*)lpBuffer+i));
		}
		printf("\n\n");
		fflush(stdout);
	}
	return res;
}

HANDLE WINAPI DetourCreateNamedPipeW(LPCWSTR  lpName,
	DWORD                 dwOpenMode,
	DWORD                 dwPipeMode,
	DWORD                 nMaxInstances,
	DWORD                 nOutBufferSize,
	DWORD                 nInBufferSize,
	DWORD                 nDefaultTimeOut,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	HANDLE handle = fpCreateNamedPipeW(lpName, dwOpenMode, dwPipeMode, nMaxInstances, nOutBufferSize, nInBufferSize, nDefaultTimeOut, lpSecurityAttributes);
	pipe_hnd = handle;
    return handle;
}


void doHook() {

	if (MH_Initialize() != MH_OK) {
		printf("min hook init fail!\n");
		fflush(stdout);
		return;
	}

	if (MH_CreateHook(&CreateNamedPipeW, &DetourCreateNamedPipeW, (LPVOID*)&fpCreateNamedPipeW) != MH_OK) {
		printf("min hook create hook faile\n");
		fflush(stdout);
		return;
	}
	if (MH_CreateHook(&ReadFile, &DetourReadFile, (LPVOID*)&fpReadFile) != MH_OK) {
		printf("min hook create hook faile\n");
		fflush(stdout);
		return;
	}
	if (MH_CreateHook(&WriteFile, &DetourWriteFile, (LPVOID*)&fpWriteFile) != MH_OK) {
		printf("min hook create hook faile\n");
		fflush(stdout);
		return;
	}

	MH_EnableHook(&CreateNamedPipeW);
	MH_EnableHook(&ReadFile);
	MH_EnableHook(&WriteFile);

	//MH_Uninitialize();
}

void initLog() {
	char path[255];
	SHGetSpecialFolderPathA(0, path, CSIDL_DESKTOPDIRECTORY, 0);
	strcat(path, "\\LogInfo.txt");
	freopen(path, "a", stdout);

	printf("PID: %d(%x)\n", _getpid(), _getpid());
	fflush(stdout);
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		initLog();
		doHook();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


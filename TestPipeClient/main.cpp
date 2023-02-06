#define  _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#define PIPE_NAME "\\\\.\\Pipe\\testpipe"

int main() {

	char buffer[1024] = "testWriteToPipe";
	DWORD ws;

	WaitNamedPipeA(PIPE_NAME, NMPWAIT_WAIT_FOREVER);
	HANDLE hPipe = CreateFileA(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("Connect success\n");

	while (1) {
		printf("write:%s\n", buffer);
		WriteFile(hPipe, buffer, strlen(buffer), &ws, NULL);
		Sleep(2000);
	}

	CloseHandle(hPipe);
	return 0;
}
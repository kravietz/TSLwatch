#include "stdafx.h"

void LogError(LPCSTR function, LPCWSTR operation) {
	LPWSTR lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
		|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR) &lpMsgBuf, 0, NULL );
	wprintf(L"%s: %s: %s\n", function, operation, lpMsgBuf);
}
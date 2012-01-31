#include "stdafx.h"

/* Log() is generally used to communicate with the user as TSLwatch doesn't have
user interface. The function writes to EventLog by default. If -foreground is specified
it writes to console. By default only summary information is written e.g. general
number of certificates fetched. If -debug is specified the program is verbose e.g. every
fetched certificate is reported
*/

// global variables
extern struct options options;
extern HANDLE hEventLog;

/*
BOOL Log(LPCSTR function, WORD wType, WORD wCategory, DWORD dwEventID,
		 PSID lpUserSid, WORD wNumStrings,  DWORD      dwDataSize,
		 (wNumStrings) LPCWSTR *lpStrings, (dwDataSize) LPVOID lpRawData ) {
			 LPWSTR lpMsgBuf;
			 DWORD dw = GetLastError();
			 BOOL doLog = false;

			 FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
				 |FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				 (LPTSTR) &lpMsgBuf, 0, NULL );

			 if(options.debug) {
				 doLog = true;
			 } else {
				 if(type == EVENTLOG_SUCCESS || type == EVENTLOG_INFORMATION_TYPE)
					 doLog = false;
				 else
					 doLog = true;
			 }

			 if(doLog == true && options.foreground == true) {
				 wprintf(L"%s\n", lpMsgBuf);
			 }
			 if(doLog == true && options.foreground == false {
				 ReportEvent(hEventLog, type, CAT_TSL_PARSER, NULL, 
			 }
}
*/
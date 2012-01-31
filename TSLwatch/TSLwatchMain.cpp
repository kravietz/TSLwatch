// TSLwatch.cpp : Defines the entry point for the console application.

#include "stdafx.h"

// global structured defined here
static struct options options;
static struct CertList GlobalCertList;
HANDLE hEventLog;

// external functions
extern BOOL insertIntoSystemStore(IN HCERTSTORE hSystemStore, IN LPTSTR certificate_base64);
extern BOOL parseTSL(LPCWSTR url, TCertListPtr pCertList);

int _tmain(int argc, _TCHAR* argv[]) {
	//--------------------------------------------------------------------
	// Declare and initialize variables.

	HCERTSTORE  hSystemStore;              // system store handle
	BOOL ret;
	INT i;

	for(i=1; i<argc; i++) {
		if(strncmp((const char*)argv[i], "-foreground", sizeof("foreground")) == 0)
			options.foreground = true;
		if(strncmp((const char*) argv[i], "-debug", sizeof("debug")) == 0)
			options.debug = true;
	}

	GlobalCertList.cert_list = NULL;
	GlobalCertList.cert_list_index = 0L;

	hEventLog = RegisterEventSource(NULL, L"TSLwatch");

	//hEventLog = OpenEventLog(NULL, L"Application");
	if(hEventLog == NULL) {
		GET_ERR(lpMsgBuf);
		wprintf(L"%s\n", lpMsgBuf);
		exit(1);
	}

	// notify about start
	ret = ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, CAT_GENERAL, TSLWATCH_START, 
		NULL, 0, 0, NULL, NULL);

	// we are adding certs to Trusted Root store on Local Machine
	// TSLwatch needs to run as a admin to access it
	// otherwise each certificate will result in a confirmation prompt
	hSystemStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, 
		CERT_SYSTEM_STORE_LOCAL_MACHINE, "Root");
	if(hSystemStore == NULL) 	{
		GET_ERR(lpMsgBuf);
		
		if(dw == ERROR_ACCESS_DENIED) { // most common cause
			ret = ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, 
				CAT_CERT_STORE, NEED_ADMIN, 
			NULL, 0, 0, NULL, NULL);
			exit(1);
		}

		// if not access denied then report
		LPWSTR pInsertStrings[2] = {NULL, NULL};

		pInsertStrings[0] = lpMsgBuf;
		ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, CAT_CERT_STORE, 
			GENERIC_ERROR, NULL, 1, 0, (LPCWSTR*)pInsertStrings, NULL);

		exit(1);
	}

#define EU_TSL  L"https://ec.europa.eu/information_society/policy/esignature/trusted-list/tl-mp.xml"
	
	ret = parseTSL(EU_TSL, &GlobalCertList);

	for(i=0;;) {
		LPTSTR cert;
		cert = GlobalCertList.cert_list[i];
		if(cert == NULL)
			break;
		insertIntoSystemStore(hSystemStore, cert);
		i += 1;
	}

	// When done using the store, close it.
	ret = CertCloseStore(hSystemStore, 0);
	if(FAILED(ret))
	{
		wprintf(L"Unable to close the CA system store.\n");
		exit(1);
	}

	ret = ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, CAT_GENERAL, TSLWATCH_STOP, 
		NULL, 0, 0, NULL, NULL);

	CloseEventLog(hEventLog);

	exit(0);
}


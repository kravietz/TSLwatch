// TSLwatch.cpp : Defines the entry point for the console application.

#include "stdafx.h"

// global structured defined here
static struct options options;			// from command line options
static struct CertList GlobalCertList;	// this stores list of certificates to add to store
static struct URLList GlobalURLList;
HANDLE hEventLog;						// for logging

// external functions
extern BOOL insertIntoSystemStore(IN HCERTSTORE hSystemStore, IN LPTSTR certificate_base64);
extern BOOL parseTSL(PSTR url, TCertListPtr pCertList, TURLListPtr URLListPtr);
extern BOOL addUrl(LPCSTR url, TURLListPtr *list);

int _tmain(int argc, _TCHAR* argv[]) {
	HCERTSTORE  hSystemStore;              // system store handle
	BOOL ret;
	DWORD i;

	// parse command line options
	for(i=1; i< (DWORD)argc; i++) {
		if(strncmp((const char*)argv[i], "-foreground", sizeof("foreground")) == 0)
			options.foreground = true;
		if(strncmp((const char*) argv[i], "-debug", sizeof("debug")) == 0)
			options.debug = true;
	}
	
	// initialize global certificate & URL list structure
	GlobalCertList.cert_list = NULL;
	GlobalCertList.count = 0L;
	GlobalURLList.url_list = NULL;
	GlobalURLList.count = 0L;

	// register us at EventLog
	hEventLog = RegisterEventSource(NULL, L"TSLwatch");
	if(hEventLog == NULL) {
		GET_ERR(lpMsgBuf);	// defined in tslwatch.h
		wprintf(L"%s\n", lpMsgBuf);
		exit(1);
	}

	// notify about start
	ret = ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, CAT_GENERAL, TSLWATCH_START, NULL, 0, 0, NULL, NULL);

	// we are adding certs to Trusted Root store on Local Machine
	// TSLwatch needs to run as a admin to access it
	// otherwise each certificate will result in a confirmation prompt
	//hSystemStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE, "Root");
	hSystemStore = CertOpenSystemStore(NULL, L"Disallowed");
	if(hSystemStore == NULL) 	{
		GET_ERR(lpMsgBuf);
		
		if(dw == ERROR_ACCESS_DENIED) { // most common cause
			ret = ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, CAT_CERT_STORE, NEED_ADMIN, NULL, 0, 0, NULL, NULL);
			exit(1);
		}

		// if not access denied then report
		LPWSTR pInsertStrings[2] = {NULL, NULL};

		pInsertStrings[0] = lpMsgBuf;
		ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, CAT_CERT_STORE, GENERIC_ERROR, NULL, 1, 0, (LPCWSTR*)pInsertStrings, NULL);

		exit(1);
	}

#define EU_TSL  "https://ec.europa.eu/information_society/policy/esignature/trusted-list/tl-mp.xml"
	
	/* initialize visited URL list with the starting TSL address
	   and recursively parse the list and its children */
	ret = parseTSL(EU_TSL, &GlobalCertList, &GlobalURLList);
	if(ret == false) {
		exit(1);
	}

	/* Walk through found certs and add them to the Windows store */
	for(i=0; i < GlobalCertList.count; i++) {
		PSTR cert;
		cert = GlobalCertList.cert_list[i];
		if(cert == NULL)
			break;
		printf("Adding to store %s\n", cert); 
		//XXX insertIntoSystemStore(hSystemStore, cert);
		
	}

	// When done using the store, close it.
	ret = CertCloseStore(hSystemStore, 0);
	if(FAILED(ret)) 	{
		GET_ERR(lpMsgBuf);
		LPWSTR pInsertStrings[2] = {NULL, NULL};

		pInsertStrings[0] = lpMsgBuf;
		ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, CAT_CERT_STORE, GENERIC_ERROR, NULL, 1, 0, (LPCWSTR*)pInsertStrings, NULL);
		
		exit(1);
	}

	// log termination
	ret = ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, CAT_GENERAL, TSLWATCH_STOP, NULL, 0, 0, NULL, NULL);

	CloseEventLog(hEventLog);

	exit(0);
}


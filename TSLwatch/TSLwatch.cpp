// TSLwatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern LPTSTR * parseTSL(void);
extern BOOL insertIntoSystemStore(HCERTSTORE hSystemStore, LPTSTR cert_base64);
extern void LogError(LPCSTR function, LPCWSTR operation);

int _tmain(int argc, _TCHAR* argv[])
{
	//--------------------------------------------------------------------
	// Declare and initialize variables.

	HCERTSTORE  hSystemStore;              // system store handle
	BOOL ret;
	LPTSTR *certificates_base64;

	//	hSystemStore = CertOpenSystemStore(0, L"Root"); // XXX Disallowed for testing
	hSystemStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE, "MY");
	if(SUCCEEDED(hSystemStore)) 	{
		wprintf(L"The CA system store is open. Continue.\n");
	} 	else 	{
		LogError(__FUNCTION__, L"CertOpenStore");
		wprintf(L"The CA system store did not open.\n");
		exit(1);
	}
	
	certificates_base64 = parseTSL();

	int i = 0;
	for(;;) {
		LPTSTR cert;
		cert = certificates_base64[i];
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

	return 0;
}


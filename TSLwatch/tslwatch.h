struct CertList {
	LPTSTR *cert_list;
	DWORD cert_list_index;
};

typedef struct CertList * TCertListPtr;

struct options {
	BOOL foreground;
	BOOL debug;
};

#define GET_ERR(lpMsgBuf) LPWSTR lpMsgBuf; DWORD dw = GetLastError(); FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );

// storage list for certificates extracted from all TSLs
struct CertList {
	// cert list if table of tchar pointers
	PSTR *cert_list;
	// number of items on the list
	DWORD count;
};

struct URLList {
	// URLs extracted from TSL used to avoid double checking the same TSL
	PSTR *url_list;
	// number of items on the list
	DWORD count;
};

typedef struct CertList * TCertListPtr;
typedef struct URLList	* TURLListPtr;

struct options {
	BOOL foreground;
	BOOL debug;
};

#define GET_ERR(lpMsgBuf) LPWSTR lpMsgBuf; DWORD dw = GetLastError(); FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );

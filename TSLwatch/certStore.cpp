/* Insert certificate into system certificate store using CryptoAPI.
 * Input: open certificate store pointer, BASE64 encoded certificate string.
 * Output: status
 */

#include "stdafx.h"

extern INT UnBase64(BYTE * dest, LPCTSTR src, INT srclen);

BOOL insertIntoSystemStore(IN HCERTSTORE hSystemStore, IN LPTSTR certificate_base64) {

	PCCERT_CONTEXT certificate_ctx; // CryptoAPI certificate context to extract cert name
	SIZE_T certificate_base64_len, certificate_asn1_len;
	BYTE * certificate_asn1;
	BOOL ret;

	//StringCchLength(certificate_base64, , certificate_base64_len);

	certificate_base64_len = wcslen( certificate_base64);

	// more than needed as BASE64 is always larger than binary data
	certificate_asn1 = (BYTE *) malloc(certificate_base64_len);

	certificate_asn1_len = UnBase64(certificate_asn1, certificate_base64, certificate_base64_len);
	if(certificate_asn1_len <= 0) {
		wprintf(L"%s: Base64 decode of certificate failed\n", __FUNCTION__);
	}

	certificate_ctx = CertCreateCertificateContext(X509_ASN_ENCODING, certificate_asn1, certificate_asn1_len);
	if(FAILED(certificate_ctx)) {
		//LogError(__FUNCTION__, L"CertCreateCertificateContext");
		return FALSE;
	}

	WCHAR pszNameString[256];
	DWORD	dwRet;

	dwRet = CertGetNameString(certificate_ctx, CERT_NAME_SIMPLE_DISPLAY_TYPE,
		0, NULL, // OIDs
		pszNameString, sizeof(pszNameString));
	if(FAILED(dwRet)) {
		//LogError(__FUNCTION__, L"CertGetNameString");
	}
	if(dwRet == 1) {
		wprintf(L"CertGetNameString: Certificate name not found\n");
		return FALSE;
	}
	if(dwRet > 0) {
		wprintf(L"%s: Certificate name: %s\n", _T(__FUNCTION__), pszNameString);
	}

	// no prompt if CERT_SYSTEM_STORE_LOCAL_MACHINE
	ret = CertAddEncodedCertificateToStore(hSystemStore, X509_ASN_ENCODING, certificate_asn1,
		certificate_asn1_len, CERT_STORE_ADD_NEW, NULL);
	if(!SUCCEEDED(ret)) {
		//LogError(__FUNCTION__, L"CertAddEncodedCertificateToStore");
		return FALSE;
	}

	return TRUE;
}
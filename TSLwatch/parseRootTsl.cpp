#include "stdafx.h"

#import <msxml6.dll>
using namespace MSXML2;

// external vars
extern HANDLE hEventLog;

// external functions
extern IXMLDOMDocument2Ptr TslFetch(PSTR url);
extern BOOL isKnownUrl(const char *url, TURLListPtr list);
extern BOOL addUrl(LPCSTR url, TURLListPtr *list);
extern BOOL addCert(LPCSTR cert, TCertListPtr * listPtr);
extern DWORD certCount(TCertListPtr list);
extern BOOL parseChildTsl(PSTR IN url, TCertListPtr IN pCertList, TURLListPtr IN URLListPtr);

// local functions
void dump_com_error(_com_error &e);

// These articles are helpful
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms754523(v=vs.85).aspx
// http://support.microsoft.com/kb/316317

BOOL parseRootTsl(PSTR IN url, TCertListPtr IN pCertList, TURLListPtr IN URLListPtr) 
{
	BOOL ret;
	
	IXMLDOMDocument2Ptr pXMLDoc;
	LPWSTR pInsertStrings[2] = {NULL, NULL};
	LPCWSTR searchTerm;
	MSXML2::IXMLDOMElementPtr pXMLDocElement = NULL;
	MSXML2::IXMLDOMNodeListPtr pXMLDomNodeList = NULL;
	INT i;

	pInsertStrings[0] = (LPWSTR) _strdup(url);
	ret = ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, CAT_TSL_PARSER, TSL_PARSE_START, NULL, 1, 0, (LPCWSTR*)pInsertStrings, NULL);

	printf("Opening %s...\n", url);

	// add currently processed URL
	ret = addUrl( url, &URLListPtr);

	// initialize XML trees
	CoInitialize(NULL);

	// attempt to fetch TSL file from Internet
	pXMLDoc = TslFetch(url);

	try{

#define DSIG_NS L"xmlns:tsl='http://uri.etsi.org/02231/v2#' xmlns:tslx='http://uri.etsi.org/02231/v2/additionaltypes#'"

		// Initialize XML parser
		pXMLDoc->async =  VARIANT_FALSE;
		pXMLDoc->setProperty("SelectionLanguage", "XPath");		// tell MSXML that we will use XPath syntax
		pXMLDoc->setProperty("SelectionNamespaces",	DSIG_NS);	// set namespace
		
		pXMLDocElement = pXMLDoc->documentElement;
	
		// Structure for root TSL (root=true)
		// OtherTSLPointer
		//	+- ServiceDigitalIdentities
		//		+- ServiceDigitalIdentity
		//			+- DigitalId
		//				+- X509Certificate	CERT - we add this to GlobalCertList
		//	+- TSLLocation					SUB-TSL - we parse this recursively
		//	+- AdditionalInformation
		//		+- OtherInformation*
		//			+- tslx:MimeType				MIME - need this to skip PDF files
		//      +- OtherInformation*
		//          +- SchemeOperatorName
		//              +- Name
		
		searchTerm = L"//tsl:TSLLocation[../tsl:AdditionalInformation/tsl:OtherInformation/tslx:MimeType='application/vnd.etsi.tsl+xml']";
		pXMLDomNodeList = pXMLDocElement->selectNodes(searchTerm);
		INT count = 0;
		count = pXMLDomNodeList->length;
		wprintf(L"XML query: %s\n", searchTerm);
		printf("Found %d nodes.\n", count);

		for(i=0; i<count; i++) {
			// process sub-TSL location
			MSXML2::IXMLDOMElementPtr current_elem = NULL;
			MSXML2::IXMLDOMElementPtr cert_elem = NULL;
			MSXML2::IXMLDOMElementPtr name_elem = NULL;
			BSTR tagName, newUrl;
			PSTR copyNewUrl;
			
			current_elem = pXMLDomNodeList->item[i]; // for root we are at TSLLocation level, for child at 
			tagName = current_elem->tagName;
			newUrl =  current_elem->text;

			copyNewUrl = (PSTR) malloc(wcslen(newUrl)*2);
			// convert URL to PSTR as this is what TslFetch expects later on
			WideCharToMultiByte(CP_ACP, 0, newUrl, -1, copyNewUrl, wcslen(newUrl)*2, NULL, NULL);

			// this part is recursively calling parseTSL()
			// need to check duplicate urls because most TSL will publish reference to EU TSL
			if(isKnownUrl((PSTR) copyNewUrl, URLListPtr) == false) {

				// fetch operator name for current certificate
				name_elem = current_elem->selectSingleNode(L"../tsl:AdditionalInformation/tsl:OtherInformation/tsl:SchemeOperatorName/tsl:Name");
				assert(name_elem != NULL);
				wprintf(L"Child TSL operator name=%s\n", (LPWSTR) name_elem->text);

				// if this TSL wasn't previously parse, do it now
				wprintf(L"Found new TSL %s=%s\n", (LPWSTR) tagName, (LPWSTR) newUrl);
				ret = parseChildTsl( copyNewUrl, pCertList, URLListPtr);

			}

			// fetch certificate
			/* this is not really needed until we verify TSL signatures
			cert_elem = current_elem->selectSingleNode(L"../tsl:ServiceDigitalIdentities/tsl:ServiceDigitalIdentity/tsl:DigitalId/tsl:X509Certificate");
			if(cert_elem == NULL) {
				printf("No certificate found in TSL: %s\n", copyNewUrl);
			} else {
				printf("Adding cert to urllist, size=%d\n", certCount(pCertList));
				// add certificate to global certificate list for further processing
				ret = addCert((PSTR) cert_elem->text, &pCertList);
			}
			*/

			if(certCount(pCertList) > 4) // XXX for faster debugging just process 4 certs
				return true;

		} /* end of XML parsing loop */

	} // end of try block

	catch(_com_error &e)
	{
		dump_com_error(e);
 		return false;
	}
	return true;
} // end of parseTSL

void dump_com_error(_com_error &e)
{
	wprintf(L"Error\n");
	wprintf(L"\a\tCode = %08lx\n", e.Error());
	wprintf(L"\a\tCode meaning = %s", e.ErrorMessage());
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	wprintf(L"\a\tSource = %s\n", (LPCSTR) bstrSource);
	wprintf(L"\a\tDescription = %s\n", (LPCSTR) bstrDescription);
}
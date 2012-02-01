#include "stdafx.h"

#import <msxml6.dll>
using namespace MSXML2;

// external functions
extern IXMLDOMDocument2Ptr TslFetch(LPCWSTR url);
extern HANDLE hEventLog;

void dump_com_error(_com_error &e);

#define DSIGNS L"xmlns:tsl='http://uri.etsi.org/02231/v2#' xmlns:tslx='http://uri.etsi.org/02231/v2/additionaltypes#'"

// These articles are helpful
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms754523(v=vs.85).aspx
// http://support.microsoft.com/kb/316317

BOOL parseTSL(LPCWSTR url, TCertListPtr pCertList) 
{
	IXMLDOMDocument2Ptr pXMLDoc;
	LPWSTR pInsertStrings[2] = {NULL, NULL};
	BOOL ret;
	LPCWSTR searchTerm;
	MSXML2::IXMLDOMElementPtr pXMLDocElement = NULL;
	MSXML2::IXMLDOMNodeListPtr pXMLDomNodeList = NULL;
	INT i;

	pInsertStrings[0] = StrDup(url);
	ret = ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, CAT_TSL_PARSER, TSL_PARSE_START, NULL, 1, 0, (LPCWSTR*)pInsertStrings, NULL);

	wprintf(L"Parsing %s...", url);

	// initialize XML trees
	CoInitialize(NULL);

	// attempt to fetch TSL file from Internet
	pXMLDoc = TslFetch(url);

	try{
		//IXMLDOMDocument2Ptr pXMLDoc = NULL;
		//HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));

		// Set parser property settings
		pXMLDoc->async =  VARIANT_FALSE;

		// Load the sample XML file
		//hr = pXMLDoc->load("PL_TSL.xml");

		// If document does not load report the parse error 
		/*
		if(hr!=VARIANT_TRUE)
		{
			MSXML2::IXMLDOMParseErrorPtr  pError;
			pError = pXMLDoc->parseError;
			_bstr_t parseError =_bstr_t("At line ")+ _bstr_t(pError->Getline())
				+ _bstr_t("\n")+ _bstr_t(pError->Getreason());
			//MessageBox(NULL,parseError, "Parse Error",MB_OK);
			return cert_list;
		}
		*/
		// Initialize XML parser
		pXMLDoc->setProperty("SelectionLanguage", "XPath");
		pXMLDoc->setProperty("SelectionNamespaces",	DSIGNS);
		
		pXMLDocElement = pXMLDoc->documentElement;
		
		// We will scan the list for sub-TSL locations and certificates separately.
		// It's because we can only parse sub-TSL posted as XML, but we want to add all certs

		// SCAN THE LIST SUB-TSL LOCATIONS
		
		// OtherTSLPointer
		//	+- ServiceDigitalIdentities
		//		+- ServiceDigitalIdentity
		//			+- DigitalId
		//				+- X509Certificate	CERT - we add this to GlobalCertList
		//	+- TSLLocation					SUB-TSL - we parse this recursively
		//	+- AdditionalInformation
		//		+- OtherInformation*
		//			+- tslx:MimeType				MIME - need this to skip PDF files

		// Retrieve sub-TSLs posted as XML
		searchTerm = L"//tsl:TSLLocation[../tsl:AdditionalInformation/tsl:OtherInformation/tslx:MimeType='application/vnd.etsi.tsl+xml']";
		pXMLDomNodeList = pXMLDocElement->selectNodes(searchTerm);
		INT count = 0;
		count = pXMLDomNodeList->length;
		printf("Found %d <tsl:OtherTSLPointer> nodes.\n", count);

		for(i=0; i<count; i++) {
			MSXML2::IXMLDOMElementPtr current_elem = NULL;
			MSXML2::IXMLDOMNodeListPtr subNodeList = NULL;
			MSXML2::IXMLDOMNodePtr mimeType = NULL, url = NULL;
			BSTR tagName;
			
			current_elem = pXMLDomNodeList->item[i]; // we are at OtherTSLPointer level
			tagName = current_elem->tagName;
			wprintf(L"tag=%s text=%s\n", (LPTSTR) tagName, (LPTSTR) current_elem->text);
			ret = parseTSL((LPTSTR) current_elem->text, pCertList);
			//subNodeList = current_elem->selectNodes(L"//tslx:MimeType");
			//mimeType = current_elem->selectSingleNode(L"//tsl:OtherTSLPointer/tsl:AdditionalInformation/tsl:OtherInformation[tslx:MimeType='application/vnd.etsi.tsl+xml']");
			//wprintf(L"MimeType=%s\n", (LPTSTR) mimeType->text);
			//mimeType = current_elem->selectSingleNode(L"tsl:AdditionalInformation/tsl:OtherInformation/tslx:MimeType");
			//wprintf(L"MimeType=%s\n", (LPTSTR) mimeType->text);
			//mimeType = current_elem->selectSingleNode(L"./tsl:AdditionalInformation/tsl:OtherInformation/tslx:MimeType");
			//wprintf(L"MimeType=%s\n", (LPTSTR) mimeType->text);
			//mimeType = current_elem->selectSingleNode(L"./tsl:AdditionalInformation/tsl:OtherInformation[tslx:MimeType=application/vnd.etsi.tsl+xml]");
			//url = current_elem->selectSingleNode(L"//tsl:TSLLocation");
			//subNodeList = current_elem->selectNodes(L"tslx:MimeType");
			//wprintf(L"MimeType=%s url=%s\n", (LPTSTR) mimeType->text, (LPTSTR) url->text);
			/* if(wcscmp( (LPWSTR) mimeType->text, L"application/vnd.etsi.tsl+xml") == 0) {
				wprintf(L"PROCESS");
			}*/
		}

		return false;

		// ADD FOUND CERTS TO OUR GLOBAL CERT LIST

		// Certificate list (re)allocation. This will be called recursively, so need
		// to update current values stored there.
		/*
		SIZE_T sizeNew = pCertList->cert_list_index; // for now count in pointers
		sizeNew += count; // add new certs
		sizeNew += 1; // terminating NULL
		sizeNew *= sizeof(LPSTR); // now count in bytes
		pCertList->cert_list = (LPTSTR *) realloc(pCertList->cert_list, sizeNew);

		for(i=0; i<2; i++) { // XXX s/3/count
			BSTR text;
			TCertListPtr p = pCertList;
			MSXML2::IXMLDOMElementPtr current_node;
				
			current_node = pXMLDomNodeList->item[i];
			p->cert_list_index++;
			text = current_node->text;
			p->cert_list[i] = StrDup((LPCTSTR) text);
			wprintf(_T("%s %d\n"), (LPTSTR) text, wcslen(text));
		}

		// SCAN THE LIST FOR SUB-TSL LOCATIONS



		//searchTerm =  L"//tsl:TSLLocation";
		searchTerm = L"//tsl:OtherTSLPointer";
		pXMLDomNodeList = pXMLDocElement->selectNodes(searchTerm);
		count = pXMLDomNodeList->length;
		printf("The number of <tsl:OtherTSLPointer> nodes is %i.\n", count);

		// FOR EACH NEW TSL RUN parseTsl() RECURSIVELY
		for(i=0; i<2; i++) { // XXX s/3/count
			BSTR text; // this will be sub-TSL url
			MSXML2::IXMLDOMElementPtr current_node = pXMLDomNodeList->item[i];
			text = current_node->text;
			wprintf(_T("%s %d\n"), (LPTSTR) text, wcslen(text));
		}
		*/

	}
	catch(_com_error &e)
	{
		dump_com_error(e);
 		return false;
	}
	return true;
}

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
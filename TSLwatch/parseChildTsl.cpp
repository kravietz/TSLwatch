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
extern void dump_com_error(_com_error &e);

// local functions
void dump_com_error(_com_error &e);

BOOL parseChildTsl(PSTR IN url, TCertListPtr IN pCertList, TURLListPtr IN URLListPtr) 
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

	printf("Opening child TSL %s...\n", url);

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
	
		// Structure for child TSL (root=false)
		//tsl:TrustServiceProvider
		//	tsl:TSPInformation
		//		tsl:TSPName
		//			tsl:Name[xml:lang='en']
		//	tsl:TSPServices
		//		tsl:TSPService
		//			tsl:ServiceInformation
		//				tsl:ServiceTypeIdentifier
		//				tsl:ServiceDigitalIdentity
		//					tsl:DigitalId
		//						tsl:X509Certificate
 
		searchTerm = L"//tsl:TSPService";
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
			MSXML2::IXMLDOMElementPtr service_elem = NULL;
			MSXML2::IXMLDOMElementPtr status_elem = NULL;
			
			current_elem = pXMLDomNodeList->item[i]; // at tsl:TSPService level
			wprintf(L"Found %s\n", (LPWSTR) current_elem->tagName);

			// fetch operator name for current certificate
			name_elem = current_elem->selectSingleNode(L"../../tsl:TSPInformation/tsl:TSPName/tsl:Name[@xml:lang='en']");
			assert(name_elem != NULL);
			wprintf(L"Operator name=%s\n", (LPWSTR) name_elem->text);

			// fetch service name identifier (QC, TSP etc)
			// Annex D.2 page 73 of ETSI TS 102 200
			status_elem = current_elem->selectSingleNode(L"tsl:ServiceInformation/tsl:ServiceTypeIdentifier");
			assert(status_elem != NULL);
			wprintf(L"Service id=%s\n", (LPWSTR) status_elem->text);

			// fetch service status identifier (active, discontinued etc)
			// Annex L.2 page 100 of ETSI TS 102 200
			service_elem = current_elem->selectSingleNode(L"tsl:ServiceInformation/tsl:ServiceStatus");
			assert(service_elem != NULL);
			wprintf(L"Service status=%s\n", (LPWSTR) service_elem->text);
			//assert( wcscmp(L"http://uri.etsi.org/TrstSvc/eSigDir-1999-93-EC-TrustedList/Svcstatus/undersupervision", status_elem->text) == 0);

			// fetch certificate
			cert_elem = current_elem->selectSingleNode(L"tsl:ServiceInformation/tsl:ServiceDigitalIdentity/tsl:DigitalId/tsl:X509Certificate");
			assert(cert_elem != NULL);
			printf("Adding cert to urllist, size=%d\n", certCount(pCertList));
			// add certificate to global certificate list for further processing
			ret = addCert((PSTR) cert_elem->text, &pCertList);

			if(certCount(pCertList) > 20) // XXX for faster debugging
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
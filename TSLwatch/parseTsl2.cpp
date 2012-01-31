#include "stdafx.h"

#import <msxml6.dll>
using namespace MSXML2;

// external functions
extern IXMLDOMDocument2Ptr TslFetch(LPCWSTR url);

void dump_com_error(_com_error &e);

#define DSIGNS L"xmlns:tsl='http://uri.etsi.org/02231/v2#'"

// These articles are helpful
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms754523(v=vs.85).aspx
// http://support.microsoft.com/kb/316317

BOOL parseTSL(LPCWSTR url, TCertListPtr pCertList) 
{
	IXMLDOMDocument2Ptr pXMLDoc;

	CoInitialize(NULL);
	pXMLDoc = TslFetch(url);

	try{
		//IXMLDOMDocument2Ptr pXMLDoc = NULL;
		//HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
		INT i;

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
		MSXML2::IXMLDOMElementPtr pXMLDocElement = NULL;
		pXMLDocElement = pXMLDoc->documentElement;
		MSXML2::IXMLDOMNodeListPtr pXMLDomNodeList = NULL;

#define	SEARCH_TERM "//tsl:X509Certificate"

		// find all matching nodes
		pXMLDomNodeList = pXMLDocElement->selectNodes(SEARCH_TERM);
		INT count = 0;
		count = pXMLDomNodeList->length;
		printf("The number of <tsl:X509Certificate> nodes is %i.\n", count);

		// UPDATE GLOBAL CERT LIST FROM THIS TSL

		// Certificate list allocation. This may be called recursively, so need
		// to update current values stored there.
		SIZE_T sizeNew = pCertList->cert_list_index; // for now count in pointers
		sizeNew += count; // add new certs
		sizeNew += 1; // terminating NULL
		sizeNew *= sizeof(LPSTR); // now count in bytes
		pCertList->cert_list = (LPTSTR *) realloc(pCertList->cert_list, sizeNew);

		for(i=0; i<count; i++) {
			BSTR text;
			TCertListPtr p = pCertList;

			p->cert_list_index++;
			MSXML2::IXMLDOMElementPtr current_node = pXMLDomNodeList->item[p->cert_list_index];
			text = current_node->text;
			p->cert_list[i] = StrDup((LPCTSTR) text);
			wprintf(_T("%s %d\n"), (LPTSTR) text, wcslen(text));
		}
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
	}
	return true;
}

void dump_com_error(_com_error &e)
{
	printf("Error\n");
	printf("\a\tCode = %08lx\n", e.Error());
	printf("\a\tCode meaning = %s", e.ErrorMessage());
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	printf("\a\tSource = %s\n", (LPCSTR) bstrSource);
	printf("\a\tDescription = %s\n", (LPCSTR) bstrDescription);
}
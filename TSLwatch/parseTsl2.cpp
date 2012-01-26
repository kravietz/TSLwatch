#include "stdafx.h"

#import <msxml6.dll>
using namespace MSXML2;

#define DSIGNS L"xmlns:tsl='http://uri.etsi.org/02231/v2#'"

void dump_com_error(_com_error &e);

// These articles are helpful
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms754523(v=vs.85).aspx
// http://support.microsoft.com/kb/316317

LPTSTR * parseTSL(void) 
{
	static LPTSTR * cert_list = NULL;

	CoInitialize(NULL);
	try{
		IXMLDOMDocument2Ptr pXMLDoc = NULL;
		HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
		int i;

		// Set parser property settings
		pXMLDoc->async =  VARIANT_FALSE;

		// Load the sample XML file
		hr = pXMLDoc->load("PL_TSL.xml");

		// If document does not load report the parse error 
		if(hr!=VARIANT_TRUE)
		{
			MSXML2::IXMLDOMParseErrorPtr  pError;
			pError = pXMLDoc->parseError;
			_bstr_t parseError =_bstr_t("At line ")+ _bstr_t(pError->Getline())
				+ _bstr_t("\n")+ _bstr_t(pError->Getreason());
			//MessageBox(NULL,parseError, "Parse Error",MB_OK);
			return cert_list;
		}
		// Otherwise, build node list using SelectNodes 
		// and returns its length as console output
		pXMLDoc->setProperty("SelectionLanguage", "XPath");
		// Set the selection namespace URI if the nodes
		// you wish to select later use a namespace prefix
		pXMLDoc->setProperty("SelectionNamespaces",	DSIGNS);
		MSXML2::IXMLDOMElementPtr pXMLDocElement = NULL;
		pXMLDocElement = pXMLDoc->documentElement;
		MSXML2::IXMLDOMNodeListPtr pXMLDomNodeList = NULL;
		pXMLDomNodeList = pXMLDocElement->selectNodes("//tsl:X509Certificate");
		int count = 0;
		count = pXMLDomNodeList->length;
		printf("The number of <tsl:X509Certificate> nodes is %i.\n", count);
		cert_list = (LPTSTR *) malloc(count * sizeof(LPSTR));
		for(i=0; i<count; i++) {
			BSTR text;
			MSXML2::IXMLDOMElementPtr current_node = pXMLDomNodeList->item[i];
			text = current_node->text;
			cert_list[i] = StrDup((LPCTSTR) text);
			wprintf(_T("%s %d\n"), (LPTSTR) text, wcslen(cert_list[i]));
		}
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
	}
	return cert_list;
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
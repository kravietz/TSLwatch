#include "stdafx.h"

#import <msxml6.dll>
using namespace MSXML2;

// defined in parseTsl
extern void dump_com_error(_com_error &e);

IXMLDOMDocument2Ptr TslFetch(LPCWSTR url) {
	HRESULT hr;
	BSTR bstrString = NULL;
	IXMLHTTPRequestPtr pIXMLHTTPRequest = NULL;
	IXMLDOMDocument2Ptr responseXML;
	LONG httpStatus;
	
	// CoInitialize() must be called in parent function
	try
	{
		hr=pIXMLHTTPRequest.CreateInstance(("MSXML2.XMLHTTP.6.0"));
		SUCCEEDED(hr) ? 0 : throw hr;

		hr=pIXMLHTTPRequest->open("GET", url, false);
		SUCCEEDED(hr) ? 0 : throw hr;

		hr=pIXMLHTTPRequest->send();
      SUCCEEDED(hr) ? 0 : throw hr;

	  hr = pIXMLHTTPRequest->get_status(&httpStatus);
	  SUCCEEDED(hr) ? 0 : throw hr;

      hr = pIXMLHTTPRequest->get_responseXML((IDispatch**) &responseXML);
	  SUCCEEDED(hr) ? 0 : throw hr;

	  return responseXML;

	}
	catch(_com_error &e)
	{
		dump_com_error(e);
	}
	// Release pIXMLHttpRequest when finished with it.

	return NULL;
};
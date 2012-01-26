#include "stdafx.h"

static const TCHAR  table[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const INT   BASE64_INPUT_SIZE = 57;

BOOL isbase64(TCHAR c)
{
	return c && StrChr(table, c) != NULL;
}

TCHAR value(TCHAR c)
{
	const TCHAR *p = StrChr(table, c);
	if(p) {
		return p-table;
	} else {
		return 0;
	}
}

INT UnBase64(BYTE * dest, LPCTSTR src, INT srclen)
{
	BYTE * p = dest;
	*dest = 0;
	if(*src == 0) 
	{
		return 0;
	}
	do
	{

		TCHAR a = value(src[0]);
		TCHAR b = value(src[1]);
		TCHAR c = value(src[2]);
		TCHAR d = value(src[3]);
		*p++ = (a << 2) | (b >> 4);
		*p++ = (b << 4) | (c >> 2);
		*p++ = (c << 6) | d;
		if(!isbase64(src[1])) 
		{
			p -= 2;
			break;
		} 
		else if(!isbase64(src[2])) 
		{
			p -= 2;
			break;
		} 
		else if(!isbase64(src[3])) 
		{
			p--;
			break;
		}
		src += 4;
		while(*src && (*src == 13 || *src == 10)) src++;
	}
	while(srclen-= 4);
	*p = 0;
	return p-dest;
}
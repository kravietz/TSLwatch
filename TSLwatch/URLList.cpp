#include "stdafx.h"

/*
INT urlCount(TURLListPtr list) {
	INT i;
	if(list == NULL)
		return 0;
	for(i=0;true;i++) {
		if(list[i] == NULL)
			return i;
	}
} // urlCount
*/

DWORD urlCount(TURLListPtr list) {
	return list->count;
}

BOOL isKnownUrl(LPCSTR url, TURLListPtr list) {
	DWORD i;
	DWORD count = list->count;
	for(i=0; i<count; i++) {
		if(list->url_list[i] == NULL)
			return false;
		else {
			if( strcmp(list->url_list[i], url) == 0)
				return true;
		}
	}
	return false;
	/*
	if(list == NULL)
		return false;
	for(i=0;true;i++) {
		if(list[i] == NULL)
			return false; // this means we haven't found a match yet
		if( strcmp((const char *) list[i],  url) == 0)
			return true;
	}
	*/
} // isKnownUrl

// we need to replace the global list here
// so ** is needed
BOOL addUrl(LPCSTR url, TURLListPtr * listPtr) {
	DWORD count;
	SIZE_T byteSize, oldByteSize;
	TURLListPtr list;
	PSTR *new_list;
	PSTR *old_list;

	list = *listPtr;
	old_list = list->url_list;

	count = list->count;
	oldByteSize = count * sizeof(PSTR);
	count += 1; // grow for new URL
	byteSize = count * sizeof(PSTR);

	new_list = (PSTR *) realloc((void *) old_list, byteSize);
	if(new_list == NULL)
		return false;

	memcpy(new_list, old_list, oldByteSize);
	new_list[count-1] = _strdup(url);

	(*listPtr)->url_list = new_list;
	(*listPtr)->count = count;
	return true;
} // addUrl
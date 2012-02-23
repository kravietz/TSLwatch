#include "stdafx.h"


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
} // isKnownUrl

// we need to replace the global list here
// so ** is needed
BOOL addUrl(LPCSTR url, TURLListPtr * listPtr) {
	DWORD count, i;
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

	// allocate new list
	new_list = (PSTR *) realloc((void *) old_list, byteSize);
	if(new_list == NULL)
		return false;

	/* copy old pointers to new list */
	for(i=0; i<count-1; i++) {
		new_list[count] = old_list[count];
	}
	// add the new url
	new_list[count-1] = _strdup(url);

	// replace the list at its original pointer
	(*listPtr)->url_list = new_list;
	(*listPtr)->count = count;
	return true;
} // addUrl
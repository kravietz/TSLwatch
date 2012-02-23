#include "stdafx.h"

// we need to replace the global list here
// so ** is needed
BOOL addCert(LPCSTR cert, TCertListPtr * listPtr) {
	DWORD count, i;
	SIZE_T byteSize, oldByteSize;
	TCertListPtr list;
	PSTR *new_list;
	PSTR *old_list;

	list = *listPtr;
	old_list = list->cert_list;

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
	new_list[count-1] = _strdup(cert);

	// replace the list at its original pointer
	(*listPtr)->cert_list = new_list;
	(*listPtr)->count = count;
	return true;
} // addUrl
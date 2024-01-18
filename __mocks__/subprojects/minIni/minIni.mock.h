#ifndef MININI_MOCK_H
#define MININI_MOCK_H

#include <bdd-for-c-mocks.h>
#include "../../../src/packages/common/IZ_common.h"

mock(ini_getl) long ini_getl(const char *Section, const char *Key, long DefValue, const char *Filename) {
	mock_return(ini_getl) DefValue;
}

mock(ini_putl) i32 ini_putl(const char *Section, const char *Key, long Value, const char *Filename) {
	mock_return(ini_putl) 1;
}

mock(ini_gets) i32 ini_gets(
	const char *Section,
	const char *Key,
	const char *DefValue,
	char *Buffer,
	i32 BufferSize,
	const char *Filename
) {
	mock_return(ini_gets) 0;
}

mock(ini_puts) i32 ini_puts(const char *Section, const char *Key, const char *Value, const char *Filename) {
	mock_return(ini_puts) 1;
}

#endif

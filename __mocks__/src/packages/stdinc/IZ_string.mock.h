#ifndef IZ_STRING_MOCK_C
#define IZ_STRING_MOCK_C

#include <bdd-for-c-mocks.h>
#include "../../../src/packages/compat/IZ_compat.h"

mock(IZ_memset) void* IZ_memset(void* dst, int c, size_t len) {
	mock_return(IZ_memset) 0;
}

mock(IZ_memcpy) errno_t IZ_memcpy(void* const dest, const rsize_t dest_size, const void* const source, const rsize_t source_size) {
	mock_return(IZ_memcpy) 0;
}

mock(IZ_strlwr) errno_t IZ_strlwr(char* dest, const char* str, rsize_t str_size) {
	mock_return(IZ_strlwr) 0;
}

#endif

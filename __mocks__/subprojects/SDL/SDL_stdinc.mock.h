#ifndef SDL_STDINC_MOCK_H
#define SDL_STDINC_MOCK_H

#include <string.h>
#include <bdd-for-c-mocks.h>
#include "../../../src/packages/common/IZ_common.h"

mock(SDL_memcpy) void* SDL_memcpy(void* dst, const void* src, size_t len) {
	mock_return(SDL_memcpy) memcpy(dst, src, len);
}

mock(SDL_memset) void* SDL_memset(void* dst, i32 c, size_t len) {
	mock_return(SDL_memset) memset(dst, c, len);
}

mock(SDL_malloc) void* SDL_malloc(size_t size) {
	mock_return(SDL_malloc) malloc(size);
}

mock(SDL_free) void SDL_free(void* mem) {
	mock_return(SDL_free);
}

mock(SDL_calloc) void* SDL_calloc(size_t num, size_t size) {
	mock_return(SDL_calloc) calloc(num, size);
}

#endif

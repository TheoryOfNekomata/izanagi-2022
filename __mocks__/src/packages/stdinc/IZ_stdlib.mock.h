#ifndef IZ_STDLIB_MOCK_C
#define IZ_STDLIB_MOCK_C

#include <bdd-for-c-mocks.h>
#include <SDL_stdinc.h>

mock(IZ_malloc) void* IZ_malloc(size_t size) {
	mock_return(IZ_malloc) SDL_malloc(size);
}

mock_modes(IZ_free) {
	IZ_FREE_CALLS_TRACKED = 0,
	IZ_FREE_CALLS_UNTRACKED,
};

mock(IZ_free) void IZ_free(void* mem) {
	mock_mode_if(IZ_free, IZ_FREE_CALLS_TRACKED) {
		mock_return(IZ_free) SDL_free(mem);
	} else mock_mode_if(IZ_free, IZ_FREE_CALLS_UNTRACKED) {
		SDL_free(mem);
	}
}

mock(IZ_calloc) void* IZ_calloc(unsigned int num, size_t size) {
	mock_return(IZ_calloc) SDL_calloc(num, size);
}

#endif

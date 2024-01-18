#include "IZ_stdlib.h"

void* IZ_malloc(size_t size) {
	return SDL_malloc(size);
}

void* IZ_calloc(unsigned int count, size_t size) {
	return SDL_calloc(count, size);
}

void IZ_free(void* p) {
	SDL_free(p);
	p = NULL;
}

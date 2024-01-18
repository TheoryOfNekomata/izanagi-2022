#ifndef IZ_STDLIB_H
#define IZ_STDLIB_H

#include <SDL2/SDL_stdinc.h>

void* IZ_malloc(size_t);
void* IZ_calloc(unsigned int, size_t);
void IZ_free(void*);

#endif //IZ_STDLIB_H

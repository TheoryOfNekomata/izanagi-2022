#ifndef IZ_STRING_H
#define IZ_STRING_H

#include <string.h>
#if defined IZ_WINDOWS
#include <io.h>
#include <SDL2/SDL_syswm.h>
#elif defined IZ_MACOS
#include <unistd.h>
#elif defined IZ_UNIX
#include "../../src/packages/compat/IZ_compat.h"
#endif

errno_t IZ_memcpy(void*, rsize_t, const void*, rsize_t);

errno_t IZ_strcat(char*, rsize_t, const char*);

errno_t IZ_strlwr(char*, const char*, rsize_t);

void* IZ_memset(void*, int, rsize_t);

const char* IZ_contains(const char*, rsize_t, const char*, rsize_t);

#endif

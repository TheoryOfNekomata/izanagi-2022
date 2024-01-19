#ifndef IZ_IO_H
#define IZ_IO_H

#if defined IZ_WINDOWS
#include <io.h>
#include "SDL_syswm.h"
#elif defined IZ_UNIX
#include <sys/stat.h>
#elif defined IZ_MACOS
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include "../compat/IZ_compat.h"

int IZ_sprintf(char*, size_t, const char*, ...);

errno_t IZ_fopen(FILE**, const char*, const char*);

errno_t IZ_mkdir(const char*);

bool IZ_isdir(const char*);

#endif

#ifndef IZ_LOG_H
#define IZ_LOG_H

#define BOLD  "\x1B[0;1m"  // Bold White
#define RED   "\x1B[0;31m"
#define GRN   "\x1B[0;32m"
#define YEL   "\x1B[0;33m"
#define BLU   "\x1B[0;34m"
#define MAG   "\x1B[0;35m"
#define CYN   "\x1B[0;36m"
#define WHT   "\x1B[0;37m"
#define RESET "\x1B[0m"

#define IZ_LOG_LEVEL_FLAG_DEBUG
#define IZ_LOG_LEVEL_FLAG_INFO
#define IZ_LOG_LEVEL_FLAG_WARN
#define IZ_LOG_LEVEL_FLAG_ERROR

#define IZ_LOG_WIDTH_TIMESTAMP "24"
#define IZ_LOG_WIDTH_CATEGORY "14"

#include <SDL2/SDL_filesystem.h>
#if defined IZ_WINDOWS
#include <io.h>
#include <SDL2/SDL_syswm.h>
#elif defined IZ_MACOS
#include <unistd.h>
#elif defined IZ_UNIX
#include <unistd.h>
#endif

#include "../compat/IZ_compat.h"
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../timer/IZ_timer.h"
#include "../io/IZ_io.h"
#include "../stdinc/IZ_string.h"

typedef enum {
	IZ_LOG_CATEGORY_INPUT,
	IZ_LOG_CATEGORY_GLOBAL,
	IZ_LOG_CATEGORY_GENERIC,
} IZ_LogCategory;

static FILE* stdout_dest;
static FILE* stderr_dest;

void IZ_LogInitialize(const char*, bool);
void IZ_LogError(const char*, const char*, ...);
void IZ_LogInfo(IZ_LogCategory, const char*, const char*, ...);
void IZ_LogWarn(bool, const char*, const char*, ...);
void IZ_Log(const char*, const char*, ...);
void IZ_LogTeardown(void);

#endif

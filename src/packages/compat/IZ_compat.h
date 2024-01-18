#ifndef IZ_COMPAT_H
#define IZ_COMPAT_H

#if defined IZ_WINDOWS
#define isatty _isatty
#define fileno _fileno
#endif

#if defined IZ_UNIX
typedef unsigned long size_t;
typedef size_t rsize_t;
typedef int errno_t;
#endif

#if defined IZ_MACOS
typedef int errno_t;
#endif

#endif //IZ_COMPAT_H

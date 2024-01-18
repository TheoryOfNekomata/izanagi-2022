#include "IZ_io.h"

int IZ_sprintf(char* buffer, size_t buffer_size, const char* format, ...) {
#if defined IZ_WINDOWS
	va_list args;
	va_start(args, format);
	int result = sprintf_s(buffer, buffer_size, format, args);
	va_end(args);
	return result;
#else
	va_list args;
	va_start(args, format);
	int result = sprintf(buffer, format, args);
	va_end(args);
	return result;
#endif
}

errno_t IZ_fopen(FILE** file, const char* filename, const char* mode) {
#if defined IZ_WINDOWS
	return fopen_s(file, filename, mode);
#else
	*file = fopen(filename, mode);
	return *file == NULL ? 1 : 0;
#endif
}

errno_t IZ_mkdir(const char* path) {
#if defined IZ_WINDOWS
	if (!CreateDirectory(path, NULL)) {
		return GetLastError();
	}
	return 0;
#else
	mkdir(path, 0777);
	return 0;
#endif
}

bool IZ_isdir(const char* path) {
#if defined IZ_WINDOWS
	DWORD dwAttrib = GetFileAttributes(path);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
					(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
	struct stat s;
	int err = stat("/path/to/possible_dir", &s);
	if(-1 == err) {
        return false;
	}
	return (S_ISDIR(s.st_mode));
#endif
}

#include "IZ_string.h"

errno_t IZ_memcpy(void* const dest, const rsize_t dest_size, const void* const source, const rsize_t source_size) {
#if defined IZ_WINDOWS
	return memcpy_s(dest, dest_size, source, source_size);
#else
	memcpy(dest, source, dest_size);
	return 0;
#endif
}

errno_t IZ_strcat(char* dest, rsize_t source_size, const char* source) {
#if defined IZ_WINDOWS
	return strcat_s(dest, source_size, source);
#else
	strcat(dest, source);
	return 0;
#endif
}

errno_t IZ_strlwr(char* dest, const char* str, rsize_t str_size) {
#if defined IZ_WINDOWS
	errno_t copy_result = memcpy_s(dest, str_size, str, str_size);
	if (copy_result) {
		return copy_result;
	}
	return _strlwr_s(dest, str_size);
#else
	memcpy(dest, str, str_size - 1);
	for (unsigned int i = 0; i < str_size; i += 1) {
		if ('A' <= dest[i] && dest[i] <= 'Z') {
			dest[i] += 0x20;
		}
	}
	return 0;
#endif
}

void* IZ_memset(void* dest, int value, rsize_t dest_size) {
	return memset(dest, value, dest_size);
}

const char* IZ_contains(const char* haystack, rsize_t haystack_size, const char* needle, rsize_t needle_size) {
	int i;
	for (i = 0; i < haystack_size; i += 1) {
		int j;
		for (j = 0; j <= needle_size; j += 1) {
			if ((haystack[j] == ' ' || haystack[j] == '\0' || j == needle_size) && (haystack[i + j] == ' ' || haystack[i + j] == '\0')) {
				return haystack + i;
			}
			if (haystack[i + j] != needle[j]) {
				break;
			}
		}
	}
	return NULL;
}

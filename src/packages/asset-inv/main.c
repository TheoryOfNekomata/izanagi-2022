#include <stdio.h>
#include "minIni.h"
#include "../common/IZ_common.h"
#include "../stdinc/IZ_string.h"

#if defined IZ_WINDOWS
#include <windows.h>
#endif

i32 IZ_ReadAsset(const char* current_dir, const char* ini_filename) {
	WIN32_FIND_DATA fd_file;
	char current_path[2048];
	sprintf(current_path, "%s\\*.*", current_dir);
	HANDLE h_find = FindFirstFile(current_path, &fd_file);

	if (h_find == INVALID_HANDLE_VALUE) {
		return -1;
	}

	do {
		if (strcmp(fd_file.cFileName, ".") == 0) {
			continue;
		}

		if (strcmp(fd_file.cFileName, "..") == 0) {
			continue;
		}

		sprintf(current_path, "%s\\%s", current_dir, fd_file.cFileName);

		if (fd_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			fprintf(stderr, "Invalid asset component: %s\n", current_path);
			continue;
		}

		char final_current_dir[2048];
		IZ_memcpy(final_current_dir, 2048, current_dir, 2048);

		u16 i = 0;
		char c = current_dir[i];
		while (c != '\0') {
			final_current_dir[i] = c == '\\' ? '/' : c;
			i += 1;
			c = current_dir[i];
		}

		ini_putl(final_current_dir, fd_file.cFileName, fd_file.nFileSizeLow, ini_filename);
	} while(FindNextFile(h_find, &fd_file));

	FindClose(h_find);

	return 0;
}

i32 IZ_ReadAssetPack(const char* current_dir, const char* ini_filename) {
	WIN32_FIND_DATA fd_file;
	char current_path[2048];
	sprintf(current_path, "%s\\*.*", current_dir);
	HANDLE h_find = FindFirstFile(current_path, &fd_file);

	if (h_find == INVALID_HANDLE_VALUE) {
		return -1;
	}

	do {
		if (strcmp(fd_file.cFileName, ".") == 0) {
			continue;
		}

		if (strcmp(fd_file.cFileName, "..") == 0) {
			continue;
		}

		sprintf(current_path, "%s\\%s", current_dir, fd_file.cFileName);

		if (fd_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			IZ_ReadAsset(current_path, ini_filename);
			continue;
		}

		fprintf(stderr, "Invalid asset: %s\n", current_path);
	} while(FindNextFile(h_find, &fd_file));

	FindClose(h_find);

	return 0;
}

i32 IZ_ReadAllAssetPacks(const char* current_dir, const char* ini_filename) {
	ini_remove(ini_filename);

	WIN32_FIND_DATA fd_file;
	char current_path[2048];
	sprintf(current_path, "%s\\*.*", current_dir);
	HANDLE h_find = FindFirstFile(current_path, &fd_file);

	if (h_find == INVALID_HANDLE_VALUE) {
		return -1;
	}

	do {
		if (strcmp(fd_file.cFileName, ".") == 0) {
			continue;
		}

		if (strcmp(fd_file.cFileName, "..") == 0) {
			continue;
		}

		sprintf(current_path, "%s\\%s", current_dir, fd_file.cFileName);

		if (fd_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			IZ_ReadAssetPack(current_path, ini_filename);
			continue;
		}

		fprintf(stderr, "Invalid asset pack: %s\n", current_path);
	} while(FindNextFile(h_find, &fd_file));

	FindClose(h_find);

	return 0;
}

i32 main(void) {
	return IZ_ReadAllAssetPacks("assets\\internal", "assets.ini");
}

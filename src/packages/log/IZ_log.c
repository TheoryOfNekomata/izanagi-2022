#include "IZ_log.h"

bool IZ_LogIsSupportedTerminal() {
	bool result;
	const char *term = getenv("TERM");
	result = term && strcmp(term, "") != 0;
#ifndef IZ_WINDOWS
	return result;
#else
	if (result) {
		return 1;
	}

	// Attempt to enable virtual terminal processing on Windows.
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/mt638032(v=vs.85).aspx
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) {
		return 0;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) {
		return 0;
	}

	dwMode |= 0x4; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
	if (!SetConsoleMode(hOut, dwMode)) {
		return 0;
	}

	return 1;
#endif
}

bool IZ_LogIsSupportedColor(FILE* s) {
	return isatty(fileno(s)) && IZ_LogIsSupportedTerminal();
}

void IZ_LogInitialize(const char* context, bool force_output) {
	if (force_output) {
		stdout_dest = stdout;
		stderr_dest = stderr;
		return;
	}

#ifdef IZ_DEBUG
	stdout_dest = stdout;
	stderr_dest = stderr;
#else
	const char* log_dir = "logs";
	char base_log_dir[128];
	sprintf(base_log_dir, "%s/%s", SDL_GetBasePath(), log_dir);
	if (!IZ_isdir(base_log_dir)) {
		IZ_mkdir(base_log_dir);
	}
	char stdout_filename[128] = "";
	char stderr_filename[128] = "";
	char* now = IZ_TimerNowPathSafe();

	sprintf(stdout_filename, "%s/%s-out-%s.log", base_log_dir, context, now);
	sprintf(stderr_filename, "%s/%s-err-%s.log", base_log_dir, context, now);
	IZ_fopen(&stdout_dest, stdout_filename, "a");
	IZ_fopen(&stderr_dest, stderr_filename, "a");
#endif
}

void IZ_LogTeardown() {
#ifndef IZ_DEBUG
	if (IZ_LogIsSupportedColor(stdout_dest)) {
		return;
	}
	fclose(stdout_dest);
	fclose(stderr_dest);
#endif
}

void IZ_LogError(const char* context, const char* fmt, ...) {
#ifdef IZ_LOG_LEVEL_FLAG_ERROR
	char buffer[4096];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (IZ_LogIsSupportedColor(stdout_dest)) {
		fprintf(stdout_dest, RED "%-" IZ_LOG_WIDTH_TIMESTAMP "s" RESET " " BOLD "%-" IZ_LOG_WIDTH_CATEGORY "s" RESET " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
		return;
	}

	fprintf(stderr_dest, "%-" IZ_LOG_WIDTH_TIMESTAMP "s" " " "%-" IZ_LOG_WIDTH_CATEGORY "s" " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
#endif
}

void IZ_LogInfo(IZ_LogCategory category, const char* context, const char* fmt, ...) {
#ifdef IZ_LOG_LEVEL_FLAG_INFO
	char buffer[4096];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (IZ_LogIsSupportedColor(stdout_dest)) {
		switch (category) {
			default:
			case IZ_LOG_CATEGORY_GENERIC:
				fprintf(stdout_dest, CYN "%-" IZ_LOG_WIDTH_TIMESTAMP "s" RESET " " BOLD "%-" IZ_LOG_WIDTH_CATEGORY "s" RESET " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
				break;
			case IZ_LOG_CATEGORY_GLOBAL:
				fprintf(stdout_dest, MAG "%-" IZ_LOG_WIDTH_TIMESTAMP "s" RESET " " BOLD "%-" IZ_LOG_WIDTH_CATEGORY "s" RESET " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
				break;
			case IZ_LOG_CATEGORY_INPUT:
				fprintf(stdout_dest, WHT "%-" IZ_LOG_WIDTH_TIMESTAMP "s" RESET " " BOLD "%-" IZ_LOG_WIDTH_CATEGORY "s" RESET " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
				break;
		}

		return;
	}

	fprintf(stdout_dest, "%-" IZ_LOG_WIDTH_TIMESTAMP "s" " " "%-" IZ_LOG_WIDTH_CATEGORY "s" " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
#endif
}

void IZ_LogWarn(bool is_critical, const char* context, const char* fmt, ...) {
#ifdef IZ_LOG_LEVEL_FLAG_WARN
	char buffer[4096];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (IZ_LogIsSupportedColor(stdout_dest)) {
		if (is_critical) {
			fprintf(stdout_dest, GRN "%-" IZ_LOG_WIDTH_TIMESTAMP "s" RESET " " BOLD "%-" IZ_LOG_WIDTH_CATEGORY "s" RESET " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
		} else {
			fprintf(stdout_dest, YEL "%-" IZ_LOG_WIDTH_TIMESTAMP "s" RESET " " BOLD "%-" IZ_LOG_WIDTH_CATEGORY "s" RESET " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
		}
		return;
	}

	fprintf(stdout_dest, "%-" IZ_LOG_WIDTH_TIMESTAMP "s" " " "%-" IZ_LOG_WIDTH_CATEGORY "s" " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
#endif
}

void IZ_Log(const char* context, const char* fmt, ...) {
#ifdef IZ_LOG_LEVEL_FLAG_DEBUG
	char buffer[4096];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	if (IZ_LogIsSupportedColor(stdout_dest)) {
		fprintf(stdout_dest, BLU "%-" IZ_LOG_WIDTH_TIMESTAMP "s" RESET " " BOLD "%-" IZ_LOG_WIDTH_CATEGORY "s" RESET " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
		return;
	}

	fprintf(stdout_dest, "%-" IZ_LOG_WIDTH_TIMESTAMP "s" " " "%-" IZ_LOG_WIDTH_CATEGORY "s" " %s\n", IZ_LOG_DATE_FUNCTION(), context, buffer);
	fflush(stdout_dest);
#endif
}

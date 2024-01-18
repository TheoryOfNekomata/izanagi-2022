#include "IZ_intercept.h"

typedef enum {
	IZ_LOG_INTERCEPT_EXTRACT_STATE_INITIAL,
	IZ_LOG_INTERCEPT_EXTRACT_STATE_OPEN_BRACKET,
	IZ_LOG_INTERCEPT_EXTRACT_STATE_CLOSE_BRACKET,
	IZ_LOG_INTERCEPT_EXTRACT_STATE_SPACE,
	IZ_LOG_INTERCEPT_EXTRACT_STATE_MESSAGE,
} IZ_LogInterceptExtractState;

void IZ_LogInterceptExtractWSMessageToFormatted(const char* raw_line, char* formatted_line) {
	IZ_LogInterceptExtractState state = IZ_LOG_INTERCEPT_EXTRACT_STATE_INITIAL;
	unsigned int offset = 0;

	IZ_memset(formatted_line, 0, 1024);
	for (unsigned int i = 0; i < 1024; i += 1) {
		if (state == IZ_LOG_INTERCEPT_EXTRACT_STATE_INITIAL && raw_line[i] == '[') {
			state = IZ_LOG_INTERCEPT_EXTRACT_STATE_OPEN_BRACKET;
			continue;
		}

		if (state == IZ_LOG_INTERCEPT_EXTRACT_STATE_OPEN_BRACKET && raw_line[i] == ']') {
			state = IZ_LOG_INTERCEPT_EXTRACT_STATE_CLOSE_BRACKET;
			continue;
		}

		if (state == IZ_LOG_INTERCEPT_EXTRACT_STATE_CLOSE_BRACKET && raw_line[i] == ' ') {
			offset = i + 1;
			state = IZ_LOG_INTERCEPT_EXTRACT_STATE_SPACE;
			continue;
		}

		if (state == IZ_LOG_INTERCEPT_EXTRACT_STATE_SPACE) {
			if (isspace(raw_line[i])) {
				continue;
			}
			state = IZ_LOG_INTERCEPT_EXTRACT_STATE_MESSAGE;
		}

		if (raw_line[i] == '\r' || raw_line[i] == '\n' || raw_line[i] == '\0') {
			formatted_line[i - offset] = '\0';
			break;
		}

		if (iscntrl(raw_line[i])) {
			continue;
		}

		formatted_line[i - offset] = raw_line[i];
	}
}

void IZ_LogInterceptHandleFromWS(i32 level, const char* line) {
	static char buffer[1024];
	IZ_LogInterceptExtractWSMessageToFormatted(line, buffer);
	switch (level) {
		// TODO level is a bit field...check if each level is OR'd.
		case LLL_ERR:
			IZ_LogError("net/ws", "%s", buffer);
			return;
		case LLL_WARN:
			IZ_LogWarn(true, "net/ws", "%s", buffer);
			return;
		case LLL_NOTICE:
			IZ_LogWarn(false, "net/ws", "%s", buffer);
			return;
		case LLL_USER:
		default:
			break;
	}

	IZ_LogInfo(IZ_LOG_CATEGORY_GENERIC, "net/ws", "%s", buffer);
}

void IZ_LogInterceptWSMessages(i32 level) {
	lws_set_log_level(level, IZ_LogInterceptHandleFromWS);
}

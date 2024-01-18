#ifndef IZ_LOG_MOCK_H
#define IZ_LOG_MOCK_H

#include <bdd-for-c-mocks.h>
#include "../../../src/packages/log/IZ_log.h"

mock_modes(IZ_LogInfo) {
	IZ_LOG_INFO_SUPPRESS = 0,
	IZ_LOG_INFO_LOG,
};

mock(IZ_LOG_DATE_FUNCTION) char* IZ_LOG_DATE_FUNCTION(void) {
	mock_return(IZ_LOG_DATE_FUNCTION) "";
}

mock(IZ_LogInfo) void IZ_LogInfo(IZ_LogCategory category, const char* context, const char* fmt, ...) {
	mock_mode_if(IZ_LogInfo, IZ_LOG_INFO_SUPPRESS) {
		mock_return(IZ_LogInfo);
	} else mock_mode_if(IZ_LogInfo, IZ_LOG_INFO_LOG) {
		char buffer[4096];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, sizeof(buffer), fmt, args);
		va_end(args);
		fprintf(stdout, CYN "%24s" RESET " %s\n", IZ_LOG_DATE_FUNCTION(), buffer);
		mock_return(IZ_LogInfo);
	}
}

#endif

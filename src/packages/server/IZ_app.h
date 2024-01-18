#ifndef IZ_APP_H
#define IZ_APP_H

#include <signal.h>
#include <stdbool.h>
#include "../common/IZ_common.h"
#include "../net/IZ_net_server.h"
#include "../log/IZ_intercept.h"
#include "../stdinc/IZ_string.h"
#include "db/IZ_repo.h"
#include "IZ_app_config.h"

typedef struct {
	IZ_NetServerState net_state;
	IZ_RepoState repo_state;
} IZ_App;

IZ_ProcedureResult IZ_AppRun(IZ_App*, u8, const char*[]);

#endif

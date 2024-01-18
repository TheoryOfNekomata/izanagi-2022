#ifndef IZ_NET_SERVER_H
#define IZ_NET_SERVER_H

#include <ini-config.h>
#include <ini-config/source/types/int.h>
#include <ini-config/source/types/string.h>
#include "../common/IZ_common.h"
#include "../stdinc/IZ_string.h"
#include "core/IZ_websocket.h"
#include "svc/IZ_wsserver.h"

#define IZ_DEFAULT_MOTD ""

typedef struct {
	u16 port;
	char name[64];
	char motd[128];
} IZ_NetServerConfig;

typedef struct {
	IZ_NetServerConfig config;
	IZ_NetBinding binding;
} IZ_NetServerState;

static IZ_NetServerState IZ_NET_SERVER_DEFAULT_STATE = {
	.config = {
		.port = 42069,
		.name = IZ_APP_NAME " Server",
		.motd = IZ_DEFAULT_MOTD,
	},
	.binding = {
		.interrupted = false,
		.context = NULL,
		.connection = NULL,
		.user_data = NULL,
	},
};

IZ_ProcedureResult IZ_NetServerInitialize(IZ_NetServerState*, void*, const char*, u8, const char*[]);

IZ_ProcedureResult IZ_NetSaveConfig(IZ_NetServerState*, const char*);

void IZ_NetServerCancelService(IZ_NetServerState*);

#endif

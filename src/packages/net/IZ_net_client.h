#ifndef IZ_NET_CLIENT_H
#define IZ_NET_CLIENT_H

#include <SDL2/SDL_thread.h>
#include "ini-config.h"
#include "types/int.h"
#include "types/string.h"

#include "../common/IZ_common.h"
#include "../game/input/IZ_action.h"
#include "../log/IZ_log.h"
#include "../stdinc/IZ_string.h"
#include "svc/IZ_wsclient.h"

typedef enum {
	IZ_NET_CLIENT_STATUS_PRISTINE,
	IZ_NET_CLIENT_STATUS_CONNECTING,
	IZ_NET_CLIENT_STATUS_ERROR,
	IZ_NET_CLIENT_STATUS_CONNECTED,
} IZ_NetClientStatus;

typedef struct {
	u16 packet_interval_ms;
	u8 max_reconnect_retries;
	u8 reconnect_interval_secs;
	char usernames[IZ_PLAYERS][32];
} IZ_NetClientConfig;

typedef struct {
	void* thread;
	IZ_NetClientConfig config;
	IZ_NetBinding binding;
	IZ_NetInitializeParams params;
	void* callback;
	IZ_Action action[IZ_PLAYERS];
	u8 retries;
	IZ_NetClientStatus status;
	// TODO add message queue
} IZ_NetClientState;

static IZ_NetClientState IZ_NET_CLIENT_DEFAULT_STATE = {
	.thread = NULL,
	.config = {
		.packet_interval_ms = 200,
		.max_reconnect_retries = 3,
		.reconnect_interval_secs = 3,
		.usernames = {
			"Player 1",
#if IZ_PLAYERS > 1
			"Player 2"
#endif
		},
	},
	.binding = {
		.interrupted = false,
		.context = NULL,
		.connection = NULL,
		.user_data = NULL,
	},
	.params = {
		.port = 42069,
		.path = "/",
		.host = "localhost",
	},
	.callback = NULL,
	.action = {},
	.retries = 3,
	.status = IZ_NET_CLIENT_STATUS_PRISTINE,
};

IZ_ProcedureResult IZ_NetClientInitialize(IZ_NetClientState*, void*, void*, const char *, u8, const char *[]);

void IZ_NetClientConnect(IZ_NetClientState*, IZ_WSClientInitializeParams);

void IZ_NetClientDisconnect(IZ_NetClientState*);

IZ_ProcedureResult IZ_NetClientSaveConfig(IZ_NetClientState*, const char*);

void IZ_NetClientSendBinaryMessage(IZ_NetClientState*, void*, size_t);

void IZ_NetClientSendTextMessage(IZ_NetClientState*, char*, size_t);

void IZ_NetClientTeardown(IZ_NetClientState*);

#endif

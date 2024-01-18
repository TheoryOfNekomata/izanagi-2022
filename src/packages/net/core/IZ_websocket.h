#ifndef IZ_WEBSOCKET_H
#define IZ_WEBSOCKET_H

#include "libwebsockets.h"
#include "../../common/IZ_common.h"
#include "../../stdinc/IZ_string.h"

#define NETWORK_PROTOCOL "izanagi-networking"
#define RING_COUNT 32

/* one of these created for each message */
typedef struct {
	size_t len;
	u8 binary: 1;
	u8 first: 1;
	u8 final: 1;
	void* payload; /* is malloc'd */
} IZ_WebsocketMessage;

typedef struct {
	struct lws_context* context;
	struct lws* connection;
	void* user_data;
	u8 interrupted: 1;
} IZ_Websocket;

typedef IZ_Websocket IZ_NetBinding;

void IZ_WebsocketInitialize(IZ_Websocket*);

IZ_ProcedureResult IZ_WebsocketHandle(IZ_Websocket*);

void IZ_WebsocketTeardown(IZ_Websocket*);

IZ_ProcedureResult IZ_WebsocketCreateBinaryMessage(struct lws*, IZ_WebsocketMessage*, void*, size_t);

IZ_ProcedureResult IZ_WebsocketCreateTextMessage(struct lws*, IZ_WebsocketMessage*, void*, size_t);

void IZ_WebsocketDestroyMessage(void*);

void IZ_WebsocketCancelService(IZ_Websocket*);

#endif

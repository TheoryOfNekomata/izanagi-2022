#ifndef IZ_WSCLIENT_H
#define IZ_WSCLIENT_H

#include "../../common/IZ_common.h"
#include "../../stdinc/IZ_string.h"
#include "../core/IZ_websocket.h"

typedef struct {
	struct lws_context *context;
	struct lws_vhost *vhost;
	const struct lws_protocols *protocol;

	lws_sorted_usec_list_t sul;

	struct lws_ring *ring; /* ringbuffer holding unsent messages */
	u32 tail;

	struct lws_client_connect_info i;
	struct lws *client_wsi;

	const char* address;
	const char* path;
	u16* port;
	const void* app;
} IZ_WSClientVHostData;

typedef struct {
	struct lws_ring *ring;
	u32 tail;
	unsigned char flow_controlled: 1;
} IZ_WSClientSessionData;

typedef struct {
	const char* host;
	const char* path;
	u16 port;
} IZ_WSClientInitializeParams;

typedef IZ_WSClientInitializeParams IZ_NetInitializeParams;

IZ_ProcedureResult IZ_WSClientInitialize(IZ_Websocket*, IZ_WSClientInitializeParams);

IZ_ProcedureResult IZ_WSClientHandle(IZ_Websocket*);

void IZ_WSClientTeardown(IZ_Websocket*);

void IZ_WSClientCancelService(IZ_Websocket*);

IZ_ProcedureResult IZ_WSClientProtocolInitialize(struct lws*, void*);

void IZ_WSClientProtocolTeardown(struct lws*);

IZ_ProcedureResult IZ_WSClientConnectionError(struct lws*, void*);

IZ_ProcedureResult IZ_WSClientOnOpen(struct lws*, IZ_WSClientSessionData*);

void IZ_WSClientOnClose(struct lws*);

IZ_ProcedureResult IZ_WSClientWritable(struct lws*);

void IZ_WSClientOnReceive(struct lws*, IZ_WSClientSessionData*, void*, size_t);

#endif

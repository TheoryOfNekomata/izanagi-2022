#ifndef IZ_WSSERVER_H
#define IZ_WSSERVER_H

#include <sys/stat.h>
#include <string.h>
#include "../../common/IZ_common.h"
#include "../../io/IZ_io.h"
#include "../../stdinc/IZ_string.h"
#include "../core/IZ_websocket.h"

#ifndef S_ISDIR
#define S_ISDIR(s) s & S_IFDIR
#endif

/* one of these is created for each client connecting to us */
typedef struct IZ_WSServerSessionData {
	struct IZ_WSServerSessionData* pss_list;
	struct lws* wsi;
	u32 tail;
	u8 culled: 1;
} IZ_WSServerSessionData;

/* one of these is created for each vhost our protocol is used with */
typedef struct {
	struct lws_context* context;
	struct lws_vhost* vhost;
	const struct lws_protocols* protocol;
	IZ_WSServerSessionData* pss_list; /* linked-list of live pss*/
	struct lws_ring* ring; /* ringbuffer holding unsent messages */
	u16* port;
	const void* app;
} IZ_WSServerVHostData;

typedef struct {
	u16 port;
} IZ_WSServerInitializeParams;

IZ_ProcedureResult IZ_WSServerInitialize(IZ_Websocket*, IZ_WSServerInitializeParams);

IZ_ProcedureResult IZ_WSServerHandle(IZ_Websocket*);

void IZ_WSServerTeardown(IZ_Websocket*);

void IZ_WSServerCancelService(IZ_Websocket*);

IZ_ProcedureResult IZ_WSServerProtocolInitialize(struct lws*, void*);

void IZ_WSServerProtocolTeardown(struct lws*);

IZ_ProcedureResult IZ_WSServerWritable(struct lws*, IZ_WSServerSessionData*);

IZ_ProcedureResult IZ_WSServerOnReceive(struct lws*, void*, size_t);

void IZ_WSServerOnOpen(struct lws*, IZ_WSServerSessionData*);

void IZ_WSServerOnClose(struct lws*, IZ_WSServerSessionData*);

#endif

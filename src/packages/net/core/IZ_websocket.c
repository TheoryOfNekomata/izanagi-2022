#include "IZ_websocket.h"

void IZ_WebsocketInitialize(IZ_Websocket* ws) {
	ws->context = NULL;
	ws->connection = NULL;
	ws->interrupted = false;
}

IZ_ProcedureResult IZ_WebsocketHandle(IZ_Websocket* ws) {
	return lws_service_tsi(ws->context, -1, 0);
}

void IZ_WebsocketCancelService(IZ_Websocket* ws) {
	ws->interrupted = true;
	ws->connection = NULL;
	lws_cancel_service(ws->context);
}

void IZ_WebsocketTeardown(IZ_Websocket* ws) {
	lws_context_destroy(ws->context);
	ws->context = NULL;
}

void IZ_WebsocketDestroyMessage(void* msg_raw) {
	IZ_WebsocketMessage* msg = msg_raw;
	free(msg->payload);
	msg->payload = NULL;
	msg->len = 0;
}

IZ_ProcedureResult IZ_WebsocketCreateBinaryMessage(struct lws* wsi, IZ_WebsocketMessage* amsg, void* in, size_t len) {
	/* notice we over-allocate by LWS_PRE */
	amsg->payload = malloc(LWS_PRE + len);
	if (!amsg->payload) {
		return -1;
	}

	amsg->first = (u8) lws_is_first_fragment(wsi);
	amsg->final = (u8) lws_is_final_fragment(wsi);
	amsg->binary = true;
	amsg->len = len;
	IZ_memcpy((char*) amsg->payload + LWS_PRE, len, in, len);
	return 0;
}

IZ_ProcedureResult IZ_WebsocketCreateTextMessage(struct lws* wsi, IZ_WebsocketMessage* amsg, void* in, size_t len) {
	/* notice we over-allocate by LWS_PRE */
	amsg->payload = malloc(LWS_PRE + len);
	if (!amsg->payload) {
		return -1;
	}

	amsg->first = (u8) lws_is_first_fragment(wsi);
	amsg->final = (u8) lws_is_final_fragment(wsi);
	amsg->binary = false;
	amsg->len = len;
	IZ_memcpy((char*) amsg->payload + LWS_PRE, len, in, len);
	return 0;
}

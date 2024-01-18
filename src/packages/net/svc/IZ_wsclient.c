#include "IZ_wsclient.h"

IZ_ProcedureResult IZ_WSClientCallback(
	struct lws* wsi,
	enum lws_callback_reasons reason,
	void* user,
	void* in,
	size_t len
) {
	switch (reason) {
		case LWS_CALLBACK_PROTOCOL_INIT:
			return IZ_WSClientProtocolInitialize(wsi, in);
		case LWS_CALLBACK_PROTOCOL_DESTROY:
			IZ_WSClientProtocolTeardown(wsi);
			break;
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			return IZ_WSClientConnectionError(wsi, in);
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			return IZ_WSClientOnOpen(wsi, user);
		case LWS_CALLBACK_CLIENT_CLOSED:
			IZ_WSClientOnClose(wsi);
			break;
		case LWS_CALLBACK_CLIENT_RECEIVE:
			IZ_WSClientOnReceive(wsi, user, in, len);
			break;
		case LWS_CALLBACK_CLIENT_WRITEABLE:
			return IZ_WSClientWritable(wsi);
		default:
			break;
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);
}

IZ_ProcedureResult IZ_WSClientInitialize(IZ_Websocket* state, IZ_WSClientInitializeParams params) {
	struct lws_context_creation_info info;
	IZ_memset(&info, 0, sizeof info);
	info.port = CONTEXT_PORT_NO_LISTEN;

	static const struct lws_protocols protocols[] = {
		{
			.name = NETWORK_PROTOCOL,
			.callback = IZ_WSClientCallback,
			.per_session_data_size = sizeof(IZ_WSClientSessionData),
			.rx_buffer_size = 1024,
			.id = 0,
			.user = NULL,
			.tx_packet_size = 0,
		},
		LWS_PROTOCOL_LIST_TERM
	};
	info.protocols = protocols;

	static struct lws_protocol_vhost_options pvo_address = {
		NULL,
		NULL,
		"address",		/* pvo name */
		"localhost"	/* pvo value */
	};
	pvo_address.value = params.host;

	static struct lws_protocol_vhost_options pvo_path = {
		&pvo_address,
		NULL,
		"path",		/* pvo name */
		"/"	/* pvo value */
	};
	pvo_path.value = params.path;

	static struct lws_protocol_vhost_options pvo_port = {
		&pvo_path,
		NULL,
		"port",		/* pvo name */
		NULL	/* pvo value */
	};
	pvo_port.value = (void*) &params.port;

	static struct lws_protocol_vhost_options pvo_app = {
		&pvo_port,
		NULL,
		"app",
		NULL,
	};
	pvo_app.value = state->user_data;

	static const struct lws_protocol_vhost_options pvo = {
		NULL,		/* "next" pvo linked-list */
		&pvo_app,	/* "child" pvo linked-list */
		NETWORK_PROTOCOL,	/* protocol name we belong to on this vhost */
		""		/* ignored */
	};

	info.pvo = &pvo;
	info.fd_limit_per_thread = 1 + 1 + 1;
	IZ_WebsocketInitialize(state);
	state->context = lws_create_context(&info);
	if (!state->context) {
		return -1;
	}
	return 0;
}

IZ_ProcedureResult IZ_WSClientHandle(IZ_Websocket* state) {
	return IZ_WebsocketHandle(state);
}

void IZ_WSClientTeardown(IZ_Websocket* state) {
	IZ_WebsocketTeardown(state);
}

void IZ_WSClientCancelService(IZ_Websocket* state) {
	IZ_WebsocketCancelService(state);
}

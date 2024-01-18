#include "IZ_wsserver.h"

IZ_ProcedureResult IZ_WSServerCallback(
	struct lws* wsi,
	enum lws_callback_reasons reason,
	void* user,
	void* in,
	size_t len
) {
	switch (reason) {
		case LWS_CALLBACK_PROTOCOL_INIT:
			return IZ_WSServerProtocolInitialize(wsi, in);
		case LWS_CALLBACK_PROTOCOL_DESTROY:
			IZ_WSServerProtocolTeardown(wsi);
			break;
		case LWS_CALLBACK_ESTABLISHED:
			IZ_WSServerOnOpen(wsi, user);
			break;
		case LWS_CALLBACK_CLOSED:
			IZ_WSServerOnClose(wsi, user);
			break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			return IZ_WSServerWritable(wsi, user);
		case LWS_CALLBACK_RECEIVE:
			return IZ_WSServerOnReceive(wsi, in, len);
		default:
			break;
	}

	return 0;
}

const char* IZ_WSServerTestPath(const char* base_dir, const char* file) {
	static char test_path[32];
	IZ_sprintf(test_path, 32, "%s/%s", base_dir, file);
	struct stat stats;
	stat(test_path, &stats);
	if (stats.st_mode & S_IREAD) {
		return file;
	}
	return NULL;
}

IZ_ProcedureResult IZ_WSServerInitialize(IZ_Websocket* state, IZ_WSServerInitializeParams params) {
	struct lws_context_creation_info info;
	IZ_memset(&info, 0, sizeof info);
	info.port = params.port;

	const char* origin = "./public";
	struct stat stats;
	stat(origin, &stats);
	if (S_ISDIR(stats.st_mode)) {
		static struct lws_http_mount mount = {
			.mount_next = NULL,    /* linked-list "next" */
			.mountpoint = "/",    /* mountpoint URL */
			.origin = NULL, /* serve from dir */
			.def = "index.htm",  /* default filename */
			.protocol = "http",
			.cgienv = NULL,
			.extra_mimetypes = NULL,
			.interpret = NULL,
			.cgi_timeout = 0,
			.cache_max_age = 0,
			.auth_mask = 0,
			.cache_reusable = 0,
			.cache_revalidate = 0,
			.cache_intermediaries = 0,
			.origin_protocol = LWSMPRO_FILE,  /* files in a dir */
			.mountpoint_len = 1,    /* char count */
			.basic_auth_login_file = NULL,
		};
		mount.origin = origin;
		const char* (alt_test_paths[]) = {
			"index.html",
		};
		const char* default_filename;
		u8 i;
		for (i = 0; i < 1; i += 1) {
			default_filename = IZ_WSServerTestPath(origin, alt_test_paths[i]);
		}

		if (default_filename) {
			mount.def = default_filename;
		}

		info.mounts = &mount;
	}

	static const struct lws_protocols protocols[] = {
		{
			.name = NETWORK_PROTOCOL,
			.callback = IZ_WSServerCallback,
			.per_session_data_size = sizeof(IZ_WSServerSessionData),
			.rx_buffer_size = 0,
			.id = 0,
			.user = NULL,
			.tx_packet_size = 0,
		},
		{
			.name = "http",
			.callback = lws_callback_http_dummy,
			.per_session_data_size = 0,
			.rx_buffer_size = 0,
			.id = 0,
			.user = NULL,
			.tx_packet_size = 0,
		},
		LWS_PROTOCOL_LIST_TERM,
	};
	info.protocols = protocols;

	static struct lws_protocol_vhost_options pvo_port = {
		NULL,
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
	info.options = (
		LWS_SERVER_OPTION_VALIDATE_UTF8
		| LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE
	);

	IZ_WebsocketInitialize(state);
	state->context = lws_create_context(&info);
	if (!state->context) {
		return -1;
	}

	return 0;
}

IZ_ProcedureResult IZ_WSServerHandle(IZ_Websocket* state) {
	return IZ_WebsocketHandle(state);
}

void IZ_WSServerTeardown(IZ_Websocket* state) {
	IZ_WebsocketTeardown(state);
}

void IZ_WSServerCancelService(IZ_Websocket* state) {
	IZ_WebsocketCancelService(state);
}

#include "IZ_app.h"

static IZ_App* global_app;

void IZ_AppHandleSignal(i32 _signal) {
	global_app->net_state.binding.interrupted = true;
	IZ_NetServerCancelService(&global_app->net_state);
}

IZ_ProcedureResult IZ_AppInitialize(IZ_App *app, u8 argc, const char **argv) {
	global_app = app;
	IZ_memset(app, 0, sizeof(IZ_App));
	signal(SIGINT, IZ_AppHandleSignal);

	IZ_LogInitialize("server", true);

	//	IZ_LogInterceptWSMessages(app->config.log_level);
	IZ_LogInterceptWSMessages(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE);

	const char* cmdline_buffer;
	char config_path[128];
	// TODO abstract command line args parsing
	if ((cmdline_buffer = INI_ConfigGetCommandlineOption(argc, argv, "-c"))) {
		IZ_memcpy(config_path, 128, cmdline_buffer, 128);
	} else {
		IZ_ConfigGetDefaultPath(config_path, 128);
	}

	if (IZ_NetServerInitialize(&app->net_state, app, config_path, argc, argv)) {
		return -1;
	}

	if (IZ_RepoInitialize(&app->repo_state, config_path, argc, argv)) {
		return -1;
	}

	return 0;
}

void IZ_AppTeardown(IZ_App* app) {
	IZ_RepoTeardown(&app->repo_state);
	IZ_WSServerTeardown(&app->net_state.binding);
	IZ_LogTeardown();
}

void IZ_AppPrintHelpOptions() {
	printf("\n");
	printf("Options:\n");
	printf("\n");
	printf(
		"\n"
		"Options:\n"
		"\n"
		"  -c <path>     Specifies the path to the config file.       (default: \"./" IZ_CONFIG_SERVER_PATH "\")\n"
		"  -d <path>     Specifies the path to the database.                  (default: \"./db.sqlite\")\n"
		"  -h            Displays this help screen.\n"
		"  -m <value>    Specifies the message of the day.                               (default: \"\")\n"
		"  -n <value>    Specifies the name of the server.                        (default: \"%s\")\n"
		"  -p <value>    Specifies the port where the server runs.                    (default: 42069)\n",
		IZ_APP_NAME
	);
}

void IZ_AppPrintHelpUsage() {
	printf("\n");
	printf("Usage:");
	printf("  %s [options]\n", "server.exe");
}

void IZ_AppPrintHelpHeader() {
	printf("\n");
	printf("%s - %s\n", IZ_APP_NAME, IZ_APP_SERVER_DESCRIPTION);
}

void IZ_AppPrintHelp() {
	IZ_AppPrintHelpHeader();
	IZ_AppPrintHelpUsage();
	IZ_AppPrintHelpOptions();
}

IZ_ProcedureResult IZ_AppRun(IZ_App *app, u8 argc, const char **argv) {
	// TODO have a config subsystem that handles these.
	if (INI_ConfigGetCommandlineOption(argc, argv, "-h")) {
		IZ_AppPrintHelp();
		return 0;
	}

	if (IZ_AppInitialize(app, argc, argv)) {
		return -1;
	}

	if (IZ_WSServerInitialize(&app->net_state.binding, (IZ_WSServerInitializeParams) {
		.port = app->net_state.config.port,
	})) {
		return -1;
	}

	i32 result = 0;
	while (true) {
		if (IZ_WSServerHandle(&app->net_state.binding)) {
			result = -1;
			break;
		}

		if (app->net_state.binding.interrupted) {
			break;
		}
	}

	IZ_AppTeardown(app);
	return result;
}

void IZ_WSServerCullLaggingClients(IZ_WSServerVHostData *vhd) {
	u32 oldest_tail = lws_ring_get_oldest_tail(vhd->ring);
	IZ_WSServerSessionData *old_pss = NULL;
	i32 most = 0;
	i32 before = (i32) lws_ring_get_count_waiting_elements(vhd->ring, &oldest_tail);
	i32 m;

	/*
	 * At least one guy with the oldest tail has lagged too far, filling
	 * the ringbuffer with stuff waiting for them, while new stuff is
	 * coming in, and they must close, freeing up ringbuffer entries.
	 */

	lws_start_foreach_llp_safe(
		IZ_WSServerSessionData**,
		ppss,
		vhd->pss_list,
		pss_list
	) {
				if ((*ppss)->tail == oldest_tail) {
					old_pss = *ppss;

					lwsl_user("Killing lagging client %p\n", (*ppss)->wsi);

					lws_set_timeout((*ppss)->wsi, PENDING_TIMEOUT_LAGGING,
						/*
						 * we may kill the wsi we came in on,
						 * so the actual close is deferred
						 */
													LWS_TO_KILL_ASYNC);

					/*
					 * We might try to write something before we get a
					 * chance to close.  But this pss is now detached
					 * from the ring buffer.  Mark this pss as culled so we
					 * don't try to do anything more with it.
					 */

					(*ppss)->culled = true;

					/*
					 * Because we can't kill it synchronously, but we
					 * know it's closing momentarily and don't want its
					 * participation any more, remove its pss from the
					 * vhd pss list early.  (This is safe to repeat
					 * uselessly later in the close flow).
					 *
					 * Notice this changes *ppss!
					 */

					lws_ll_fwd_remove(IZ_WSServerSessionData, pss_list, (*ppss), vhd->pss_list);

					/* use the changed *ppss so we won't skip anything */

					continue;
				}

				/*
				 * so this guy is a survivor of the cull.  Let's track
				 * what is the largest number of pending ring elements
				 * for any survivor.
				 */
				m = (i32) lws_ring_get_count_waiting_elements(vhd->ring, &((*ppss)->tail));
				if (m > most) {
					most = m;
				}
			} lws_end_foreach_llp_safe(ppss);

	/* it would mean we lost track of oldest... but Coverity insists */
	if (!old_pss) {
		return;
	}

	/*
	 * Let's recover (ie, free up) all the ring slots between the
	 * original oldest's last one and the "worst" survivor.
	 */

	lws_ring_consume_and_update_oldest_tail(
		vhd->ring,
		IZ_WSServerSessionData,
		&old_pss->tail,
		(size_t) (before - most),
		vhd->pss_list,
		tail,
		pss_list
	);

	lwsl_user("%s: shrunk ring from %d to %d\n", __func__, before, most);
}

/* destroys the message when everyone has had a copy of it */

IZ_ProcedureResult IZ_WSServerProtocolInitialize(struct lws* wsi, void* in) {
	const struct lws_protocols* protocols = lws_get_protocol(wsi);
	struct lws_vhost* vhost = lws_get_vhost(wsi);
	IZ_WSServerVHostData* vhd_instance = (IZ_WSServerVHostData*) lws_protocol_vh_priv_get(vhost, protocols);
	IZ_WSServerVHostData** vhd = &vhd_instance;
	*vhd = lws_protocol_vh_priv_zalloc(vhost, protocols, sizeof(IZ_WSServerVHostData));
	(*vhd)->ring = lws_ring_create(
		sizeof(IZ_WebsocketMessage),
		RING_COUNT,
		IZ_WebsocketDestroyMessage
	);
	if (!(*vhd)->ring) {
		return -1;
	}
	(*vhd)->context = lws_get_context(wsi);
	(*vhd)->protocol = protocols;
	(*vhd)->vhost = vhost;
	(*vhd)->port = (u16*) lws_pvo_search(
		(const struct lws_protocol_vhost_options *)in,
		"port"
	)->value;
	(*vhd)->app = lws_pvo_search(
		(const struct lws_protocol_vhost_options *)in,
		"app"
	)->value;
	return 0;
}

void IZ_WSServerProtocolTeardown(struct lws* wsi) {
	IZ_WSServerVHostData* vhd = (IZ_WSServerVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);
	lws_ring_destroy(vhd->ring);
}

void IZ_WSServerOnOpen(struct lws* wsi, IZ_WSServerSessionData* pss) {
	IZ_WSServerVHostData* vhd = (IZ_WSServerVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);

	/* add ourselves to the list of live pss held in the vhd */
	lwsl_user("LWS_CALLBACK_ESTABLISHED: wsi %p\n", wsi);
	lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
	pss->tail = lws_ring_get_oldest_tail(vhd->ring);
	pss->wsi = wsi;
}

void IZ_WSServerOnClose(struct lws* wsi, IZ_WSServerSessionData* pss) {
	IZ_WSServerVHostData* vhd = (IZ_WSServerVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);

	lwsl_user("LWS_CALLBACK_CLOSED: wsi %p\n", wsi);
	/* remove our closing pss from the list of live pss */
	lws_ll_fwd_remove(IZ_WSServerSessionData, pss_list, pss, vhd->pss_list);
}

IZ_ProcedureResult IZ_WSServerWritable(struct lws* wsi, IZ_WSServerSessionData* pss) {
	IZ_WSServerVHostData* vhd = (IZ_WSServerVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);

	if (pss->culled) {
		return 0;
	}
	const IZ_WebsocketMessage* pmsg = lws_ring_get_element(vhd->ring, &pss->tail);
	if (!pmsg) {
		return 0;
	}

	/* notice we allowed for LWS_PRE in the payload already */
	i32 m = lws_write(
		wsi,
		((unsigned char*) pmsg->payload) + LWS_PRE,
		pmsg->len,
		pmsg->binary ? LWS_WRITE_BINARY : LWS_WRITE_TEXT
	);

	if (m < (i32) pmsg->len) {
		lwsl_err("ERROR %d writing to ws socket\n", m);
		return -1;
	}

	lws_ring_consume_and_update_oldest_tail(
		vhd->ring,  /* lws_ring object */
		IZ_WSServerSessionData, /* type of objects with tails */
		&pss->tail,  /* tail of guy doing the consuming */
		1,    /* number of payload objects being consumed */
		vhd->pss_list,  /* head of list of objects with tails */
		tail,    /* member name of tail in objects with tails */
		pss_list  /* member name of next object in objects with tails */
	);

	/* more to do for us? */
	if (lws_ring_get_element(vhd->ring, &pss->tail)) {
		/* come back as soon as we can write more */
		lws_callback_on_writable(pss->wsi);
	}

	return 0;
}

IZ_ProcedureResult IZ_WSServerOnReceive(struct lws* wsi, void* in, size_t len) {
	IZ_WSServerVHostData* vhd = (IZ_WSServerVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);
	i32 n = (i32) lws_ring_get_count_free_elements(vhd->ring);
	if (!n) {
		/* forcibly make space */
		IZ_WSServerCullLaggingClients(vhd);
		n = (i32) lws_ring_get_count_free_elements(vhd->ring);
	}

	if (!n) {
		return 0;
	}

	lwsl_user("LWS_CALLBACK_RECEIVE: free space %d\n", n);

	IZ_WebsocketMessage amsg;
	const u8 result = (
		lws_frame_is_binary(wsi)
			? IZ_WebsocketCreateBinaryMessage(wsi, &amsg, in, len)
			: IZ_WebsocketCreateTextMessage(wsi, &amsg, in, len)
	);

	if (result) {
		lwsl_user("OOM: dropping\n");
		return 1;
	}

	if (!lws_ring_insert(vhd->ring, &amsg, 1)) {
		IZ_WebsocketDestroyMessage(&amsg);
		lwsl_user("dropping!\n");
		return 1;
	}

	/*
	 * let everybody know we want to write something on them
	 * as soon as they are ready
	 */
	lws_start_foreach_llp(IZ_WSServerSessionData**, ppss, vhd->pss_list) {
				lws_callback_on_writable((*ppss)->wsi);
			}

	lws_end_foreach_llp(ppss, pss_list);
	return 0;
}

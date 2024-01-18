#include "IZ_app_net.h"

void IZ_AppHandleNetworkingInboundBinaryEvents(struct IZ_App* app, void* binary_raw, size_t len) {
	if (len < 1) {
		return;
	}
	u8* binary = binary_raw;
	size_t i;
	char binary_string[len * 3 + 1];
	IZ_memset(binary_string, 0, len * 3 + 1);
	sprintf(binary_string, "%02x", binary[0]);
	for (i = 1; i < len; i += 1) {
		sprintf(binary_string, "%s %02x", binary_string, binary[i]);
	}
	IZ_LogInfo(IZ_LOG_CATEGORY_INPUT, "net/ws", "Received binary(length: %d): %s", len, binary_string);
}

void IZ_AppHandleNetworkingInboundTextEvents(struct IZ_App* app, const char* text, size_t len) {
	IZ_LogInfo(IZ_LOG_CATEGORY_INPUT, "net/ws", "Received string(length: %d): %s", len, text);
}

void IZ_AppHandleOutboundNetworking(struct IZ_App* app) {
	// TODO implement queueing of messages
	IZ_NetClientState* net_state = IZ_AppGetNetState(app);
	IZ_InputState* input_state = IZ_AppGetInputState(app);

	u8 player_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		if (net_state->action[player_index] != input_state->action[player_index]) {
			u8 player_actions_count = 1;
			IZ_AppActionSyncMessage* msg = IZ_malloc(
				sizeof(IZ_AppMessageHeader)
				+ sizeof(u8)
				+ (sizeof(IZ_AppPlayerActionSyncMessage) * player_actions_count)
			);
			msg->header.client_elapsed_time = IZ_AppGetTicks(app);
			msg->header.message_kind = IZ_MESSAGE_KIND_ACTION_SYNC;
			msg->action.player_actions_count = player_actions_count;
			msg->action.player[0].index = player_index;
			msg->action.player[0].value = input_state->action[player_index];
			msg->action.player[0].state = 0;
			IZ_NetClientSendBinaryMessage(
				net_state,
				msg,
				sizeof(*msg)
			);
			IZ_free(msg);
		}
		net_state->action[player_index] = input_state->action[player_index];
	}
}

void IZ_WSClientAttemptConnect(struct lws_sorted_usec_list *sul) {
	IZ_WSClientVHostData* vhd = lws_container_of(sul, IZ_WSClientVHostData, sul);

	vhd->i.context = vhd->context;
	vhd->i.port = *vhd->port;
	vhd->i.address = vhd->address;
	vhd->i.path = vhd->path;
	vhd->i.host = vhd->i.address;
	vhd->i.origin = vhd->i.address;
	vhd->i.ssl_connection = 0;

	vhd->i.protocol = NETWORK_PROTOCOL;
	vhd->i.pwsi = &vhd->client_wsi;

	struct IZ_App* app = (struct IZ_App*) vhd->app;
	IZ_NetClientState* net_state = IZ_AppGetNetState(app);
	net_state->status = IZ_NET_CLIENT_STATUS_CONNECTING;
	if (lws_client_connect_via_info(&vhd->i)) {
		return;
	}

	lws_sul_schedule(
		vhd->context,
		0,
		&vhd->sul,
		IZ_WSClientAttemptConnect,
		10 * LWS_US_PER_SEC
	);
}

IZ_ProcedureResult IZ_WSClientProtocolInitialize(struct lws* wsi, void* in) {
	const struct lws_protocols* protocols = lws_get_protocol(wsi);
	struct lws_vhost* vhost = lws_get_vhost(wsi);
	IZ_WSClientVHostData* vhd_instance = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(vhost,protocols);
	IZ_WSClientVHostData** vhd = &vhd_instance;
	*vhd = lws_protocol_vh_priv_zalloc(vhost, protocols, sizeof(IZ_WSClientVHostData));
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
	(*vhd)->address = lws_pvo_search(
		(const struct lws_protocol_vhost_options *)in,
		"address"
	)->value;
	(*vhd)->path = lws_pvo_search(
		(const struct lws_protocol_vhost_options *)in,
		"path"
	)->value;
	(*vhd)->app = lws_pvo_search(
		(const struct lws_protocol_vhost_options *)in,
		"app"
	)->value;
	IZ_WSClientAttemptConnect(&(*vhd)->sul);
	return 0;
}

void IZ_WSClientProtocolTeardown(struct lws* wsi) {
	IZ_WSClientVHostData* vhd = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);

	if (vhd->ring) {
		lws_ring_destroy(vhd->ring);
	}

	lws_sul_cancel(&vhd->sul);
	struct IZ_App* app = (struct IZ_App*) vhd->app;
	IZ_NetClientState* net_state = IZ_AppGetNetState(app);
	net_state->status = IZ_NET_CLIENT_STATUS_PRISTINE;
}

IZ_ProcedureResult IZ_WSClientConnectionError(struct lws* wsi, void* in) {
	lwsl_err("CLIENT_CONNECTION_ERROR: %s\n", in ? (char *)in : "(null)");
	IZ_WSClientVHostData* vhd = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);

	if (!vhd) {
		return -1;
	}

	struct IZ_App* app = (struct IZ_App*) vhd->app;
	IZ_AppBindConnection(app, NULL);
	vhd->client_wsi = NULL;
	IZ_NetClientState* net_state = IZ_AppGetNetState(app);
	if (net_state->retries == net_state->config.max_reconnect_retries) {
		lwsl_err("Max number of retries reached!\n");
		net_state->status = IZ_NET_CLIENT_STATUS_PRISTINE;
		return -1;
	}
	net_state->status = IZ_NET_CLIENT_STATUS_ERROR;
	net_state->retries += 1;
	lws_sul_schedule(
		vhd->context,
		0,
		&vhd->sul,
		IZ_WSClientAttemptConnect,
		net_state->config.reconnect_interval_secs * LWS_US_PER_SEC
	);

	return 0;
}

IZ_ProcedureResult IZ_WSClientOnOpen(struct lws* wsi, IZ_WSClientSessionData* pss) {
	IZ_WSClientVHostData* vhd = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);
	struct IZ_App* app = (struct IZ_App*) vhd->app;
	IZ_NetClientState* net_state = IZ_AppGetNetState(app);
	pss->ring = lws_ring_create(sizeof(IZ_WebsocketMessage), RING_COUNT,IZ_WebsocketDestroyMessage);
	if (!pss->ring) {
		net_state->status = IZ_NET_CLIENT_STATUS_ERROR;
		return -1;
	}

	IZ_AppBindConnection(app, wsi);
	net_state->status = IZ_NET_CLIENT_STATUS_CONNECTED;
	net_state->retries = 0;
	pss->tail = 0;
	return 0;
}

void IZ_WSClientOnClose(struct lws* wsi) {
	IZ_WSClientVHostData* vhd = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);
	struct IZ_App* app = (struct IZ_App*) vhd->app;
	IZ_AppBindConnection(app, NULL);
	vhd->client_wsi = NULL;
	lws_sul_schedule(
		vhd->context,
		0,
		&vhd->sul,
		IZ_WSClientAttemptConnect,
		LWS_US_PER_SEC
	);
}

IZ_ProcedureResult IZ_WSClientWritable(struct lws* wsi) {
	IZ_WSClientVHostData* vhd = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);

	const IZ_WebsocketMessage* pmsg = lws_ring_get_element(vhd->ring, &vhd->tail);
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

	if (m < (i32)pmsg->len) {
		lwsl_err("ERROR %d writing to ws socket\n", m);
		return -1;
	}

	lws_ring_consume_single_tail(vhd->ring, &vhd->tail, 1);

	/* more to do for us? */
	if (lws_ring_get_element(vhd->ring, &vhd->tail)) {
		/* come back as soon as we can write more */
		lws_callback_on_writable(wsi);
	}

	return 0;
}

void IZ_WSClientOnReceive(struct lws* wsi, IZ_WSClientSessionData* pss, void* in, size_t len) {
	i32 n = (i32) lws_ring_get_count_free_elements(pss->ring);
	if (!n) {
		lwsl_user("dropping!\n");
		return;
	}

	lwsl_user("LWS_CALLBACK_CLIENT_RECEIVE: %4d (rpp %5d, first %d, last %d, bin %d)\n",
						(int)len, (int)lws_remaining_packet_payload(wsi),
						lws_is_first_fragment(wsi),
						lws_is_final_fragment(wsi),
						lws_frame_is_binary(wsi));

	// lwsl_hexdump_notice(in, len);

	IZ_WebsocketMessage amsg;
	i32 result = (
		lws_frame_is_binary(wsi)
		? IZ_WebsocketCreateBinaryMessage(wsi, &amsg, in, len)
		: IZ_WebsocketCreateTextMessage(wsi, &amsg, in, len)
	);
	if (result < 0) {
		lwsl_user("OOM: dropping\n");
		return;
	}

	IZ_WSClientVHostData* vhd = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(wsi),
		lws_get_protocol(wsi)
	);

	struct IZ_App* app = (struct IZ_App*) vhd->app;
	if (amsg.binary) {
		IZ_AppHandleNetworkingInboundBinaryEvents(app, in, len);
	} else {
		IZ_AppHandleNetworkingInboundTextEvents(app, in, len);
	}

	if (!lws_ring_insert(pss->ring, &amsg, 1)) {
		IZ_WebsocketDestroyMessage(&amsg);
		lwsl_user("dropping!\n");
		return;
	}

	lws_ring_consume_single_tail(pss->ring, &pss->tail, 1);
	lws_callback_on_writable(wsi);

	if (!pss->flow_controlled && n < 3) {
		pss->flow_controlled = true;
		lws_rx_flow_control(wsi, 0);
	}
}

IZ_ProcedureResult IZ_AppRunNetworkingThread(struct IZ_App* app) {
	IZ_NetClientState* net_state = IZ_AppGetNetState(app);

	if (IZ_WSClientInitialize(&net_state->binding, net_state->params)) {
		return -1;
	}

	i32 result = 0;
	while (true) {
		if (IZ_WSClientHandle(&net_state->binding)) {
			result = -1;
			break;
		}

		if (net_state->binding.interrupted) {
			break;
		}
	}

	IZ_WSClientTeardown(&net_state->binding);
	return result;
}

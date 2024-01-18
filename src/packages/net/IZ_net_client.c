#include "IZ_net_client.h"

bool IZ_NetClientIsValidPacketIntervalMs(long packet_interval_ms) {
	return (100 <= packet_interval_ms && packet_interval_ms <= 500);
}

bool IZ_NetClientIsValidMaxReconnectRetries(long max_reconnect_retries) {
	return (0 <= max_reconnect_retries && max_reconnect_retries <= 8);
}

bool IZ_NetClientIsValidReconnectIntervalSeconds(long reconnect_interval_secs) {
	return (3 <= reconnect_interval_secs &&  reconnect_interval_secs <= 10);
}

static INI_ConfigItem net_client_config_items[IZ_PLAYERS + 3 + 1];

void IZ_NetClientInitializeConfigItems(INI_ConfigItem config_items[]) {
	config_items[0] = (INI_ConfigItem) {
		INI_CONFIG_TYPE_U16,
		"Network",
		"PacketIntervalMs",
		"-i",
		&IZ_NET_CLIENT_DEFAULT_STATE.config.packet_interval_ms,
		IZ_NetClientIsValidPacketIntervalMs,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	};
	config_items[1] = (INI_ConfigItem) {
		INI_CONFIG_TYPE_U8,
		"Network",
		"MaxReconnectRetries",
		NULL,
		&IZ_NET_CLIENT_DEFAULT_STATE.config.max_reconnect_retries,
		IZ_NetClientIsValidMaxReconnectRetries,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	};
	config_items[2] = (INI_ConfigItem) {
		INI_CONFIG_TYPE_U8,
		"Network",
		"ReconnectIntervalSeconds",
		NULL,
		&IZ_NET_CLIENT_DEFAULT_STATE.config.reconnect_interval_secs,
		IZ_NetClientIsValidReconnectIntervalSeconds,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	};

	u8 player_index;
	char* main_section_name;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		main_section_name = malloc(sizeof(char) * 64);
		sprintf(main_section_name, "Network.%d", player_index);
		config_items[3 + player_index] = (INI_ConfigItem) {
			INI_CONFIG_TYPE_STRING(32),
			main_section_name,
			"Username",
			NULL,
			&IZ_NET_CLIENT_DEFAULT_STATE.config.usernames[player_index],
			NULL,
			INI_CONFIG_TRANSFORMER_NONE,
			NULL,
		};
	}

	config_items[3 + IZ_PLAYERS] = INI_CONFIG_ITEM_NULL;
}

void IZ_NetClientBindStateToConfig(IZ_NetClientState* state, INI_ConfigItem config_items[]) {
	config_items[0].dest = &state->config.packet_interval_ms;
	config_items[1].dest = &state->config.max_reconnect_retries;
	config_items[2].dest = &state->config.reconnect_interval_secs;
	u8 player_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		config_items[3 + player_index].dest = &state->config.usernames[player_index];
	}
}

IZ_ProcedureResult IZ_NetClientSaveConfig(IZ_NetClientState* state, const char* config_path) {
	IZ_NetClientBindStateToConfig(state, net_client_config_items);
	return INI_ConfigSave(net_client_config_items, config_path);
}

IZ_ProcedureResult IZ_NetClientInitializeConfig(
	IZ_NetClientState* state,
	const char* config_path,
	u8 argc,
	const char* argv[]
) {
	IZ_NetClientInitializeConfigItems(net_client_config_items);
	IZ_NetClientBindStateToConfig(state, net_client_config_items);
	if (INI_ConfigInitialize(net_client_config_items, config_path, argc, argv) < 0) {
		return -1;
	}
	return 0;
}

IZ_ProcedureResult IZ_NetClientInitialize(
	IZ_NetClientState* state,
	void* user_data,
	void* callback,
	const char* config_path,
	u8 argc,
	const char* argv[]
) {
	if (!user_data) {
		return -1;
	}
	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "net", "Setting up...");
	IZ_memcpy(state, sizeof(IZ_NetClientState), &IZ_NET_CLIENT_DEFAULT_STATE, sizeof(IZ_NetClientState));
	if (IZ_NetClientInitializeConfig(state, config_path, argc, argv) < 0) {
		return -2;
	}
	state->binding.user_data = user_data;
	state->callback = callback;
	state->retries = state->config.max_reconnect_retries;
	u8 player_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		state->action[player_index] = 0;
	}

	return 0;
}

void IZ_NetClientTeardown(IZ_NetClientState* state) {
	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "net", "Shutting down...");
	IZ_NetClientDisconnect(state);
}

void IZ_NetClientConnect(IZ_NetClientState* state, IZ_WSClientInitializeParams params) {
	if (!state->callback) {
		return;
	}

	if (state->status == IZ_NET_CLIENT_STATUS_CONNECTED) {
		return;
	}

	if (state->status == IZ_NET_CLIENT_STATUS_CONNECTING) {
		return;
	}

	if (state->retries < state->config.max_reconnect_retries) {
		return;
	}

	state->retries = 0;
	state->params = params;
	state->thread = SDL_CreateThread(state->callback, "networking", state->binding.user_data);
	SDL_DetachThread(state->thread);
}

void IZ_NetClientDisconnect(IZ_NetClientState* state) {
	if (state->status == IZ_NET_CLIENT_STATUS_PRISTINE) {
		return;
	}
	if (state->binding.connection) {
		IZ_WSClientVHostData* vhd = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(
			lws_get_vhost(state->binding.connection),
			lws_get_protocol(state->binding.connection)
		);
		if (vhd) {
			lws_sul_cancel(&vhd->sul);
		}
	}
	state->retries = state->config.max_reconnect_retries;
	IZ_WSClientCancelService(&state->binding);
}

void IZ_NetClientSendBinaryMessage(IZ_NetClientState* state, void* in, size_t len) {
	if (state->status != IZ_NET_CLIENT_STATUS_CONNECTED) {
		return;
	}

	IZ_WSClientVHostData* vhd = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(state->binding.connection),
		lws_get_protocol(state->binding.connection)
	);

	IZ_WebsocketMessage amsg;
	i32 result;
	result = IZ_WebsocketCreateBinaryMessage(state->binding.connection, &amsg, in, len);

	if (result < 0) {
		return;
	}

	lws_ring_insert(vhd->ring, &amsg, 1);
	lws_callback_on_writable(state->binding.connection);
}

void IZ_NetClientSendTextMessage(IZ_NetClientState* state, char* in, size_t len) {
	if (state->status != IZ_NET_CLIENT_STATUS_CONNECTED) {
		return;
	}

	IZ_WSClientVHostData* vhd = (IZ_WSClientVHostData*) lws_protocol_vh_priv_get(
		lws_get_vhost(state->binding.connection),
		lws_get_protocol(state->binding.connection)
	);

	IZ_WebsocketMessage amsg;
	i32 result;
	result = IZ_WebsocketCreateTextMessage(state->binding.connection, &amsg, in, len);

	if (result < 0) {
		return;
	}

	lws_ring_insert(vhd->ring, &amsg, 1);
	lws_callback_on_writable(state->binding.connection);
}

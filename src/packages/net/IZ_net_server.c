#include "IZ_net_server.h"

bool IZ_NetServerIsValidPort(u16 port) {
	return (1000 <= port && port <= 59999);
}

static INI_ConfigItem net_server_config_items[] = {
	{
		INI_CONFIG_TYPE_STRING(64),
		"Network",
		"Name",
		"-n",
		&IZ_NET_SERVER_DEFAULT_STATE.config.name,
		NULL,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	},
	{
		INI_CONFIG_TYPE_STRING(128),
		"Network",
		"Motd",
		"-m",
		&IZ_NET_SERVER_DEFAULT_STATE.config.motd,
		NULL,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	},
	{
		INI_CONFIG_TYPE_U16,
		"Network",
		"Port",
		"-p",
		&IZ_NET_SERVER_DEFAULT_STATE.config.port,
		IZ_NetServerIsValidPort,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	},
	INI_CONFIG_ITEM_NULL,
};

void IZ_NetServerBindStateToConfig(IZ_NetServerState* state, INI_ConfigItem config_items[]) {
	config_items[0].dest = &state->config.name;
	config_items[1].dest = &state->config.motd;
	config_items[2].dest = &state->config.port;
}

IZ_ProcedureResult IZ_NetSaveConfig(IZ_NetServerState* state, const char* config_path) {
	IZ_NetServerBindStateToConfig(state, net_server_config_items);
	return INI_ConfigSave(net_server_config_items, config_path);
}

IZ_ProcedureResult IZ_NetServerInitializeConfig(
	IZ_NetServerState* state,
	const char* config_path,
	u8 argc,
	const char* argv[]
) {
	IZ_NetServerBindStateToConfig(state, net_server_config_items);
	if (INI_ConfigInitialize(net_server_config_items, config_path, argc, argv) < 0) {
		return -1;
	}
	return 0;
}

IZ_ProcedureResult IZ_NetServerInitialize(
	IZ_NetServerState* state,
	void* user_data,
	const char* config_path,
	u8 argc,
	const char* argv[]
) {
	if (!user_data) {
		return -1;
	}
	IZ_memcpy(state, sizeof(IZ_NetServerState), &IZ_NET_SERVER_DEFAULT_STATE, sizeof(IZ_NetServerState));
	if (IZ_NetServerInitializeConfig(state, config_path, argc, argv) < 0) {
		return -2;
	}
	state->binding.user_data = user_data;
	return 0;
}

void IZ_NetServerCancelService(IZ_NetServerState* state) {
	IZ_WSServerCancelService(&state->binding);
}

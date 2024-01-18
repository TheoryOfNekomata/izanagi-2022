#include "IZ_repo.h"

static INI_ConfigItem repo_config_items[] = {
	{
		INI_CONFIG_TYPE_STRING(64),
		"Database",
		"Path",
		"-d",
		&IZ_REPO_DEFAULT_STATE.config.path,
		NULL,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	},
	INI_CONFIG_ITEM_NULL,
};

void IZ_RepoBindStateToConfig(IZ_RepoState* state, INI_ConfigItem config_items[]) {
	config_items[0].dest = &state->config.path;
}

IZ_ProcedureResult IZ_RepoSaveConfig(IZ_RepoState* state, const char* config_path) {
	IZ_RepoBindStateToConfig(state, repo_config_items);
	return INI_ConfigSave(repo_config_items, config_path);
}

IZ_ProcedureResult IZ_RepoInitializeConfig(IZ_RepoState* state, const char* config_path, u8 argc, const char* argv[]) {
	IZ_RepoBindStateToConfig(state, repo_config_items);
	if (INI_ConfigInitialize(repo_config_items, config_path, argc, argv) < 0) {
		return -1;
	}
	return 0;
}

IZ_ProcedureResult IZ_RepoInitialize(IZ_RepoState* state, const char* config_path, u8 argc, const char* argv[]) {
	IZ_memcpy(state, sizeof(IZ_RepoState), &IZ_REPO_DEFAULT_STATE, sizeof(IZ_RepoState));
	if (IZ_RepoInitializeConfig(state, config_path, argc, argv) < 0) {
		return -2;
	}
	const char* base_path = SDL_GetBasePath();
	char config_path_dir[128];
	IZ_memcpy(config_path_dir, 128, base_path, 128);
	IZ_strcat(config_path_dir, 128, "/");
	IZ_strcat(config_path_dir, 128, state->config.path);
	sqlite3_open_v2(config_path_dir, &state->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

	INI_ConfigSaveResult post_config_save_result = IZ_RepoSaveConfig(state, config_path);
	if (post_config_save_result < 0) {
		return -3;
	}
	return 0;
}

void IZ_RepoTeardown(IZ_RepoState* state) {
	sqlite3_close_v2(state->db);
}

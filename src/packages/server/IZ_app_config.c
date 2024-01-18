#include "IZ_app_config.h"

void IZ_ConfigGetDefaultPath(char* config_path, size_t string_size) {
//#ifdef IZ_DEBUG
	const char* config_path_dir = SDL_GetBasePath();
//#else
//	const char* config_path_dir = SDL_GetPrefPath("Modal Studios", IZ_APP_NAME);
//#endif

	IZ_memcpy(config_path, string_size, config_path_dir, 128);
	IZ_strcat(config_path, string_size, IZ_CONFIG_SERVER_PATH);
}

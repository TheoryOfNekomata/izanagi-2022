#ifndef IZ_CONFIG_GUID_H
#define IZ_CONFIG_GUID_H

#include "SDL_guid.h"
#include "ini-config.h"
#include "minIni.h"

INI_CONFIG_DECLARE_TYPE(Guid);
#define INI_CONFIG_TYPE_GUID (INI_ConfigType) { \
	.size = sizeof(SDL_GUID), \
	.load = INI_ConfigLoadGuid, \
	.save = INI_ConfigSaveGuid, \
	.override = INI_ConfigOverrideGuid, \
}

#endif

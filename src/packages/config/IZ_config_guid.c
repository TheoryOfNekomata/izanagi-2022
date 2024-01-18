#include "IZ_config_guid.h"

typedef SDL_GUID INI_ConfigDeserializeGUID(const char*);

typedef void INI_ConfigSerializeGUID(SDL_GUID, const char[128]);

typedef bool INI_ConfigLoadParamsGuidValidator(SDL_GUID);

void INI_ConfigEnsureValidGuid(INI_ConfigItem* item, SDL_GUID raw_value, SDL_GUID default_value) {
	SDL_GUID* dest = item->dest;
	if (item->validator) {
		INI_ConfigLoadParamsGuidValidator* validate = item->validator;
		if (validate(raw_value)) {
			// within valid values
			*dest = raw_value;
			return;
		}
		// outside valid values, we use default value
		//
		// TODO: what if the default value is also invalid?
		*dest = default_value;
		return;
	}

	// no validator, get whatever is the deserialized value
	*dest = raw_value;
}

void INI_ConfigLoadGuid(INI_ConfigItem* item, const char* config_path, void* item_array) {
	static SDL_GUID raw_value;
	static SDL_GUID default_value = {
		.data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	if (item->default_value) {
		default_value = *((SDL_GUID *) item->default_value);
	}

	char buffer[33];
	if (item->transformer.deserialize && item->transformer.serialize) {
		INI_ConfigDeserializeGUID* deserialize = item->transformer.deserialize;
		INI_ConfigSerializeGUID* serialize = item->transformer.serialize;
		const char serialized_default_value[33];
		serialize(default_value, serialized_default_value);
		ini_gets(item->section, item->key, serialized_default_value, buffer, 33, config_path);
		raw_value = deserialize(buffer);
	} else {
		char guid_string_default_value[33];
		SDL_GUIDToString(default_value, guid_string_default_value, 33);
		ini_gets(item->section, item->key, guid_string_default_value, buffer, 33, config_path);
		raw_value = SDL_GUIDFromString(buffer);
	}
	INI_ConfigEnsureValidGuid(item, raw_value, default_value);
}

INI_ConfigSaveItemResult INI_ConfigSaveGuid(INI_ConfigItem* item, const char* config_path, void* item_array) {
	SDL_GUID dest = *((SDL_GUID*) item->dest);
	if (item->validator) {
		INI_ConfigLoadParamsGuidValidator* validate = item->validator;
		if (!validate(dest)) {
			dest = *((const SDL_GUID*) item->default_value);
		}
	}

	if (item->transformer.deserialize && item->transformer.serialize) {
		INI_ConfigSerializeGUID* serialize = item->transformer.serialize;
		const char serialized_value[128];
		serialize(dest, serialized_value);
		if (!ini_puts(item->section, item->key, serialized_value, config_path)) {
			return -1;
		}
		return 0;
	}

	char guid_str[33];
	memset(guid_str, 0, 33);
	SDL_GUIDToString(dest, guid_str, 33);
	if (!ini_puts(item->section, item->key, guid_str, config_path)) {
		return -1;
	}

	return 0;
}

void INI_ConfigOverrideGuid(INI_ConfigItem* item, uint8_t argc, const char* argv[]) {
	if (!item->cmdline_option) {
		return;
	}
	const char* cmdline_buffer;
	static SDL_GUID dest;
	static SDL_GUID config_value;
	config_value = *((SDL_GUID*) item->dest);
	if((cmdline_buffer = INI_ConfigGetCommandlineOption(argc, argv, item->cmdline_option))) {
		dest = SDL_GUIDFromString(cmdline_buffer);
		INI_ConfigEnsureValidGuid(item, dest, config_value);
	}
}

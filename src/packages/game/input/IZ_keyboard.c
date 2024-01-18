#include "IZ_keyboard.h"

static INI_ConfigItem keyboard_config_items[IZ_PLAYERS * IZ_CONTROLS + 1];

void IZ_KeyboardSerializeControl(i32 value, char control[128], void* _unused) {
	const char* serialized = SDL_GetKeyName(value);
	IZ_memcpy(control, 128, serialized, 128);
}

i32 IZ_KeyboardDeserializeControl(const char* control, void* _unused) {
	return SDL_GetKeyFromName(control);
}

void IZ_KeyboardHandleKeyUpDownEvents(IZ_KeyboardState* state, IZ_Action* action, SDL_Event e) {
	u8 control_index;
	for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
		if (e.key.keysym.sym == state->config.control_mapping[control_index]) {
			const u16 bitflag = (0x1 << control_index);
			if (e.type == SDL_KEYDOWN && !(*action & bitflag)) {
				IZ_LogInfo(
					IZ_LOG_CATEGORY_INPUT,
					"input/keyboard",
					"dn %s (%s)",
					IZ_ACTION_NAMES[control_index],
					SDL_GetKeyName(e.key.keysym.sym)
				);
				*action |= bitflag;
				return;
			}
			if (e.type == SDL_KEYUP && (*action & bitflag)) {
				IZ_LogInfo(
					IZ_LOG_CATEGORY_INPUT,
					"input/keyboard",
					"up %s (%s)",
					IZ_ACTION_NAMES[control_index],
					SDL_GetKeyName(e.key.keysym.sym)
				);
				*action &= ~bitflag;
				return;
			}
		}
	}
}

void IZ_KeyboardHandleEvents(IZ_KeyboardState(* state)[IZ_PLAYERS], IZ_Action(* action)[IZ_PLAYERS], SDL_Event e) {
	for (u8 player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		IZ_KeyboardHandleKeyUpDownEvents(&(*state)[player_index], &(*action)[player_index], e);
	}
}

void IZ_KeyboardBindStateToConfig(IZ_KeyboardState(* state)[IZ_PLAYERS], INI_ConfigItem config_items[]) {
	u8 player_index;
	u8 control_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
			config_items[player_index * IZ_CONTROLS + control_index].dest = &((*state)[player_index].config.control_mapping[control_index]);
		}
	}
}

IZ_ProcedureResult IZ_KeyboardSaveConfig(IZ_KeyboardState(* state)[IZ_PLAYERS], const char* config_path) {
	IZ_KeyboardBindStateToConfig(state, keyboard_config_items);
	return INI_ConfigSave(keyboard_config_items, config_path);
}

void IZ_KeyboardInitializeConfigItems(INI_ConfigItem config_items[]) {
	u8 player_index;
	u8 control_index;
	char* control_mapping_section_name;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		control_mapping_section_name = IZ_calloc(64, sizeof(char));
		sprintf(control_mapping_section_name, "Keyboard.%d.ControlMapping", player_index);
		for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
			config_items[player_index * IZ_CONTROLS + control_index] = (INI_ConfigItem) {
				INI_CONFIG_TYPE_I32,
				control_mapping_section_name,
				IZ_ACTION_NAMES[control_index],
				NULL,
				&IZ_KEYBOARD_DEFAULT_STATE[player_index].config.control_mapping[control_index],
				NULL,
				{
					.serialize = IZ_KeyboardSerializeControl,
					.deserialize = IZ_KeyboardDeserializeControl,
				},
				NULL,
			};
		}
	}

	config_items[IZ_PLAYERS * IZ_CONTROLS] = INI_CONFIG_ITEM_NULL;
}

IZ_ProcedureResult IZ_KeyboardInitializeConfig(IZ_KeyboardState(* state)[IZ_PLAYERS], const char* config_path, u8 argc, const char* argv[]) {
	IZ_KeyboardInitializeConfigItems(keyboard_config_items);
	IZ_KeyboardBindStateToConfig(state, keyboard_config_items);
	if (INI_ConfigInitialize(keyboard_config_items, config_path, argc, argv) < 0) {
		return -1;
	}
	return 0;
}

IZ_ProcedureResult IZ_KeyboardInitialize(IZ_KeyboardState(* state)[IZ_PLAYERS], const char* config_path, u8 argc, const char* argv[]) {
	IZ_memcpy(state, sizeof(IZ_KeyboardState) * IZ_PLAYERS, &IZ_KEYBOARD_DEFAULT_STATE, sizeof(IZ_KeyboardState) * IZ_PLAYERS);
	if (IZ_KeyboardInitializeConfig(state, config_path, argc, argv) < 0) {
		return -2;
	}

	INI_ConfigSaveResult post_config_save_result = IZ_KeyboardSaveConfig(state, config_path);
	if (post_config_save_result < 0) {
		return -3;
	}

	return 0;
}

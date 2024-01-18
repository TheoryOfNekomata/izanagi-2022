#include "IZ_gamecontroller.h"

static INI_ConfigItem game_controller_config_items[IZ_PLAYERS * (IZ_CONTROLS + IZ_GAME_CONTROLLER_DEFAULT_CONFIGS) + 1];

bool IZ_GameControllerIsValidAxisThreshold(u16 value) {
	return (IZ_GAME_CONTROLLER_MIN_AXIS_THRESHOLD <= value && value <= IZ_GAME_CONTROLLER_MAX_AXIS_THRESHOLD);
}

void IZ_GameControllerHandleDeviceEvents(IZ_GameControllerState* state, SDL_Event e) {
	if (e.type == SDL_CONTROLLERDEVICEADDED) {
		u8 game_controllers_count = SDL_NumJoysticks();
		if (game_controllers_count <= IZ_PLAYERS && !state->device) {
			state->device = SDL_GameControllerOpen(e.cdevice.which);
		}
		return;
	}

	if (e.type == SDL_CONTROLLERDEVICEREMOVED) {
		if (
			state->device
			&& SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(state->device)) == e.cdevice.which
		) {
			state->device = NULL;
		}
	}
}

void IZ_GameControllerHandleAxisEvents(IZ_GameControllerState* state, IZ_Action* action, SDL_Event e) {
	if (e.type != SDL_CONTROLLERAXISMOTION) {
		return;
	}

	u8 control_index;

	for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
		const char* current_axis_name = SDL_GameControllerGetStringForAxis(e.caxis.axis);
		if (!current_axis_name) {
			continue;
		}

		char current_positive_axis_full_name[IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH];
		sprintf(
			current_positive_axis_full_name,
			"axis:+%s",
			current_axis_name
		);

		char current_negative_axis_full_name[IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH];
		sprintf(
			current_negative_axis_full_name,
			"axis:-%s",
			current_axis_name
		);

		bool is_capture = false;
		const u16 bitflag = (0x1 << control_index);
		if (strstr(state->config.control_mapping[control_index], current_positive_axis_full_name)) {
			if (*action & bitflag) {
				IZ_LogInfo(IZ_LOG_CATEGORY_INPUT, "input/gamepad", "up %s", IZ_ACTION_NAMES[control_index]);
			}
			// TODO: optimize unsetting of action
			*action &= ~bitflag;
			// should we implement actions that do not cancel out across input controllers?
			//   add extra byte for source of action:
			//    0x1 - keyboard
			//    0x2 - game controller dpad
			//    0x4 - game controller left stick
			//    0x8 - game controller right stick
			//   0x10 - other device
		}

		if (strstr(state->config.control_mapping[control_index], current_negative_axis_full_name)) {
			if (*action & bitflag) {
				IZ_LogInfo(IZ_LOG_CATEGORY_INPUT, "input/gamepad", "up %s", IZ_ACTION_NAMES[control_index]);
			}
			*action &= ~bitflag;
			// should we implement actions that do not cancel out across input controllers?
			//   add extra byte for source of action:
			//    0x1 - keyboard
			//    0x2 - game controller dpad
			//    0x4 - game controller left stick
			//    0x8 - game controller right stick
			//   0x10 - other device
		}

		char current_axis_full_name[IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH];
		IZ_memset(current_axis_full_name, 0, IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH);
		if (e.caxis.value > 0) {
			IZ_memcpy(
				current_axis_full_name,
				IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH,
				current_positive_axis_full_name,
				IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH
			);
			is_capture = e.caxis.value > state->config.axis_threshold;
		}
		if (e.caxis.value < 0) {
			IZ_memcpy(
				current_axis_full_name,
				IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH,
				current_negative_axis_full_name,
				IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH
			);
			is_capture = e.jaxis.value <= -state->config.axis_threshold;
		}

		if (!strstr(state->config.control_mapping[control_index], current_axis_full_name)) {
			continue;
		}

		if (!is_capture) {
			continue;
		}

		*action |= bitflag;
		IZ_LogInfo(IZ_LOG_CATEGORY_INPUT, "input/gamepad", "dn %s (%s)", IZ_ACTION_NAMES[control_index], current_axis_full_name);
	}
}

void IZ_GameControllerHandleButtonEvents(IZ_GameControllerState* state, IZ_Action* action, SDL_Event e) {
	u8 control_index;
	for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
		const char* current_button_name = SDL_GameControllerGetStringForButton(e.cbutton.button);
		if (!current_button_name) {
			continue;
		}

		char current_button_full_name[IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH];
		sprintf(
			current_button_full_name,
			"button:%s",
			current_button_name
		);

		if (!IZ_contains(state->config.control_mapping[control_index], IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH, current_button_full_name, IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH)) {
			continue;
		}

		const u16 bitflag = (0x1 << control_index);
		if (e.type == SDL_CONTROLLERBUTTONDOWN) {
			IZ_LogInfo(IZ_LOG_CATEGORY_INPUT, "input/gamepad", "dn %s (%s)", IZ_ACTION_NAMES[control_index], current_button_full_name);
			*action |= bitflag;
			return;
		}

		if (e.type == SDL_CONTROLLERBUTTONUP) {
			IZ_LogInfo(IZ_LOG_CATEGORY_INPUT, "input/gamepad", "up %s (%s)", IZ_ACTION_NAMES[control_index], current_button_full_name);
			*action &= ~bitflag;
			return;
		}
	}
}

void IZ_GameControllerHandleEvents(IZ_GameControllerState(* state)[IZ_PLAYERS], IZ_Action(* action)[IZ_PLAYERS], SDL_Event e) {
	u8 player_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		IZ_GameControllerHandleDeviceEvents(&(*state)[player_index], e);
		IZ_GameControllerHandleAxisEvents(&(*state)[player_index], &(*action)[player_index], e);
		IZ_GameControllerHandleButtonEvents(&(*state)[player_index], &(*action)[player_index], e);
	}
}

void IZ_GameControllerBindStateToConfig(IZ_GameControllerState(* state)[IZ_PLAYERS], INI_ConfigItem config_items[]) {
	u8 player_index;
	u8 control_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		u8 base_index = (player_index * (IZ_CONTROLS + IZ_GAME_CONTROLLER_DEFAULT_CONFIGS));
		config_items[base_index + 0].dest = &((*state)[player_index].config.axis_threshold);
		config_items[base_index + 1].dest = &((*state)[player_index].config.guid);
		for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
			config_items[base_index + IZ_GAME_CONTROLLER_DEFAULT_CONFIGS + control_index].dest = &((*state)[player_index].config.control_mapping[control_index]);
		}
	}
}

IZ_ProcedureResult IZ_GameControllerSaveConfig(IZ_GameControllerState(* state)[IZ_PLAYERS], const char* config_path) {
	IZ_GameControllerBindStateToConfig(state, game_controller_config_items);
	return INI_ConfigSave(game_controller_config_items, config_path);
}

void IZ_GameControllerInitializeConfigItems(INI_ConfigItem config_items[]) {
	u8 player_index;
	u8 control_index;
	char* main_section_name;
	char* control_mapping_section_name;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		main_section_name = IZ_calloc(64, sizeof(char));
		sprintf(main_section_name, "GameController.%d", player_index);
		u8 base_index = (player_index * (IZ_CONTROLS + IZ_GAME_CONTROLLER_DEFAULT_CONFIGS));
		config_items[base_index + 0] = (INI_ConfigItem) {
			INI_CONFIG_TYPE_U16,
			main_section_name,
			"AxisThreshold",
			NULL,
			&IZ_GAME_CONTROLLER_DEFAULT_STATE[player_index].config.axis_threshold,
			IZ_GameControllerIsValidAxisThreshold,
			INI_CONFIG_TRANSFORMER_NONE,
			NULL,
		};

		config_items[base_index + 1] = (INI_ConfigItem) {
			INI_CONFIG_TYPE_GUID,
			main_section_name,
			"GUID",
			NULL,
			&IZ_GAME_CONTROLLER_DEFAULT_STATE[player_index].config.guid,
			NULL,
			INI_CONFIG_TRANSFORMER_NONE,
			NULL,
		};

		control_mapping_section_name = IZ_calloc(64, sizeof(char));
		sprintf(control_mapping_section_name, "GameController.%d.ControlMapping", player_index);
		for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
			config_items[base_index + IZ_GAME_CONTROLLER_DEFAULT_CONFIGS + control_index] = (INI_ConfigItem) {
				INI_CONFIG_TYPE_STRING(IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH),
				control_mapping_section_name,
				IZ_ACTION_NAMES[control_index],
				NULL,
				&IZ_GAME_CONTROLLER_DEFAULT_STATE[player_index].config.control_mapping[control_index],
				NULL,
				INI_CONFIG_TRANSFORMER_NONE,
				NULL,
			};
		}
	}

	config_items[IZ_PLAYERS * (IZ_CONTROLS + IZ_GAME_CONTROLLER_DEFAULT_CONFIGS)] = INI_CONFIG_ITEM_NULL;
}

IZ_ProcedureResult IZ_GameControllerInitializeConfig(IZ_GameControllerState(* state)[IZ_PLAYERS], const char* config_path, u8 argc, const char* argv[]) {
	IZ_GameControllerInitializeConfigItems(game_controller_config_items);
	IZ_GameControllerBindStateToConfig(state, game_controller_config_items);
	if (INI_ConfigInitialize(game_controller_config_items, config_path, argc, argv) < 0) {
		return -1;
	}
	return 0;
}

IZ_ProcedureResult IZ_GameControllerInitialize(IZ_GameControllerState(* state)[IZ_PLAYERS], const char* config_path, u8 argc, const char* argv[]) {
	IZ_memcpy(state, sizeof(IZ_GameControllerState) * IZ_PLAYERS, &IZ_GAME_CONTROLLER_DEFAULT_STATE, sizeof(IZ_GameControllerState) * IZ_PLAYERS);

	// TODO query this file from the internet?
	//   Download from https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt
	//   Add the subproject
	IZ_LogInfo(IZ_LOG_CATEGORY_GENERIC, "input", "Loading game controller mappings file: %s", "assets/gamecontrollerdb.txt");
	const i16 loaded_mappings = SDL_GameControllerAddMappingsFromFile("assets/gamecontrollerdb.txt");
	if (loaded_mappings <= 0) {
		return -1;
	}
	IZ_LogInfo(IZ_LOG_CATEGORY_GENERIC, "input", "Mappings loaded: %d", loaded_mappings);
	u8 player_index;

	if (IZ_GameControllerInitializeConfig(state, config_path, argc, argv) < 0) {
		return -2;
	}

	u8 game_controllers_count = SDL_NumJoysticks();
	for (player_index = 0; player_index < game_controllers_count; player_index += 1) {
		if (player_index >= IZ_PLAYERS) {
			break;
		}

		(*state)[player_index].device = SDL_GameControllerOpen(state[player_index]->config.device_id);
		if (!(*state)[player_index].device) {
			break;
		}

		(*state)[player_index].config.device_id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick((*state)[player_index].device));
		SDL_GUID game_controller_guid = SDL_JoystickGetGUID(SDL_GameControllerGetJoystick((*state)[player_index].device));
		IZ_memcpy(&(*state)[player_index].config.guid, sizeof(SDL_GUID), &game_controller_guid, sizeof(SDL_GUID));

		IZ_LogInfo(
			IZ_LOG_CATEGORY_GENERIC,
			"input/gamepad",
			"Found device for player %u: %s",
			player_index,
			SDL_GameControllerName((*state)[player_index].device)
		);
	}

	// Post config (after game_controller GUIDs have been queried), this is unique to game_controllers since they can be plugged in any
	// time.
	INI_ConfigSaveResult post_config_save_result = IZ_GameControllerSaveConfig(state, config_path);
	if (post_config_save_result < 0) {
		return -3;
	}

	return 0;
}

void IZ_GameControllerTeardown(IZ_GameControllerState(* state)[IZ_PLAYERS]) {
	u8 player_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		if (!(*state)[player_index].device) {
			continue;
		}
		SDL_GameControllerClose((*state)[player_index].device);
	}
}

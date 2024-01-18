#include "IZ_midi.h"

static INI_ConfigItem midi_input_config_items[(IZ_PLAYERS * (IZ_CONTROLS + 2)) + 1];

bool IZ_MIDIInputIsValidChannel(u8 value) {
	return (0 <= value && value <= 15);
}

void IZ_MIDIInputSerializeControl(i32 value, char* control[128]) {
	const char* serialized = MIDI_GetNoteName(value);
	IZ_memcpy(control, 128, serialized, 128);
}

i32 IZ_MIDIInputDeserializeControl(const char* control) {
	return MIDI_GetNoteFromName(control);
}

void IZ_MIDIInputHandleNoteOnOffEvents(IZ_MIDIInputState* state, IZ_Action* action, PmEvent e) {
	u32 message = e.message;
	u8 status = message & 0xF0u;
	u8 channel = message & 0x0Fu;
	u8 data1 = (message >> 8) & 0xFFu;
	// u8 data2 = (message >> 16) & 0xFFu;

	u8 control_index;
	for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
		if (
			data1 == state->config.control_mapping[control_index]
			&& (
				(state->config.channel < 16 && channel == state->config.channel)
				|| state->config.channel >= 16
			)
		) {
			const u16 bitflag = (0x1 << control_index);
			if (status == MIDI_MESSAGE_NOTEON) {
				*action |= bitflag;
				return;
			}
			if (status == MIDI_MESSAGE_NOTEOFF) {
				*action &= ~bitflag;
				return;
			}
		}
	}
}

void IZ_MIDIInputHandleEvents(IZ_MIDIInputState(* state)[IZ_PLAYERS], IZ_Action(* action)[IZ_PLAYERS], PmEvent e) {
	u8 player_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		IZ_MIDIInputHandleNoteOnOffEvents(&(*state)[player_index], &(*action)[player_index], e);
	}
}

void IZ_MIDIInputBindStateToConfig(IZ_MIDIInputState(* state)[IZ_PLAYERS], INI_ConfigItem config_items[]) {
	u8 player_index;
	u8 control_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		u8 base_index = (player_index * (IZ_CONTROLS + 2));
		config_items[base_index].dest = &((*state)[player_index].config.device_id);
		config_items[base_index + 1].dest = &((*state)[player_index].config.channel);
		for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
			config_items[base_index + 2 + control_index].dest = &((*state)[player_index].config.control_mapping[control_index]);
		}
	}
}

IZ_ProcedureResult IZ_MIDIInputSaveConfig(IZ_MIDIInputState(* state)[IZ_PLAYERS], const char* config_path) {
	IZ_MIDIInputBindStateToConfig(state, midi_input_config_items);
	return INI_ConfigSave(midi_input_config_items, config_path);
}

void IZ_MIDIInputInitializeConfigItems(INI_ConfigItem config_items[]) {
	u8 player_index;
	u8 control_index;

	char* main_section_name;
	char* control_mapping_section_name;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		main_section_name = IZ_calloc(64, sizeof(char));
		sprintf(main_section_name, "MIDIInput.%d", player_index);

		u16 base_index = (player_index * (IZ_CONTROLS + 2));
		config_items[base_index] = (INI_ConfigItem) {
			INI_CONFIG_TYPE_I32,
			main_section_name,
			"DeviceID",
			NULL,
			&IZ_MIDI_INPUT_DEFAULT_STATE[player_index].config.device_id,
			NULL,
			INI_CONFIG_TRANSFORMER_NONE,
			NULL,
		};

		config_items[base_index + 1] = (INI_ConfigItem) {
			INI_CONFIG_TYPE_U8,
			main_section_name,
			"Channel",
			NULL,
			&IZ_MIDI_INPUT_DEFAULT_STATE[player_index].config.channel,
			IZ_MIDIInputIsValidChannel,
			INI_CONFIG_TRANSFORMER_NONE,
			NULL,
		};

		control_mapping_section_name = IZ_calloc(64, sizeof(char));
		sprintf(control_mapping_section_name, "MIDIInput.%d.ControlMapping", player_index);
		for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
			config_items[base_index + 2 + control_index] = (INI_ConfigItem) {
				INI_CONFIG_TYPE_U8,
				control_mapping_section_name,
				IZ_ACTION_NAMES[control_index],
				NULL,
				&IZ_MIDI_INPUT_DEFAULT_STATE[player_index].config.control_mapping[control_index],
				NULL,
				{
					.serialize = IZ_MIDIInputSerializeControl,
					.deserialize = IZ_MIDIInputDeserializeControl,
				},
				NULL,
			};
		}
	}

	config_items[IZ_PLAYERS * (IZ_CONTROLS + 2)] = INI_CONFIG_ITEM_NULL;
}

IZ_ProcedureResult IZ_MIDIInputInitializeConfig(IZ_MIDIInputState(* state)[IZ_PLAYERS], const char* config_path, u8 argc, const char* argv[]) {
	IZ_MIDIInputInitializeConfigItems(midi_input_config_items);
	IZ_MIDIInputBindStateToConfig(state, midi_input_config_items);
	if (INI_ConfigInitialize(midi_input_config_items, config_path, argc, argv) < 0) {
		return -1;
	}
	return 0;
}

IZ_ProcedureResult IZ_MIDIInputInitialize(IZ_MIDIInputState(* state)[IZ_PLAYERS], const char* config_path, u8 argc, const char* argv[]) {
	if (Pm_Initialize()) {
		return -1;
	}

	IZ_memcpy(state, sizeof(IZ_MIDIInputState) * IZ_PLAYERS, &IZ_MIDI_INPUT_DEFAULT_STATE, sizeof(IZ_MIDIInputState) * IZ_PLAYERS);
	if (IZ_MIDIInputInitializeConfig(state, config_path, argc, argv) < 0) {
		return -2;
	}

	u8 player_index;
	u8 midi_devices_count = Pm_CountDevices();
	u8 input_midi_devices_count = 0;
	u8 device_index;
	for (device_index = 0; device_index < midi_devices_count; device_index += 1) {
		if (!Pm_GetDeviceInfo(device_index)->input) {
			continue;
		}

		// midi device can output messages for the app to receive
		input_midi_devices_count += 1;
	}

	for (player_index = 0; player_index < input_midi_devices_count; player_index += 1) {
		if (player_index >= IZ_PLAYERS) {
			break;
		}
		(*state)[player_index].device_info = Pm_GetDeviceInfo((*state)[player_index].config.device_id);
		(*state)[player_index].stream = NULL;
		Pm_OpenInput(
			&(*state)[player_index].stream,
			(*state)[player_index].config.device_id,
			NULL,
			MIDI_EVENT_BUFFER_SIZE,
			NULL,
			NULL
		);
	}

	INI_ConfigSaveResult post_config_save_result = IZ_MIDIInputSaveConfig(state, config_path);
	if (post_config_save_result < 0) {
		return -3;
	}

	return 0;
}

void IZ_MIDIInputTeardown(IZ_MIDIInputState(* state)[IZ_PLAYERS]) {
	u8 player_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		if (!(*state)[player_index].stream) {
			continue;
		}
		Pm_Close((*state)[player_index].stream);
	}
}

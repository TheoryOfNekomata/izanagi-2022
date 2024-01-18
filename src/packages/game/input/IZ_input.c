#include "IZ_input.h"

void IZ_InputHandleSDLEvents(IZ_InputState* state, SDL_Event e) {
	IZ_GameControllerHandleEvents(&state->game_controller_state, &state->action, e);
	IZ_KeyboardHandleEvents(&state->keyboard_state, &state->action, e);
}

void IZ_InputHandlePortMIDIEvents(IZ_InputState* state, PmEvent e) {
	IZ_MIDIInputHandleEvents(&state->midi_input_state, &state->action, e);
}

IZ_ProcedureResult IZ_InputInitialize(IZ_InputState* state, const char* config_path, u8 argc, const char* argv[]) {
	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "input", "Setting up...");

	*state = (IZ_InputState) {
		.action = {},
		.game_controller_state = {},
		.midi_input_state = {},
		.keyboard_state = {},
	};

	IZ_ProcedureResult result = 0;

	if (IZ_GameControllerInitialize(&state->game_controller_state, config_path, argc, argv) < 0) {
		result |= 1;
	}

	if (IZ_KeyboardInitialize(&state->keyboard_state, config_path, argc, argv) < 0) {
		result |= 2;
	}

	if (IZ_MIDIInputInitialize(&state->midi_input_state, config_path, argc, argv) < 0) {
		result |= 4;
	}

	u8 player_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		state->action[player_index] = 0;
	}

	IZ_ProcedureResult final_result = -result;

	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "input", "Setup complete. Result: %d", final_result);

	return final_result;
}

void IZ_InputTeardown(IZ_InputState* state) {
	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "input", "Shutting down...");
	IZ_GameControllerTeardown(&state->game_controller_state);
	IZ_MIDIInputTeardown(&state->midi_input_state);
}

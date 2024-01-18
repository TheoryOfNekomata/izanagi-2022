#include "IZ_app_input.h"

#if defined IZ_WINDOWS
	#define IZ_DEBUG_CONNECT SDLK_PAGEUP
	#define IZ_DEBUG_DISCONNECT SDLK_PAGEDOWN
	#define IZ_DEBUG_SEND_MESSAGE SDLK_INSERT
#elif defined IZ_UNIX
    #define IZ_DEBUG_CONNECT SDLK_PAGEUP
    #define IZ_DEBUG_DISCONNECT SDLK_PAGEDOWN
    #define IZ_DEBUG_SEND_MESSAGE SDLK_INSERT
#elif defined IZ_MACOS
	#define IZ_DEBUG_CONNECT SDLK_EQUALS
	#define IZ_DEBUG_DISCONNECT SDLK_MINUS
	#define IZ_DEBUG_SEND_MESSAGE SDLK_BACKSLASH
#endif

IZ_ProcedureResult IZ_AppHandleSDLEvents(struct IZ_App* app) {
	SDL_Event e;
	IZ_InputState* input_state = IZ_AppGetInputState(app);
	IZ_NetClientState* net_state = IZ_AppGetNetState(app);

	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			return 1;
		}

#ifdef IZ_DEBUG
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == IZ_DEBUG_CONNECT) {
				IZ_NetClientConnect(
					net_state,
					(IZ_WSClientInitializeParams) {
						.host = "127.0.0.1",
						.path = "/",
						.port = 42069,
					}
				);
			} else if (e.key.keysym.sym == IZ_DEBUG_DISCONNECT) {
				IZ_NetClientDisconnect(net_state);
			} else if (e.key.keysym.sym == IZ_DEBUG_SEND_MESSAGE) {
				IZ_NetClientSendTextMessage(net_state, "hello", 5);
			}
		}
#endif

		IZ_InputHandleSDLEvents(input_state, e);
	}
	return 0;
}

void IZ_AppHandlePortMIDIEvents(struct IZ_App* app) {
	IZ_InputState* input_state = IZ_AppGetInputState(app);
	u8 player_index;
	i32* midi_events_count;
	u32 midi_event_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		if (!input_state->midi_input_state[player_index].device_info) {
			continue;
		}

		midi_events_count = &input_state->midi_input_state[player_index].midi_events_count;
		*midi_events_count = Pm_Read(
			input_state->midi_input_state[player_index].stream,
			// TODO bind buffers and streams to device instead of player input state
			input_state->midi_input_state[player_index].event_buffer,
			1024
		);

		if (*midi_events_count < 1) {
			continue;
		}

		for (midi_event_index = 0; midi_event_index < *midi_events_count; midi_event_index += 1) {
			IZ_InputHandlePortMIDIEvents(
				input_state,
				input_state->midi_input_state[player_index].event_buffer[midi_event_index]
			);
		}
	}
}

IZ_ProcedureResult IZ_AppHandleInputEvents(struct IZ_App* app) {
	i32 sdl_events_result = IZ_AppHandleSDLEvents(app);
	if (sdl_events_result) {
		return sdl_events_result;
	}

	IZ_AppHandlePortMIDIEvents(app);
	return 0;
}

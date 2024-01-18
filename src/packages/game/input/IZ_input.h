#ifndef IZ_INPUT_H
#define IZ_INPUT_H

#include "IZ_action.h"
#include "IZ_keyboard.h"
#include "IZ_gamecontroller.h"
#include "IZ_midi.h"
#include "../../log/IZ_log.h"

typedef struct {
	IZ_Action action[IZ_PLAYERS];
	IZ_KeyboardState keyboard_state[IZ_PLAYERS];
	IZ_GameControllerState game_controller_state[IZ_PLAYERS];
	IZ_MIDIInputState midi_input_state[IZ_PLAYERS];
} IZ_InputState;

void IZ_InputHandleSDLEvents(IZ_InputState*, SDL_Event);

void IZ_InputHandlePortMIDIEvents(IZ_InputState*, PmEvent);

IZ_ProcedureResult IZ_InputInitialize(IZ_InputState*, const char*, u8, const char*[]);

void IZ_InputTeardown(IZ_InputState*);

#endif

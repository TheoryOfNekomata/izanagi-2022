#ifndef IZ_KEYBOARD_H
#define IZ_KEYBOARD_H

#include "SDL_keyboard.h"
#include "SDL_events.h"
#include "ini-config.h"
#include "types/int.h"
#include "../../stdinc/IZ_string.h"
#include "../../stdinc/IZ_stdlib.h"
#include "../../log/IZ_log.h"
#include "IZ_action.h"

typedef struct {
	SDL_KeyCode control_mapping[IZ_CONTROLS];
} IZ_KeyboardConfig;

typedef struct {
	IZ_KeyboardConfig config;
} IZ_KeyboardState;

static const IZ_KeyboardState IZ_KEYBOARD_DEFAULT_STATE[IZ_PLAYERS] = {
	{
		.config = {
			.control_mapping = {
				SDLK_UP,
				SDLK_RIGHT,
				SDLK_DOWN,
				SDLK_LEFT,
				SDLK_RETURN, // yes
				SDLK_BACKSPACE, // no
				SDLK_a, // action0
				SDLK_s, // action1
				SDLK_d, // action2
				SDLK_f, // action3
				SDLK_z, // action4
				SDLK_x, // action5
				SDLK_c, // action6
				SDLK_v, // action7
				SDLK_w, // action8
				SDLK_e, // action9
			},
		},
	},
#if IZ_PLAYERS > 1
	{
		.config = {
			.control_mapping = {
				SDLK_KP_8,
				SDLK_KP_6,
				SDLK_KP_5,
				SDLK_KP_4,
				SDLK_KP_ENTER, // yes
				SDLK_KP_MINUS, // no
				SDLK_j, // action0
				SDLK_k, // action1
				SDLK_l, // action2
				SDLK_SEMICOLON, // action3
				SDLK_m, // action4
				SDLK_COMMA, // action5
				SDLK_PERIOD, // action6
				SDLK_SLASH, // action7
				SDLK_i, // action8
				SDLK_o, // action9
			},
		},
	},
#endif
};

IZ_ProcedureResult IZ_KeyboardSaveConfig(IZ_KeyboardState(*)[IZ_PLAYERS], const char*);

void IZ_KeyboardHandleEvents(IZ_KeyboardState(*)[IZ_PLAYERS], IZ_Action(*)[IZ_PLAYERS], SDL_Event);

IZ_ProcedureResult IZ_KeyboardInitialize(IZ_KeyboardState(*)[IZ_PLAYERS], const char*, u8, const char*[]);

#endif

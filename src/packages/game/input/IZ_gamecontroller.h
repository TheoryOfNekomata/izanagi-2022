#ifndef IZ_GAMECONTROLLER_H
#define IZ_GAMECONTROLLER_H

#include "SDL_gamecontroller.h"
#include "SDL_events.h"
#include "ini-config.h"
#include "types/int.h"
#include "types/string.h"
#include "../../config/IZ_config_guid.h"
#include "../../common/IZ_common.h"
#include "../../stdinc/IZ_string.h"
#include "../../stdinc/IZ_stdlib.h"
#include "../../log/IZ_log.h"
#include "IZ_action.h"

#define IZ_GAME_CONTROLLER_AXIS_THRESHOLD ((u16) 8000)

#define IZ_GAME_CONTROLLER_MIN_AXIS_THRESHOLD ((u16) 4000)
#define IZ_GAME_CONTROLLER_MAX_AXIS_THRESHOLD ((u16) 12000)

#define IZ_GAME_CONTROLLER_DEFAULT_CONFIGS 2

#define IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH 64

typedef struct {
	u16 axis_threshold;
	SDL_JoystickID device_id;
	SDL_GUID guid;
	const char control_mapping[IZ_CONTROLS][IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH];
} IZ_GameControllerConfig;

typedef struct {
	SDL_GameController* device;
	IZ_GameControllerConfig config;
} IZ_GameControllerState;

static const IZ_GameControllerState IZ_GAME_CONTROLLER_DEFAULT_STATE[IZ_PLAYERS] = {
	{
		.config = {
			.control_mapping = {
				"button:dpup axis:-lefty axis:-righty",
				"button:dpright axis:+leftx axis:+rightx",
				"button:dpdown axis:+lefty axis:+righty",
				"button:dpleft axis:-leftx axis:-rightx",
				"button:start",
				"button:back",
				"button:x",
				"axis:+righttrigger",
				"axis:+lefttrigger",
				"button:a",
				"button:y",
				"button:rightshoulder",
				"button:leftstick",
				"button:rightstick",
				"button:leftshoulder",
				"button:b",
			},
			.axis_threshold = IZ_GAME_CONTROLLER_AXIS_THRESHOLD,
			.guid = {
				.data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			},
		},
		.device = NULL,
	},
#if IZ_PLAYERS > 1
	{
		.config = {
			.control_mapping = {
				"button:dpadup",
				"button:dpadright",
				"button:dpaddown",
				"button:dpadleft",
				"button:start",
				"button:back",
				"button:x",
				"button:y",
				"button:a",
				"button:b",
				"button:rightshoulder",
				"axis:righttrigger",
				"button:leftstick",
				"button:rightstick",
				"button:leftshoulder",
				"axis:lefttrigger",
			},
			.axis_threshold = 8000u,
			.guid = {
				.data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			},
		},
		.device = NULL,
	},
#endif
};

IZ_ProcedureResult IZ_GameControllerSaveConfig(IZ_GameControllerState(*)[IZ_PLAYERS], const char*);

void IZ_GameControllerHandleEvents(IZ_GameControllerState(*)[IZ_PLAYERS], IZ_Action(*)[IZ_PLAYERS], SDL_Event);

IZ_ProcedureResult IZ_GameControllerInitialize(IZ_GameControllerState(*)[IZ_PLAYERS], const char*, u8, const char*[]);

void IZ_GameControllerTeardown(IZ_GameControllerState(*)[IZ_PLAYERS]);

#endif

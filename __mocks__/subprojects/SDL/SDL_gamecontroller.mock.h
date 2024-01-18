#ifndef SDL_GAMECONTROLLER_MOCK_H
#define SDL_GAMECONTROLLER_MOCK_H

#include <bdd-for-c-mocks.h>
#include <SDL_gamecontroller.h>
#include <SDL_rwops.h>
#include "../../../src/packages/common/IZ_common.h"

#define MOCK_OPEN_JOYSTICKS 1

mock(SDL_GameControllerOpen) SDL_GameController* SDL_GameControllerOpen(i32 device_index) {
	static void* joystick = (void*) 1;
	mock_return(SDL_GameControllerOpen) (SDL_GameController*) &joystick;
}

mock(SDL_NumJoysticks) i32 SDL_NumJoysticks(void) {
	mock_return(SDL_NumJoysticks) MOCK_OPEN_JOYSTICKS;
}

mock(SDL_JoystickInstanceID) i32 SDL_JoystickInstanceID(SDL_Joystick* joystick) {
	mock_return(SDL_JoystickInstanceID) 0;
}

mock(SDL_GameControllerClose) void SDL_GameControllerClose(SDL_GameController* _joystick) {
	mock_return(SDL_GameControllerClose);
}

mock(SDL_JoystickGetGUID) SDL_JoystickGUID SDL_JoystickGetGUID(SDL_Joystick* joystick) {
	mock_return(SDL_JoystickGetGUID) (SDL_JoystickGUID) {
		.data = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, }
	};
}

mock(SDL_GameControllerAddMappingsFromRW) int SDL_GameControllerAddMappingsFromRW(SDL_RWops *rw, int freerw) {
	mock_return(SDL_GameControllerAddMappingsFromRW) 1;
}

mock(SDL_GameControllerFromInstanceID) SDL_GameController* SDL_GameControllerFromInstanceID(SDL_JoystickID instance_id) {
	static void* joystick = (void*) 1;
	mock_return(SDL_GameControllerFromInstanceID) (SDL_GameController*) &joystick;
}

mock(SDL_GameControllerGetJoystick) SDL_Joystick* SDL_GameControllerGetJoystick(SDL_GameController* gamecontroller) {
	static void* joystick = (void*) 1;
	mock_return(SDL_GameControllerGetJoystick) (SDL_Joystick*) &joystick;
}

#endif

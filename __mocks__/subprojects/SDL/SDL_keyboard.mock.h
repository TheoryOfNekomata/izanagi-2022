#ifndef SDL_KEYBOARD_MOCK_H
#define SDL_KEYBOARD_MOCK_H

#include <bdd-for-c-mocks.h>
#include "../../../src/packages/common/IZ_common.h"

mock(SDL_GetKeyName) const char* SDL_GetKeyName(i32 key) {
	mock_return(SDL_GetKeyName) "";
}

mock(SDL_GetKeyFromName) i32 SDL_GetKeyFromName(const char *name) {
	mock_return(SDL_GetKeyFromName) 0;
}

#endif

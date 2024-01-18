#ifndef SDL_RENDER_MOCK_H
#define SDL_RENDER_MOCK_H

#include <bdd-for-c-mocks.h>
#include "../../../src/packages/common/IZ_common.h"

typedef struct SDL_Renderer SDL_Renderer;

typedef struct SDL_Window SDL_Window;

typedef struct SDL_FRect SDL_FRect;

mock(SDL_CreateWindow) SDL_Window* SDL_CreateWindow(
	const char *title,
	i32 x, i32 y, i32 w,
	i32 h, u32 flags
) {
	static SDL_Window* window = (SDL_Window*) 1;
	mock_return(SDL_CreateWindow) window;
}

mock(SDL_CreateRenderer) SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, i32 index, u32 flags) {
	static SDL_Renderer* renderer = (SDL_Renderer*) 1;
	mock_return(SDL_CreateRenderer) renderer;
}

mock(SDL_SetRenderDrawColor) i32 SDL_SetRenderDrawColor(
	SDL_Renderer* renderer,
	u8 r, u8 g, u8 b,
	u8 a
) {
	mock_return(SDL_SetRenderDrawColor) 0;
}

mock(SDL_RenderFillRectF) i32 SDL_RenderFillRectF(
	SDL_Renderer* renderer,
	const SDL_FRect* rect
) {
	mock_return(SDL_RenderFillRectF) 0;
}

mock(SDL_RenderClear) i32 SDL_RenderClear(SDL_Renderer* renderer) {
	mock_return(SDL_RenderClear) 0;
}

mock(SDL_RenderPresent) void SDL_RenderPresent(SDL_Renderer* renderer) {
	mock_return(SDL_RenderPresent);
}

mock(SDL_DestroyWindow) void SDL_DestroyWindow(SDL_Window* window) {
	mock_return(SDL_DestroyWindow);
}

#endif

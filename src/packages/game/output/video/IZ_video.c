#include "IZ_video.h"

bool IZ_VideoIsValidWidth(u16 width) {
	// TODO check screen size
	return (320 <= width && width <= 16384);
}

bool IZ_VideoIsValidHeight(u16 height) {
	// TODO check screen size
	return (240 <= height && height <= 8192);
}

bool IZ_VideoIsValidMaxFPS(u8 max_fps) {
	return (10 <= max_fps && max_fps <= 200);
}

static INI_ConfigItem video_config_items[] = {
	{
		INI_CONFIG_TYPE_U16,
		"Video",
		"Width",
		NULL,
		&IZ_VIDEO_DEFAULT_STATE.config.width,
		IZ_VideoIsValidWidth,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	},
	{
		INI_CONFIG_TYPE_U16,
		"Video",
		"Height",
		NULL,
		&IZ_VIDEO_DEFAULT_STATE.config.height,
		IZ_VideoIsValidHeight,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	},
	{
		INI_CONFIG_TYPE_U8,
		"Video",
		"MaxFps",
		"-f",
		&IZ_VIDEO_DEFAULT_STATE.config.max_fps,
		IZ_VideoIsValidMaxFPS,
		INI_CONFIG_TRANSFORMER_NONE,
		NULL,
	},
	INI_CONFIG_ITEM_NULL,
};

void IZ_VideoBindStateToConfig(IZ_VideoState* state, INI_ConfigItem config_items[]) {
	config_items[0].dest = &state->config.width;
	config_items[1].dest = &state->config.height;
	config_items[2].dest = &state->config.max_fps;
}

IZ_ProcedureResult IZ_VideoSaveConfig(IZ_VideoState* state, const char* config_path) {
	IZ_VideoBindStateToConfig(state, video_config_items);
	return INI_ConfigSave(video_config_items, config_path);
}

IZ_ProcedureResult IZ_VideoInitializeConfig(IZ_VideoState* state,  const char* config_path, u8 argc, const char* argv[]) {
	IZ_VideoBindStateToConfig(state, video_config_items);
	if (INI_ConfigInitialize(video_config_items, config_path, argc, argv) < 0) {
		return -1;
	}
	return 0;
}

IZ_ProcedureResult IZ_VideoInitialize(IZ_VideoState* state, void* user_data, const char* config_path, u8 argc, const char* argv[]) {
	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "video", "Setting up...");
	IZ_memcpy(state, sizeof(IZ_VideoState), &IZ_VIDEO_DEFAULT_STATE, sizeof(IZ_VideoState));
	IZ_memset(state->active_sprites, 0, sizeof(IZ_SpriteSlot) * IZ_MAX_ACTIVE_SPRITES);
	if (IZ_VideoInitializeConfig(state, config_path, argc, argv) < 0) {
		return -2;
	}

	state->last_update_at = 0u;
	state->user_data = user_data;

	SDL_Window* window = SDL_CreateWindow(
		IZ_APP_NAME,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		state->config.width,
		state->config.height,
		SDL_WINDOW_SHOWN
	);
	if (window == NULL) {
		IZ_LogError("video", "Window could not be created! Reason: %s", SDL_GetError());
		return -3;
	}
    SDL_ShowWindow(window);
	state->window = window;
	state->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	return 0;
}

void IZ_VideoTeardown(IZ_VideoState* state) {
	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "video", "Shutting down...");
	for (u16 i = 0; i < IZ_MAX_ACTIVE_SPRITES; i += 1) {
		if (state->active_sprites[i].sprite.texture) {
			SDL_DestroyTexture(state->active_sprites[i].sprite.texture);
			state->active_sprites[i].sprite.texture = NULL;
			state->active_sprites[i].requested_at = 0;
		}
	}

	SDL_DestroyWindow(state->window);
}

// we can use IZ_Pool for memory management. Do we want to use a single pool or multiple ones?
u16 IZ_VideoGetNextFreeSpriteSlot(IZ_VideoState* state, IZ_VideoSpritePriority priority) {
	// TODO:
	//   1. Run through all sprites in the active sprites array
	//   2. Check each sprite's priority and requested_at (for eviction policy)
	//   3. Return that new slot. (prefer returning empty slots)
	//   4. Return MAX_ACTIVE_SPRITES if there's no slot left)
	for (u16 i = 0; i < IZ_MAX_ACTIVE_SPRITES; i += 1) {
		if (!state->active_sprites[i].sprite.texture) {
			return i;
		}
	}

	return IZ_MAX_ACTIVE_SPRITES;
}

void IZ_VideoLoadSprite(IZ_VideoState* state, IZ_VideoLoadSpriteParams params, IZ_SpriteSlot* out) {
	char full_path[2048];
	sprintf(full_path, "%s/%s", params.dir, params.filename);
	FILE* f;
	if (IZ_fopen(&f, full_path, "r")) {
		return;
	}
	u32 sprite_length_bytes = ini_getl(params.dir, params.filename, 0, "assets.ini");
	u8* sprite = malloc(sprite_length_bytes + 1);
	fread(sprite, 1, sprite_length_bytes, f);
	SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_BLEND);
	SDL_Surface* test_surface = IMG_LoadSVG_RW(SDL_RWFromConstMem(sprite, sprite_length_bytes));
	free(sprite);
	if (test_surface) {
		out->sprite.texture = SDL_CreateTextureFromSurface(state->renderer, test_surface);
		out->sprite.original_width = test_surface->w;
		out->sprite.original_height = test_surface->h;
		out->sprite.scale_factor = 1;
		out->sprite.rotate_degrees = 0;
		out->sprite.position = (IZ_Vector2D) { 0, 0 };
		out->sprite.flip_x = false;
		out->sprite.flip_y = false;
		SDL_FreeSurface(test_surface);
	}
}

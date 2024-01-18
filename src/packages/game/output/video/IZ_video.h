#ifndef IZ_VIDEO_H
#define IZ_VIDEO_H

#include <stdio.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include "ini-config.h"

#include "../../../log/IZ_log.h"
#include "../../../net/IZ_net_client.h"
#include "../../../common/IZ_common.h"
#include "../../../io/IZ_io.h"
#include "../../../stdinc/IZ_string.h"
#include "../../core/IZ_sprite.h"
#include "../../input/IZ_input.h"

#define IZ_MAX_ACTIVE_SPRITES 512u

typedef enum {
	// eyecandy, e.g. sparks
	IZ_VIDEO_SPRITE_PRIORITY_LOWEST,
	// bottom backgrounds
	IZ_VIDEO_SPRITE_PRIORITY_LOWER,
	// top backgrounds
	IZ_VIDEO_SPRITE_PRIORITY_LOW,
	// projectiles
	IZ_VIDEO_SPRITE_PRIORITY_MEDIUM,
	// foreground objects
	IZ_VIDEO_SPRITE_PRIORITY_HIGH,
	// pickups
	IZ_VIDEO_SPRITE_PRIORITY_HIGHER,
	// player, enemies, weapons
	IZ_VIDEO_SPRITE_PRIORITY_HIGHEST,
} IZ_VideoSpritePriority;

typedef struct {
	const char* dir;
	const char* filename;
	IZ_VideoSpritePriority priority;
} IZ_VideoLoadSpriteParams;

typedef struct {
	u16 width;
	u16 height;
	u8 max_fps;
} IZ_VideoConfig;

typedef struct {
	IZ_Sprite sprite;
	u64 requested_at;
	IZ_VideoSpritePriority priority;
} IZ_SpriteSlot;

typedef struct {
	void* user_data;
	IZ_VideoConfig config;
	u64 last_update_at;
	SDL_Window* window;
	SDL_Renderer* renderer;
	IZ_SpriteSlot active_sprites[IZ_MAX_ACTIVE_SPRITES];
} IZ_VideoState;

static const IZ_VideoState IZ_VIDEO_DEFAULT_STATE = {
	.user_data = NULL,
	.config = {
		.width = 320u,
		.height = 240u,
		.max_fps = 30u,
	},
	.last_update_at = 0,
	.renderer = NULL,
	.window = NULL,
	.active_sprites = {},
};

IZ_ProcedureResult IZ_VideoInitialize(IZ_VideoState*, void*, const char*, u8, const char*[]);

IZ_ProcedureResult IZ_VideoSaveConfig(IZ_VideoState*, const char*);

u16 IZ_VideoGetNextFreeSpriteSlot(IZ_VideoState*, IZ_VideoSpritePriority);

void IZ_VideoLoadSprite(IZ_VideoState*, IZ_VideoLoadSpriteParams, IZ_SpriteSlot*);

void IZ_VideoTeardown(IZ_VideoState*);

#endif

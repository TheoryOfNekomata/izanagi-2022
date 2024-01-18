#include "IZ_app_video.h"

void IZ_VideoUpdateForDebugTicks(IZ_VideoState* video_state, uint64_t ticks) {
	SDL_SetRenderDrawColor(video_state->renderer, 0x00, 0xff, 0xff, 0xff);
	u64 the_ticks = ticks;
	u8 column;
	u8 row;
	const u8 size = 4;

	u8 bit_index;
	for (bit_index = 0; bit_index < 64; bit_index += 1) {
		column = bit_index % 32;
		row = bit_index / 32;

		if (the_ticks & 0x1) {
			SDL_RenderFillRectF(video_state->renderer, &(SDL_FRect) {
				(f32) (video_state->config.width - ((column + 1) * size)),
				(f32) (video_state->config.height - ((row + 1) * size)),
				size,
				size
			});
		}
		the_ticks >>= 1;
	}
}

void IZ_VideoUpdateForDebugInput(IZ_VideoState* video_state, IZ_InputState* input_state) {
	SDL_SetRenderDrawColor(video_state->renderer, 0xff, 0xff, 0x00, 0xff);
	const u8 size = 4;

	u8 column;
	u8 row;

	u8 player_index;
	u8 control_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		IZ_Action the_action = input_state->action[player_index];
		for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
			column = (control_index % 4) + (player_index * 4);
			row = control_index / 4;

			if (the_action & 0x1) {
				SDL_RenderFillRectF(video_state->renderer, &(SDL_FRect) {
					(f32) (column * size),
					(f32) (row * size),
					size,
					size
				});
			}
			the_action >>= 1;
		}
	}
}

void IZ_VideoUpdateForDebugNet(IZ_VideoState* video_state, IZ_NetClientState* net_state) {
	const u8 size = 4;

	switch (net_state->status) {
		default:
			return;
		case IZ_NET_CLIENT_STATUS_ERROR:
			SDL_SetRenderDrawColor(video_state->renderer, 0xff, 0x00, 0x00, 0xff);
			break;
		case IZ_NET_CLIENT_STATUS_CONNECTING:
			SDL_SetRenderDrawColor(video_state->renderer, 0xff, 0xff, 0x00, 0xff);
			break;
		case IZ_NET_CLIENT_STATUS_CONNECTED:
			SDL_SetRenderDrawColor(video_state->renderer, 0x00, 0xff, 0x00, 0xff);
			break;
	}

	SDL_RenderFillRectF(video_state->renderer, &(SDL_FRect) {
		0,
		(f32) (video_state->config.height - size),
		size,
		size,
	});

	if (!net_state->binding.connection) {
		return;
	}

	u8 column;
	u8 row;

	u8 player_index;
	u8 control_index;
	for (player_index = 0; player_index < IZ_PLAYERS; player_index += 1) {
		IZ_Action the_action = net_state->action[player_index];
		for (control_index = 0; control_index < IZ_CONTROLS; control_index += 1) {
			column = (control_index % 4) + (player_index * 4);
			row = control_index / 4;

			if (the_action & 0x1) {
				SDL_RenderFillRectF(video_state->renderer, &(SDL_FRect) {
					(f32) (column * size),
					(f32) ((row * size) + (video_state->config.height - (size * 5))),
					size,
					size
				});
			}
			the_action >>= 1;
		}
	}
}

void IZ_VideoUpdate(IZ_VideoState* video_state) {
	struct IZ_App* app = video_state->user_data;
	u64 ticks = IZ_AppGetTicks(app);
	IZ_InputState* input_state = IZ_AppGetInputState(app);
	IZ_NetClientState* net_state = IZ_AppGetNetState(app);

	if (ticks - video_state->last_update_at > 1000 / video_state->config.max_fps) {
		// Update window
		SDL_SetRenderDrawColor(video_state->renderer, 0x00, 0x00, 0x00, 0xff);
		SDL_RenderClear(video_state->renderer);
		u16 sprite_index;
		// TODO: optimize this, cut the loop as much as possible.
		for (sprite_index = 0; sprite_index < IZ_MAX_ACTIVE_SPRITES; sprite_index += 1) {
			if (!video_state->active_sprites[sprite_index].sprite.texture) {
				continue;
			}
			IZ_Sprite* sprite = &video_state->active_sprites[sprite_index].sprite;
			f32 draw_width = sprite->original_width * sprite->scale_factor;
			f32 draw_height = sprite->original_height * sprite->scale_factor;
			u8 flip_flag = 0;
			if (sprite->flip_x) {
				flip_flag |= SDL_FLIP_HORIZONTAL;
			}
			if (sprite->flip_y) {
				flip_flag |= SDL_FLIP_VERTICAL;
			}
			SDL_RenderCopyExF(video_state->renderer, sprite->texture, NULL, &(SDL_FRect) {
				// TODO honor each sprite's location in the world for calculation in the screen.
				.x = sprite->position.x,
				.y = sprite->position.y,
				.w = draw_width,
				.h = draw_height,
			}, sprite->rotate_degrees++, &(SDL_FPoint) {
				.x = draw_width / 2,
				.y = draw_height / 2,
			}, flip_flag);

			// our goal is to render the svg files and apply custom transforms to some SVG groups if ever.
			// TODO perhaps we can parse the SVG for easier transforms?
		}

		IZ_VideoUpdateForDebugTicks(video_state, ticks);
		IZ_VideoUpdateForDebugInput(video_state, input_state);
		IZ_VideoUpdateForDebugNet(video_state, net_state);
		SDL_RenderPresent(video_state->renderer);
		video_state->last_update_at = ticks;
	}
}

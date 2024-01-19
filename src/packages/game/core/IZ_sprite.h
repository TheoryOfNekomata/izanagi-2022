#ifndef IZ_SPRITE_H
#define IZ_SPRITE_H

#include "SDL_image.h"
#include "../geometry/IZ_vector2d.h"

typedef struct {
	SDL_Texture* texture;
	IZ_Vector2D position;
	f32 original_width;
	f32 original_height;
	f32 scale_factor;
	f32 rotate_degrees;
	bool flip_x;
	bool flip_y;
} IZ_Sprite;

#endif //IZ_SPRITE_H

#ifndef IZ_RECT_H
#define IZ_RECT_H

#include <stdbool.h>
#include "IZ_vector2d.h"

typedef struct {
	f32 left;
	f32 top;
	f32 right;
	f32 bottom;
} IZ_Bounds;

typedef struct {
	// top left
	IZ_Vector2D pos;
	f32 width;
	f32 height;
} IZ_Rect;

IZ_Bounds IZ_RectGetBounds(IZ_Rect);

bool IZ_RectBoundsContainPoint(IZ_Bounds, IZ_Vector2D);

bool IZ_RectBoundsCollide(IZ_Bounds, IZ_Bounds);

#endif

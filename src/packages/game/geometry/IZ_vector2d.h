#ifndef IZ_VECTOR2D_H
#define IZ_VECTOR2D_H

#include "../../common/IZ_common.h"

typedef struct {
	f32 x;
	f32 y;
} IZ_Vector2D;

IZ_Vector2D IZ_Vector2DAdd(IZ_Vector2D, IZ_Vector2D);

IZ_Vector2D IZ_Vector2DMultiply(IZ_Vector2D, IZ_Vector2D);

IZ_Vector2D IZ_Vector2DScale(IZ_Vector2D, f32);

#endif

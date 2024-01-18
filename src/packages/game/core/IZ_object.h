#ifndef IZ_OBJECT_H
#define IZ_OBJECT_H

#include <string.h>
#include "../geometry/IZ_vector2d.h"
#include "../geometry/IZ_rect.h"
#include "IZ_entity.h"
#include "IZ_sprite.h"

typedef struct {
	IZ_Entity as_entity;

	IZ_Rect collision_rect;

	IZ_Sprite* sprite;
} IZ_Object;

IZ_Object* IZ_ObjectNew(IZ_Object);

#endif

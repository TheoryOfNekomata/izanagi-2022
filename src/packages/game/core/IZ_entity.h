#ifndef IZ_ENTITY_H
#define IZ_ENTITY_H

#include <stdlib.h>
#include <string.h>
#include "../geometry/IZ_vector2d.h"

typedef struct {
	IZ_Vector2D pos;

	IZ_Vector2D speed;
} IZ_Entity;

IZ_Entity* IZ_EntityNew(IZ_Entity);

#endif

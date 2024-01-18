#ifndef IZ_CREATURE_H
#define IZ_CREATURE_H

#include "../../common/IZ_common.h"
#include "IZ_object.h"

typedef struct {
	IZ_Object as_object;

	u16 hp;
} IZ_Creature;

#endif

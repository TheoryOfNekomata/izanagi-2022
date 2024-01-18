#include "IZ_entity.h"

IZ_Entity* IZ_EntityNew(IZ_Entity attrs) {
	// TODO let pool allocate memory for entity instead of malloc!
	IZ_Entity* new_entity = (IZ_Entity*) malloc(sizeof(IZ_Entity));

	if (new_entity) {
		memcpy(new_entity, &attrs, sizeof(IZ_Entity));
	}

	return new_entity;
}

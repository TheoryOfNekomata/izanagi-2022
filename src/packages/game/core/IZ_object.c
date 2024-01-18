#include "IZ_object.h"

IZ_Object* IZ_ObjectNew(IZ_Object attrs) {
	// TODO let pool allocate memory for entity instead of malloc!
	IZ_Object* new_object = (IZ_Object*) malloc(sizeof(IZ_Object));

	if (new_object) {
		memcpy(new_object, &attrs, sizeof(IZ_Object));
	}

	return new_object;
}

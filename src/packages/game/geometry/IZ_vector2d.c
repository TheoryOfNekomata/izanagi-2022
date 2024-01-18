#include "IZ_vector2d.h"

IZ_Vector2D IZ_Vector2DAdd(IZ_Vector2D addend, IZ_Vector2D augend) {
	return (IZ_Vector2D) {
		.x = addend.x + augend.x,
		.y = addend.y + augend.y,
	};
}

IZ_Vector2D IZ_Vector2DMultiply(IZ_Vector2D multiplicand, IZ_Vector2D multiplier) {
	return (IZ_Vector2D) {
		.x = multiplicand.x * multiplier.x,
		.y = multiplicand.y * multiplier.y,
	};
}

IZ_Vector2D IZ_Vector2DScale(IZ_Vector2D vector, f32 scalar) {
	return (IZ_Vector2D) {
		.x = vector.x * scalar,
		.y = vector.y * scalar,
	};
}

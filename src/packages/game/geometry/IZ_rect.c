#include "IZ_rect.h"

IZ_Bounds IZ_RectGetBounds(IZ_Rect rect) {
	return (IZ_Bounds) {
		.left = rect.pos.x,
		.top = rect.pos.y,
		.right = rect.pos.x + rect.width,
		.bottom = rect.pos.y + rect.height,
	};
}

bool IZ_RectBoundsContainPoint(IZ_Bounds bounds, IZ_Vector2D point) {
	return (
		bounds.left <= point.x
		&& bounds.top <= point.y
		&& point.x <= bounds.right
		&& point.y <= bounds.bottom
	);
}

bool IZ_RectBoundsCollide(IZ_Bounds a, IZ_Bounds b) {
	return (
		b.left < a.right
		&& b.top < a.bottom
	);
}

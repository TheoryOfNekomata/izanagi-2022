#include <bdd-for-c.h>
#include <game/geometry/IZ_vector2d.h>
#include <game/geometry/IZ_rect.h>

spec("geometry") {
	describe("vector2d") {
		describe("Add") {
			it("adds two vectors") {
				static IZ_Vector2D addend = {
					.x = 420.f,
					.y = 1337.f,
				};

				static IZ_Vector2D augend = {
					.x = 6.f,
					.y = 9.f,
				};

				static IZ_Vector2D expected_sum = {
					.x = 426.f,
					.y = 1346.f,
				};

				static IZ_Vector2D actual_sum;
				actual_sum = IZ_Vector2DAdd(addend, augend);

				check(expected_sum.x == actual_sum.x, "Right values do not match.");
				check(expected_sum.y == actual_sum.y, "Up values do not match.");
			}
		}

		describe("Multiply") {
			it("multiplies two vectors") {
				static IZ_Vector2D multiplicand = {
					.x = 6.f,
					.y = 9.f,
				};

				static IZ_Vector2D multiplier = {
					.x = 3.f,
					.y = 2.f,
				};

				static IZ_Vector2D expected_product = {
					.x = 18.f,
					.y = 18.f,
				};

				static IZ_Vector2D actual_product;
				actual_product = IZ_Vector2DMultiply(multiplicand, multiplier);

				check(expected_product.x == actual_product.x, "Right values do not match.");
				check(expected_product.y == actual_product.y, "Up values do not match.");
			}
		}

		describe("Scale") {
			it("scales a vector") {
				static IZ_Vector2D v = {
					.x = 420.f,
					.y = 69.f,
				};

				static f32 s = 2.f;

				static IZ_Vector2D expected = {
					.x = 840.f,
					.y = 138.f,
				};

				static IZ_Vector2D actual;
				actual = IZ_Vector2DScale(v, s);

				check(expected.x == actual.x, "Right values do not match.");
				check(expected.y == actual.y, "Up values do not match.");
			}
		}
	}

	describe("rect") {
		describe("GetBounds") {
			it("returns the bounds of a rectangle") {
				static IZ_Rect r = {
					.pos = {
						.x = 50,
						.y = 100,
					},
					.width = 150,
					.height = 200,
				};

				static IZ_Bounds expected = {
					.left = 50,
					.top = 100,
					.right = 200,
					.bottom = 300,
				};

				static IZ_Bounds actual;
				actual = IZ_RectGetBounds(r);

				check(expected.left == actual.left, "Left values do not match.");
				check(expected.top == actual.top, "Top values do not match.");
				check(expected.right == actual.right, "Right values do not match.");
				check(expected.bottom == actual.bottom, "Bottom values do not match.");
			}
		}

		describe("BoundsContainPoint") {
			it("returns true for points inside bounds") {
				static IZ_Bounds b = {
					.left = 50,
					.top = 100,
					.right = 200,
					.bottom = 300,
				};

				static IZ_Vector2D p = {
					.x = 75,
					.y = 150,
				};

				check(IZ_RectBoundsContainPoint(b, p), "Point not found inside bounds.");
			}

			it("returns true for points along bounds edge") {
				static IZ_Bounds b = {
					.left = 50,
					.top = 100,
					.right = 200,
					.bottom = 300,
				};

				static IZ_Vector2D p1 = {
					.x = 50,
					.y = 100,
				};

				static IZ_Vector2D p2 = {
					.x = 200,
					.y = 300,
				};

				check(IZ_RectBoundsContainPoint(b, p1), "Point p1 not found inside bounds.");
				check(IZ_RectBoundsContainPoint(b, p2), "Point p2 not found inside bounds.");
			}

			it("returns false for points outside bounds") {
				static IZ_Bounds b = {
					.left = 50,
					.top = 100,
					.right = 200,
					.bottom = 300,
				};

				static IZ_Vector2D p = {
					.x = 0,
					.y = 0,
				};

				check(!IZ_RectBoundsContainPoint(b, p), "Point found inside bounds.");
			}
		}

		describe("BoundsCollide") {
			it("returns true for bounds A inside bounds B") {
				static IZ_Bounds a = {
					.left = 100,
					.top = 125,
					.right = 150,
					.bottom = 200,
				};

				static IZ_Bounds b = {
					.left = 50,
					.top = 75,
					.right = 200,
					.bottom = 250,
				};

				check(IZ_RectBoundsCollide(a, b), "Bounds not colliding.");
			}

			it("returns true for bounds A intersecting bounds B") {
				static IZ_Bounds a = {
					.left = 0,
					.top = 0,
					.right = 150,
					.bottom = 200,
				};

				static IZ_Bounds b = {
					.left = 50,
					.top = 75,
					.right = 200,
					.bottom = 250,
				};

				check(IZ_RectBoundsCollide(a, b), "Bounds not colliding.");
			}

			it("returns true for bounds A outside bounds B") {
				static IZ_Bounds a = {
					.left = 50,
					.top = 75,
					.right = 200,
					.bottom = 250,
				};

				static IZ_Bounds b = {
					.left = 550,
					.top = 575,
					.right = 800,
					.bottom = 850,
				};

				check(!IZ_RectBoundsCollide(a, b), "Bounds colliding.");
			}
		}
	}
}

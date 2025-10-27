#include "Vec2.h"

Vec2::Vec2(float x, float y) {
	this->x = x;
	this->y = y;
}

void Vec2::Addvf2(const Vec2& v) {
	x += v.x;
	y += v.y;
}

Vec2 Vec2::Multiplyf(float f) {
	return Vec2(x * f, y * f);
}
#pragma once
class Vec2
{
public:
	float x, y;
	Vec2() {};
	Vec2(float x, float y);
	void Addvf2(const Vec2& v);
	Vec2 Multiplyf(float f);
};


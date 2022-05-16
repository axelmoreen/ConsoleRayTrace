#include "VecMath.h"
#include <cmath>
using namespace std;
// start vec3
void Vec3::Add(Vec3 b)
{
	x = x + b.x;
	y = y + b.y;
	z = z + b.z;
}

void Vec3::Subtract(Vec3 b)
{
	x = x - b.x;
	y = y - b.y;
	z = z - b.z;
}

void Vec3::ScalarMultiply(float s)
{
	x = x * s;
	y = y * s;
	z = z * s;
}

float Vec3::Mag()
{
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

Vec3 Vec3::Normalize()
{
	float mag = this->Mag();
	return Vec3(x / mag, y / mag, z / mag);
}

void Vec3::Abs()
{
	x = abs(x);
	y = abs(y);
	z = abs(z);
}
float Vec3::Dot(Vec3 b)
{
	return x * b.x + y * b.y + z * b.z;
}



//start vec2
void Vec2::Add(Vec2 b)
{
	x = x + b.x;
	y = y + b.y;
}

void Vec2::Subtract(Vec2 b)
{
	x = x - b.x;
	y = y - b.y;
}

void Vec2::ScalarMultiply(float s)
{
	x = x * s;
	y = y * s;
}

float Vec2::Mag()
{
	return sqrt(pow(x, 2) + pow(y, 2));
}

Vec2 Vec2::Normalize()
{
	float mag = this->Mag();
	return Vec2(x / mag, y / mag);
}

void Vec2::Abs()
{
	x = abs(x);
	y = abs(y);
}
float Vec2::Dot(Vec2 b)
{
	return x * b.x + y * b.y;
}

Vec2 Vec2::operator+(const Vec2& other)
{
	Vec2 copy = Vec2(x, y);
	copy.Add(other);
	return copy;
}

Vec2 Vec2::operator-(const Vec2& other)
{
	Vec2 copy = Vec2(x, y);
	copy.Subtract(other);
	return copy;
}
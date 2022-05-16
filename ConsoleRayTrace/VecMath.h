#ifndef VecMath_H
#define VecMath_H
#include <math.h>
#include <ostream>

class Vec3
{
public:
	Vec3() : x(0), y(0), z(0) {};
	Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {};

	void Add(Vec3 b);

	void Subtract(Vec3 b);

	void ScalarMultiply(float s);

	float Mag();

	Vec3 Normalize();

	float Dot(Vec3 b);

	void Abs();

	friend Vec3 operator+(const Vec3& A, const Vec3& B) {
		return Vec3{ A.x + B.x, A.y + B.y, A.z + B.z };
	}
	friend Vec3 operator-(const Vec3& A, const Vec3& B) {
		return Vec3{ A.x - B.x, A.y - B.y, A.z - B.z };
	}
	

	float x;
	float y;
	float z;
};

class Vec2
{
public:
	Vec2() :x(0), y(0) {};
	Vec2(float x_, float y_) : x(x_), y(y_) {};

	void Add(Vec2 b);

	void Subtract(Vec2 b);

	void ScalarMultiply(float s);

	float Mag();

	Vec2 Normalize();

	float Dot(Vec2 b);

	void Abs();

	Vec2 operator+(const Vec2& other);
	Vec2 operator-(const Vec2& other);

	float x;
	float y;
};

#endif
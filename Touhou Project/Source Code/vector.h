#pragma once


/* Notes
3D Vector
contatins x , y , z and w
ignore w for now

Operator works the same way as mathamatics
by default its all set 0 when constructed, you may pass in diffrently

it is possible to use glVertex3fv(Vector());
as it can converts itself into vertex array..

Able to normalize, dot product and cross product
*/
#include <math.h>

#define PIE 3.14159265f

inline float fastSqrt_Bab(const float x)  
{
	union
	{
		int i;
		float x;
	} u;
	u.x = x;
	u.i = (1<<29) + (u.i >> 1) - (1<<22); 

	// One Babylonian Step
	u.x = 0.5f * (u.x + x/u.x);

	return u.x;
}

class Vector
{
public:
	//__declspec(align(16))
		float x;
		float y, z, w;
	Vector()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
		, w(0.0f)
	{
	}
	Vector(float _x, float _y, float _z, float _w = 1.0f)
		: x(_x)
		, y(_y)
		, z(_z)
		, w(_w)
	{
	}
	Vector(float *p)
		: x(p[0])
		, y(p[1])
		, z(p[2])
		, w(p[3])
	{
	}
	Vector(const Vector &v)
	{
		try{*this = v;}
		catch(...){*this = Vector();}
	}
	bool operator ==(const Vector &v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}
	bool operator !=(const Vector &v) const
	{
		return x != v.x || y != v.y || z != v.z;
	}
	Vector operator +() const
	{
		return Vector(x, y, z, w);
	}
	Vector operator +(const Vector& v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z, 0.0f);
	}
	Vector operator -() const
	{
		return Vector(-x, -y, -z, w);
	}
	Vector operator -(const Vector& v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z, 0.0f);
	}
	Vector operator *(float f) const
	{
		return Vector(x * f, y * f, z * f, w);
	}
	Vector operator /(float f) const
	{
		return Vector(x / f, y / f, z / f, w);
	}
	Vector operator^(Vector &v)
	{
		return Vector(y * v.z - z * v.y, -x * v.z - z * v.x, x * v.y - y * v.x);
	}
	Vector operator^=(Vector &v)
	{
		x = y * v.z - z * v.y;
		y = -x * v.z - z * v.x;
		z = x * v.y - y * v.x;
		return *this;
	}
	Vector &operator =(const Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}
	Vector &operator -=(const Vector &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector &operator +=(const Vector &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector &operator *=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}
	Vector &operator /=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}
	operator float *() const
	{
		return (float *)&x;
	}
	operator const float *() const
	{
		return &x;
	}
	float Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	Vector Normalize() const
	{
		float f = x * x + y * y + z * z;
		if (f)
		{
			f = 1.0f / fastSqrt_Bab(f);
			return Vector(x * f, y * f, z * f, w);
		}
		return Vector(0.0f, 0.0f, 0.0f, 0.0f);
	}
	float Dot(const Vector &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}
	Vector Cross(const Vector &v) const
	{
		return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x, 0.0f);
	}
	Vector RotateFromPoint(const Vector &v, float Angle)
	{
		Vector nv = Vector(*this - v);

		return (Vector(	nv.y * sin(Angle) + nv.x * cos(Angle), 
						nv.y * cos(Angle) - nv.x * sin(Angle), 
						nv.z) + v);
	}
	float AngleBewteenVector(const Vector &v)
	{
		float Angle = (atan2(x, y) - atan2(v.x, v.y));
		if(Angle < 0)
			Angle += 2.0f * PIE;
		return Angle;
	}
	static void Register(lua_State * State);
};

class Vector2D
{
public:
	//__declspec(align(16))
		float x;
		float y;
	Vector2D()
		: x(0.0f)
		, y(0.0f)
	{
	}
	Vector2D(float _x, float _y)
		: x(_x)
		, y(_y)
	{
	}
	Vector2D(float *p)
		: x(p[0])
		, y(p[1])
	{
	}
	Vector2D(const Vector2D &v)
		: x(v.x)
		, y(v.y)
	{
	}
	bool operator ==(const Vector2D &v) const
	{
		return x == v.x && y == v.y;
	}
	bool operator !=(const Vector2D &v) const
	{
		return x != v.x || y != v.y;
	}
	Vector2D operator +() const
	{
		return Vector2D(x, y);
	}
	Vector2D operator +(const Vector2D& v) const
	{
		return Vector2D(x + v.x, y + v.y);
	}
	Vector2D operator -() const
	{
		return Vector2D(-x, -y);
	}
	Vector2D operator -(const Vector2D& v) const
	{
		return Vector2D(x - v.x, y - v.y);
	}
	Vector2D operator *(float f) const
	{
		return Vector2D(x * f, y * f);
	}
	Vector2D operator /(float f) const
	{
		return Vector2D(x / f, y / f);
	}
	Vector2D &operator =(const Vector2D &v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}
	Vector2D &operator -=(const Vector2D &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector2D &operator +=(const Vector2D &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector2D &operator *=(float f)
	{
		x *= f;
		y *= f;
		return *this;
	}
	Vector2D &operator /=(float f)
	{
		x /= f;
		y /= f;
		return *this;
	}

	float Length() const
	{
		return sqrtf(x * x + y * y);
	}
	Vector2D Normalize() const
	{
		float f = x * x + y * y;
		if (f)
		{
			f = 1.0f / sqrtf(f);
			return Vector2D(x * f, y * f);
		}
		return Vector2D(0.0f, 0.0f);
	}
	Vector2D RotateFromPoint(const Vector2D &v, float Angle)
	{
		Vector2D nv = Vector2D(*this - v);

		return (Vector2D(	nv.y * sin(Angle) + nv.x * cos(Angle), 
						nv.y * cos(Angle) - nv.x * sin(Angle)));
	}
	float AngleBewteenVector(const Vector2D &v)
	{
		float Angle = (atan2(x, y) - atan2(v.x, v.y));
		if(Angle < 0)
			Angle += 2.0f * PIE;
		return Angle;
	}
};

Vector operator *(float f, const Vector &v);
Vector2D operator *(float f, const Vector2D &v);


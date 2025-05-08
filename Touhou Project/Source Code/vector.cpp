#include "stdafx.h"
#include "vector.h"

Vector operator *(float f, const Vector &v)
{
	return Vector(v.x * f, v.y * f, v.z * f, v.w);
}

Vector2D operator *(float f, const Vector2D &v)
{
	return Vector2D(v.x * f, v.y * f);
}

void Vector :: Register(lua_State * State)
{
	luabind::module(State) 
	[
		luabind::class_<Vector>("Vector")
		.def(luabind::constructor<>())
		.def(luabind::constructor<float, float, float>())
		.def("Length", &Vector::Length)
		.def("Normalize", &Vector::Normalize)
		.def("Dot", &Vector::Dot)
		.def("Cross", &Vector::Cross)
		.def("RotateFromPoint", &Vector::RotateFromPoint)
		.def("AngleBewteenVector", &Vector::AngleBewteenVector)
		.def_readwrite("x", &Vector::x)
		.def_readwrite("y", &Vector::y)
		.def_readwrite("z", &Vector::x)
		.def(luabind::const_self + Vector())
		.def(luabind::const_self - Vector())
		.def(luabind::const_self == Vector())
		.def(luabind::const_self * float())
		.def(luabind::const_self / float())
	];
}

#include "stdafx.h"
#include "wave.h"

BulletWave :: BulletWave()
{

}

BulletWave :: ~BulletWave()
{
	ReleaseAll();
}

void BulletWave :: AddBullet(BulletDef & Link)
{
	Link.m_Control = true;
	List.push_back(new Bullet(Link));
}

void BulletWave :: SetBullet(int index, Bullet Bullet)
{
	*List[index] = Bullet;
}

void BulletWave :: ReleaseAll()
{
	EnterCriticalSection(&_critSection);
	for(vector <Bullet *> :: iterator index = List.begin(); index != List.end(); index++)
	{
		(*index)->Release();
		(*index)->SetMass((*index)->GetHitboxSize());
	}
	LeaveCriticalSection(&_critSection);
	List.clear();
}

void BulletWave :: ThrowInAll()
{
	while(g_InGameStatus == PAUSE)
		Sleep(10);
	if(g_InGameStatus == NONE)
		return;
	EnterCriticalSection(&_critSection);
	for(unsigned int index = 0; index < List.size(); index++)
		List.at(index) = g_Engine->InsertEnemyBullet(List.at(index));
	LeaveCriticalSection(&_critSection);
}

void BulletWave :: Throw(BulletDef & Throw)
{
	Throw.m_Control = true;
	while(g_InGameStatus == PAUSE)
		Sleep(10);
	if(g_InGameStatus == NONE)
		return;
	EnterCriticalSection(&_critSection);
	List.push_back(g_Engine->InsertEnemyBullet(Throw));
	LeaveCriticalSection(&_critSection);
}

int BulletWave :: SizeOfWave()
{
	try
	{
		return static_cast<int>(List.size());
	}
	catch(...)
	{
		return 0;
	}
}

Vector BulletWave :: GetPosition(int index)
{
	return List[index]->GetPosition();
}

Vector BulletWave :: GetVelocity(int index)
{
	return List[index]->GetVelocity();
}

Vector BulletWave :: GetAcceleration(int index)
{
	return List[index]->GetAcceleration();
}

void BulletWave :: SetPosition(int index, Vector Pos)
{
	List[index]->SetPosition(Pos);
}

void BulletWave :: SetVelocity(int index, Vector Vel)
{
	List[index]->SetVelocity(Vel);
}

void BulletWave :: SetAcceleration(int index, Vector Acc)
{
	List[index]->SetAcceleration(Acc);
}

void BulletWave :: SetAngle(int index, float Angle)
{
	List[index]->SetAngle(Angle);
}

void BulletWave :: SetScale(int index, float Scale)
{
	List[index]->SetScale(Scale);
}

void BulletWave :: SetHitBox(int index, float HitBox)
{
	List[index]->SetHitbox(HitBox, false);
}

void BulletWave :: Register(lua_State * State)
{
	luabind::module(State) 
	[
		luabind::class_<BulletWave>("BulletWave")
		.def(luabind::constructor<>())
		.def("AddBullet", &BulletWave::AddBullet)
		.def("ThrowInAll", &BulletWave::ThrowInAll)
		.def("Throw", &BulletWave::Throw)
		.def("ReleaseAll", &BulletWave::ReleaseAll)
		.def("SetBullet", &BulletWave::SetBullet)
		.def("Size", &BulletWave::SizeOfWave)

		.def("GetPosition", &BulletWave::GetPosition)
		.def("GetVelocity", &BulletWave::GetVelocity)
		.def("GetAcceleration", &BulletWave::GetAcceleration)

		.def("SetPosition", &BulletWave::SetPosition)
		.def("SetVelocity", &BulletWave::SetVelocity)
		.def("SetAcceleration", &BulletWave::SetAcceleration)
		.def("SetAngle", &BulletWave::SetAngle)
		.def("SetScale", &BulletWave::SetScale)
		.def("SetHitBox", &BulletWave::SetHitBox)
	];
}
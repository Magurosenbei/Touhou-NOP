#include "stdafx.h"
#include "EnemyScript.h"

EnemyScript::EnemyScript(){
	
}
void EnemyScript::AddEnemy(int id, Vector pos, float size, int hp )
{
	Enemy* temp = new Enemy(pos, Vector(),Vector(), 32, hp, id, size);
	Enemies.push_back(temp);
	//g_Engine->InsertEnemyList(temp);
}
void EnemyScript::AddToEngine(bool putall, unsigned int ID)
{
	//if(putall == true)
	//{
	//	for(unsigned int i = 0; i < Enemies.size();i++)
	//	g_Engine->InsertEnemyList(Enemies[i]);
	//}
	//else
	EnterCriticalSection(&_critSection);
	if(putall == false)
		g_Engine->InsertEnemyList(Enemies[ID]);
	LeaveCriticalSection(&_critSection);
}
void EnemyScript::AddReward(){}
void EnemyScript::AddAccel(Vector velocity	, unsigned int ID)
{
	Enemies[ID]->SetVelocity(velocity);
}
void EnemyScript::ChangeAnimation(int frame, unsigned int ID)
{
	Enemies[ID]->m_iAnimationFrame = frame;
}
void EnemyScript::ChangePosition(unsigned int ID, Vector pos)
{
	Enemies[ID]->SetPosition(pos);
}
bool EnemyScript::isMoving(unsigned int ID)
{
	return Enemies[ID]->GetVelocity() == Vector();
}
bool EnemyScript::AtBorder(unsigned int ID)
{
	Vector DV = Enemies[ID]->GetPosition();
	return (DV.x < SCREEN_BORDER_MAX_X-80 && DV.x > SCREEN_BORDER_MIN_X+80 && DV.y < SCREEN_BORDER_MAX_Y-80 && DV.y > SCREEN_BORDER_MIN_Y+80);
}
bool EnemyScript::isDead(unsigned int ID, int amount)
{
	if(amount < 2)
		return getHp(ID) > 0;
	else
	amount += ID;
	while(ID < (unsigned int)amount)
	{
		if(getHp(ID) > 0)
			return false;
		else
			ID++;
	}
	return true;
}
bool EnemyScript::ComparePosition(Vector temp, Vector temp2, int Position )
{
	if(Position == 1)
	{
		if(temp.x < temp2.x || temp.y < temp2.y)
		return true;
	return false;
	}
	else if(Position == 2)
	{
		if(temp.x > temp2.x || temp.y > temp2.y)
			return true;
		return false;
	}
	else if(Position == 3)
	{
		if(temp.x > temp2.x || temp.y < temp2.y)
			return true;
		return false;
	}
	else
	{
		if(temp.x < temp2.x || temp.y > temp2.y)
			return true;
		return false;
	}
}
bool EnemyScript::CheckStatus(unsigned int ID)
{
	return Enemies[ID]->CanDelete();
}
int EnemyScript::getHp(unsigned int ID)
{
	return Enemies[ID]->GetHp();
}
Vector EnemyScript::getposition(unsigned int ID)
{
	return Enemies[ID]->GetPosition();
}
float EnemyScript::getpercent(unsigned int ID)
{
	return Enemies[ID]->getPercentage();	
}
void EnemyScript::Register(lua_State *State)
{
	luabind::module(State) 
	[
		luabind::class_<EnemyScript>("EnemyScript")
		.def(luabind::constructor<>())
		.def("AddEnemy", &EnemyScript::AddEnemy)
		.def("AddToEngine", &EnemyScript::AddToEngine)
		.def("AddReward", &EnemyScript::AddReward)
		.def("AddAccel", &EnemyScript::AddAccel)
		.def("ChangeAnimation", &EnemyScript::ChangeAnimation)
		.def("ChangePosition", &EnemyScript::ChangePosition)
		.def("isMoving", &EnemyScript::isMoving)
		.def("AtBorder", &EnemyScript::AtBorder)
		.def("getpercent", &EnemyScript::getpercent)
		.def("isDead", &EnemyScript::isDead)
		.def("getHp", &EnemyScript::getHp)
		.def("getposition", &EnemyScript::getposition)
		.def("Release", &EnemyScript::Release)
		.def("getC", &EnemyScript::getC)
		.def("SetHover", &EnemyScript::SetHover)
		.def("ComparePosition", &EnemyScript::ComparePosition)
		.def("SetRotate", &EnemyScript::SetRotate)
		.def("OffRotate", &EnemyScript::OffRotate)
		.def("CheckStatus", &EnemyScript::CheckStatus)
		.def("GoToPosition", &EnemyScript::GoToPosition)
		.def("LifebarSetup", &EnemyScript::LifebarSetup)
		.def("CheckDistance", &EnemyScript::CheckDistance)
		.def("AddToControl", &EnemyScript::AddToControl)
	];
} 
void EnemyScript::Release(unsigned int ID)
{
	Enemies[ID]->Release();
	Enemies[ID] = NULL;
}	
void EnemyScript::getC(int C)
{
	if(C < 70)
	C++;
}

void EnemyScript::SetHover(unsigned int ID)
{
	Enemies[ID]->Sethover();
}
void EnemyScript::SetRotate(Vector pos,float ang,unsigned int ID)
{
	Enemies[ID]->angle = ang;
	Enemies[ID]->rotatepoint = pos;
	Enemies[ID]->SetRotate();
}
void EnemyScript::OffRotate(unsigned int ID)
{
	Enemies[ID]->OffRotate();
}


void EnemyScript::LifebarSetup(unsigned int ID)
{
	gHUD->Getboss(Enemies[ID]);
}
bool EnemyScript::CheckDistance(unsigned int ID)
{
	return Enemies[ID]->Checkdistance();
}
void EnemyScript::GoToPosition(Vector pos, unsigned int ID)
{
	Enemies[ID]->GoToPosition(pos);
}
void EnemyScript::AddToControl(unsigned int FirstID, unsigned int SecID)
{
	Enemies[FirstID]->AddToControl(Enemies[SecID]);
}
EnemyWave :: EnemyWave()
{
}

EnemyWave :: ~EnemyWave()
{
	ReleaseAll();
}

void EnemyWave :: AddEnemy(int id, float TexWidth, float HitBox, int HP, int frame)
{
	List.push_back(new Enemy(Vector(), Vector(), Vector(), HitBox, HP, id, TexWidth));
	(List.back())->m_iAnimationFrame = frame;
}

void EnemyWave :: SetPosition(int index, Vector Pos)
{
	List[index]->SetPosition(Pos);
}

void EnemyWave :: SetVelocity(int index, Vector Vel)
{
	List[index]->SetVelocity(Vel);
}

void EnemyWave :: SetAcceleration(int index, Vector Acc)
{
	List[index]->SetAcceleration(Acc);
}

Vector EnemyWave :: GetPosition(int index)
{
	return List[index]->GetPosition();
}

Vector EnemyWave :: GetVelocity(int index)
{
	return List[index]->GetVelocity();
}

Vector EnemyWave :: GetAcceleration(int index)
{
	return List[index]->GetAcceleration();
}

int EnemyWave :: GetHp(int index)
{
	return List[index]->GetHp();
}

int EnemyWave :: Size()
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
void EnemyWave :: ReleaseAll()
{
	EnterCriticalSection(&_critSection);
	for(unsigned int index = 0; index < List.size(); index++)
		List[index]->Release();
	List.clear();
	LeaveCriticalSection(&_critSection);
}
void EnemyWave :: ThrowInAll()
{
	while(g_InGameStatus == PAUSE)
		Sleep(10);
	if(g_InGameStatus == NONE)
		return;

	EnterCriticalSection(&_critSection);
	for(unsigned int index = 0; index < List.size(); index++)
		g_Engine->InsertEnemyList(List[index]);
	LeaveCriticalSection(&_critSection);
}

void EnemyWave :: Throw(int id, float TexWidth, float HitBox, int HP, int frame)
{
	List.push_back(new Enemy(Vector(), Vector(), Vector(), HitBox, HP, id, TexWidth));
	(List.back())->m_iAnimationFrame = frame;
	EnterCriticalSection(&_critSection);
	g_Engine->InsertEnemyList(*(List.end() - 1));
	LeaveCriticalSection(&_critSection);
}

void EnemyWave :: ThrowFromStore(int index)
{
	EnterCriticalSection(&_critSection);
	g_Engine->InsertEnemyList(List[index]);
	LeaveCriticalSection(&_critSection);
}

void EnemyWave :: Register(lua_State * State)
{
	luabind::module(State) 
	[
		luabind::class_<EnemyWave>("EnemyWave")
		.def(luabind::constructor<>())
		.def("AddEnemy", &EnemyWave::AddEnemy)
		.def("ThrowInAll", &EnemyWave::ThrowInAll)
		.def("Throw", &EnemyWave::Throw)
		.def("ThrowFromList", &EnemyWave::ThrowFromStore)
		.def("ReleaseAll", &EnemyWave::ReleaseAll)
		.def("Size", &EnemyWave::Size)
		
		.def("GetHP", &EnemyWave::GetHp)
		.def("GetPosition", &EnemyWave::GetPosition)
		.def("GetVelocity", &EnemyWave::GetVelocity)
		.def("GetAcceleration", &EnemyWave::GetAcceleration)

		.def("SetPosition", &EnemyWave::SetPosition)
		.def("SetVelocity", &EnemyWave::SetVelocity)
		.def("SetAcceleration", &EnemyWave::SetAcceleration)
	];
}
#include "stdafx.h"
#include "weaponscript.h"
#include "bullet.h"
#include "Player.h"
#include <fstream>

WeaponScript * g_Weapon;

WeaponScript :: WeaponScript()
{
}

WeaponScript :: WeaponScript(string Title)
{
	m_sLabelName = Title;
	m_iTexID = 0;
	m_fTexWidth = 0;
	m_fCurrentDistance = m_fDistanceLowest = m_fDistance = m_fDistanceFactor = m_fFocusRotationAngle = m_fFocusOrbitAngle = m_fCurrentDistance = m_fCurrentAngle = 0.0f;
}

void WeaponScript :: Render()
{
	switch(g_GameManager->GetPowerStage())
	{
	case 0:	LevelRender(LevelBasic_Order);
			break;
	case 1:	LevelRender(LevelOne_Order);
			break;
	case 2:	LevelRender(LevelTwo_Order);
			break;
	case 3:	LevelRender(LevelThree_Order);
			break;
	case 4:	LevelRender(LevelFour_Order);
			break;
	}

	if(g_GameManager->IsBomb())
	{
		glPushMatrix();
//		glTranslatef(P1->GetPosition().x, P1->GetPosition().y,0);
		
		Bt_Def.m_fAlpha = 0.8f;
		for(unsigned int index = 0; index < BombList.size(); index++)
		{
			if(HoldInPlace)
				BombList[index]->SetPosition(P1->GetPosition());				
			else
				BombList[index]->SetPosition(BombList[index]->GetPosition() + BombList[index]->GetVelocity());
			if(PlayerDash)
			{
				if(P1->GetPosition().y < SCREEN_BORDER_MAX_Y-1)
					P1->SetVelocity(DashSpeed);
				else
				{
					g_GameManager->m_iBombTimer = 4;
					g_GameManager->m_iGodTimer = 3;
				}
			}
			BombList[index]->UpdateVectors();
			BombList[index]->Render();
		}
		glPopMatrix();
	}
	else
	{
		for(unsigned int index = 0; index < BombList.size(); index++)
		{
			BombList[index]->SetPosition(P1->GetPosition());
			BombList[index]->UpdateVectors();
		}
	}

}

void WeaponScript :: LevelRender(vector <int> &Level)
{
	//Level;
	glDepthMask(GL_FALSE);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for(unsigned int index = 0; index < Level.size(); index++)
	{
		glPushMatrix();
		glTranslatef(SatellitePosition[Level[index]].x, SatellitePosition[Level[index]].y, SatellitePosition[Level[index]].z);
		glRotatef(m_fCurrentAngle, 0, 0, 1);
		g_TexLoader->DrawXY(m_iTexID, SatelliteFrame[Level[index]], m_fTexWidth);
		glPopMatrix();
	}
	glDepthMask(GL_TRUE);
}

void WeaponScript :: Update(bool FocusMode)
{
	Vector Point = P1->GetPosition();
	float UseAngle;
	(FocusMode)? UseAngle = m_fFocusOrbitAngle : UseAngle = m_fOrbitAngle;		// Switch for Focus / No Focus
	for(unsigned int index = 0; index < SatellitePosition.size(); index++)
	{
		SatellitePosition[index] = SatellitePosition[index].RotateFromPoint(Point, PIE / 180.0f * UseAngle);
		SatellitePosition[index] = (SatellitePosition[index] - Point).Normalize();
		SatellitePosition[index] = SatellitePosition[index] * m_fCurrentDistance + Point;
	}
	(FocusMode)? UseAngle = m_fFocusRotationAngle : UseAngle = m_fRotationAngle;
	(m_fCurrentAngle < 360)? m_fCurrentAngle += UseAngle : m_fCurrentAngle = 0.0f;
	if(FocusMode)
		(m_fCurrentDistance > m_fDistanceLowest)? m_fCurrentDistance -= m_fDistanceFactor : m_fCurrentDistance = m_fDistanceLowest;
	else
		(m_fCurrentDistance < m_fDistance)? m_fCurrentDistance += m_fDistanceFactor : m_fCurrentDistance = m_fDistance;

	if(g_GameManager->IsBomb())
	{
		for(unsigned int index = 0; index < BombList.size(); index++)
		{
			BombList[index]->Update();
			BombList[index]->UpdateVectors();
			g_Engine->CollideEnemy(*BombList[index]);
			g_Engine->CollideEnemyBullet(*BombList[index]);
		}
	}
	else
	{
		for(unsigned int index = 0; index < BombList.size(); index++)
			BombList[index]->m_bRemove = false;
	}

}

void WeaponScript :: RelativePoint(float dx, float dy)
{
	for(unsigned int index = 0; index < SatellitePosition.size(); index++)
	{
		SatellitePosition[index].x += dx;
		SatellitePosition[index].y += dy;
	}
}

void WeaponScript :: Bomb()
{
	//g_GameManager->UseBomb();
}

void WeaponScript :: Shoot(int PowerLevel)
{
	switch(PowerLevel)
	{
	case 0:	ShootLevel(LevelBasicList);
			break;
	case 1: ShootLevel(LevelOneList);
			break;
	case 2:	ShootLevel(LevelTwoList);
			break;
	case 3: ShootLevel(LevelThreeList);
			break;
	case 4: ShootLevel(LevelFourList);
			break;

	}
}

void WeaponScript :: ShootLevel(list <BulletDef> &List)
{
	for(list <BulletDef> :: iterator index = List.begin(); index != List.end(); ++index)
	{
		BulletDef Ins = (*index);

		if(Ins.Interval > 1)
			if(g_TimeManager->GetFPS() % (*index).Interval != 0)
				return;
		if(Ins.Follow == -1)
			Ins.Position += P1->GetPosition();
		else
			Ins.Position = SatellitePosition[Ins.Follow];
		Ins.m_Control = false;
		if(Ins.m_UseRelative)
		{
			Vector RPoint = SatellitePosition[Ins.Follow] - P1->GetPosition();
			float Angle = vecPolarPoint.AngleBewteenVector(RPoint);
			Ins.Velocity = RPoint.Normalize() * Ins.Velocity.Length();
			Ins.RVelocity = Ins.RVelocity.RotateFromPoint(Vector(), -Angle);
			Ins.Velocity += Ins.RVelocity;
		}
		g_Engine->InsertBulletList(Ins);
	}
}	

void WeaponScript :: DefineSatellite(int NumberOfSatellite, float StartAngle, float SpreadAngle, float Distance)
{
	Vector P_Pos = P1->GetPosition();
	Vector Position = Vector(0.0f, Distance, 0.0f) + P_Pos;
	Position = Position.RotateFromPoint(P_Pos, StartAngle);

	for(int index = 0; index < NumberOfSatellite; index++)
	{
		if(index != 0)
			Position = Position.RotateFromPoint(P_Pos, SpreadAngle);
		SatellitePosition.push_back(Position);
		SatelliteFrame.push_back(0);
	}
	m_fCurrentDistance = m_fDistanceLowest = m_fDistance = Distance;
	m_fDistanceFactor = 0;
}

void WeaponScript :: AddBullet(int LevelList, BulletDef Insert, int PositionId, bool Relative)
{
	ASSERT(PositionId < (int)SatellitePosition.size(), "Satellite Out of index");

	if(PositionId > -1)	
		Insert.Follow = PositionId;

	Insert.m_UseRelative = Relative;
	Insert.m_fAlpha = 0.6f;
	switch(LevelList)
	{
	case 0: LevelBasicList.push_back(Insert);
			break;
	case 1:	LevelOneList.push_back(Insert);
			break;
	case 2:	LevelTwoList.push_back(Insert);
			break;
	case 3:	LevelThreeList.push_back(Insert);
			break;
	case 4:	LevelFourList.push_back(Insert);
			break;

	}
}

void WeaponScript :: PushOrder(int LevelOrder, int index)
{
	switch(LevelOrder)
	{
	case 0:	LevelBasic_Order.push_back(index);
			break;
	case 1:	LevelOne_Order.push_back(index);
			break;
	case 2:	LevelTwo_Order.push_back(index);
			break;
	case 3:	LevelThree_Order.push_back(index);
			break;
	case 4:	LevelFour_Order.push_back(index);
			break;
	}
}

void WeaponScript :: AddBombBullet(BulletDef Insert)
{
	//Insert.Position = P1->GetPosition();
	Insert.m_fAlpha = 0.6f;
	BombList.push_back(new Bullet(Insert));
}

int WeaponScript :: DefineImage(const char * FileName, float Width)
{
	if(m_iTexID != 0)
		g_TexLoader->RemoveTexture(m_iTexID);
	m_fTexWidth = Width;
	return m_iTexID = g_TexLoader->AddTexture(FileName);
}

void WeaponScript :: ChangeFrame(int index, int frame)
{
	if(index < 0 || index > (int)SatelliteFrame.size() - 1)
		return;
	else
		SatelliteFrame[index] = frame;
}

int WeaponScript :: AddImage(const char * FileName)
{
	return g_TexLoader->AddTexture(FileName);
}

void WeaponScript :: AllowOrbit(bool Allow)
{
	if(!Allow) 
		m_fOrbitAngle = 0.0f;
}

void WeaponScript :: SetOrbit(float Angle)
{
	m_fOrbitAngle = Angle;
	if(m_fFocusOrbitAngle == 0)
		m_fFocusOrbitAngle = m_fOrbitAngle;
}

void WeaponScript :: SetFocusOrbit(float Angle)
{
	m_fFocusOrbitAngle = Angle;
}

void WeaponScript :: AllowRotate(bool Allow)
{
	if(!Allow)
		m_fRotationAngle = 0.0f;	
}

void WeaponScript :: SetRotation(float Angle)
{
	m_fRotationAngle = Angle;
	if(m_fFocusRotationAngle == 0)
		m_fFocusRotationAngle = m_fRotationAngle;
}

void WeaponScript :: SetFocusRotation(float Angle)
{
	m_fFocusRotationAngle = Angle;
}

void WeaponScript :: SetLowestDistance(float D)
{
	m_fDistanceLowest = D;
}

void WeaponScript :: SetDistanceFactor(float F)
{
	m_fDistanceFactor = F;
}

void WeaponScript :: Finalize()
{
	*g_Weapon = *this;
}

void WeaponScript :: Read(const char * Path)
{
	lua_State * State = lua_open();
	luabind::open(State);
	luaL_openlibs(State);
	Vector :: Register(State);
	BulletDef :: Register(State);
	WeaponScript :: Register(State);
	luaL_dofile(State, Path);
	try
	{
		lua_close(State);
	}
	catch(exception &E)
	{
		perror("Weapon Memeory Gone");
		perror(E.what());
	}
}

void WeaponScript :: Register(lua_State *State)
{
	luabind::module(State) 
	[
		luabind::class_<WeaponScript>("WeaponSystem")
		.def(luabind::constructor<string>())
		.def("DefineSatellite", &WeaponScript::DefineSatellite)
		.def("AllowOrbit", &WeaponScript::AllowOrbit)
		.def("SetOrbit", &WeaponScript::SetOrbit)
		.def("SetFocusOrbit", &WeaponScript::SetFocusOrbit)
		.def("AllowRotate", &WeaponScript::AllowRotate)
		.def("SetRotation", &WeaponScript::SetRotation)
		.def("SetFocusRotation", &WeaponScript::SetFocusRotation)
		.def("SetLowestDistance", &WeaponScript::SetLowestDistance)
		.def("SetDistanceFactor", &WeaponScript::SetDistanceFactor)
		.def("DefineImage", &WeaponScript::DefineImage)
		.def("AddImage", &WeaponScript::AddImage)
		.def("AddBullet", &WeaponScript::AddBullet)
		.def("Finalize", &WeaponScript::Finalize)
		.def("PushOrder", &WeaponScript::PushOrder)
		.def("ChangeFrame", &WeaponScript::ChangeFrame)

		.def("AddBombParts", &WeaponScript::AddBombBullet)
		.def_readwrite("HoldInPlace", &WeaponScript::HoldInPlace)
		.def_readwrite("PlayerDash", &WeaponScript::PlayerDash)
		.def_readwrite("DashSpeed", &WeaponScript::DashSpeed)
	];
}

#include "stdafx.h"
#include "player.h"
#include "bullet.h"
//real
Player *P1 = new Player();
bool g_godMode = false;
extern bool g_disableKeyboard;

Player :: Player(string Name)
{
	P1->m_sName = Name;
}

Player :: Player()
{
}

Player :: ~Player()
{
	Player_Sprite = NULL;
	Border_Sprite = NULL;
}

void Player :: Render()
{
	g_GameManager->UpdateCounters();
	if( g_diePause < time(0) && g_die )
		g_GameManager->Die();

	glDepthMask(GL_FALSE);
	glPushMatrix();
	glTranslatef(vecPosition.x, vecPosition.y, vecPosition.z);
	if(m_fBorderAlpha != 0)
	{
		glPushMatrix();
		glRotatef(m_fBorderRotation, 0, 0, 1);
		glColor4f(1.0f, 1.0f, 1.0f, m_fBorderAlpha);
		g_TexLoader->DrawXY(m_iBorderSpriteID, 0, 64.0f);		// Draw Border
		glPopMatrix();
	}

	if(g_die)
	{
		glColor4f(1.0f, 0.4f, 0.4f, 1.0f);
		g_TexLoader->DrawXY(m_iPlayerSpriteID, m_Direction, 48);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		g_TexLoader->DrawXY(m_iPlayerSpriteID, 3, 48);	// Draw HitBox
	}
	else if(!g_GameManager->IsGod())
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		g_TexLoader->DrawXY(m_iPlayerSpriteID, m_Direction, 48);
	}
	else if(g_TimeManager->GetFPS() % 2 != 0)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		g_TexLoader->DrawXY(m_iPlayerSpriteID, m_Direction, 48);	
	}
	glColor4f(1.0f, 1.0f, 1.0f, m_fBorderAlpha);
	g_TexLoader->DrawXY(m_iPlayerSpriteID, 3, 48);	// Draw HitBox

	glPopMatrix();
	
	g_Weapon->Render();
	glDepthMask(GL_TRUE);
}

void Player :: Init()
{
	m_sName = "";
	vecPosition = Vector(0.0f, -150.0f, 0.0f);

	m_iSFX_ID = 0;
	g_AEngine->SetListenerPos(vecPosition);
	m_Direction = NORMAL;

	m_iPlayerSpriteID = 0;
	m_iBorderSpriteID = g_TexLoader->AddTexture("Textures/Character/Player/Border.png");
	
	m_fHitBoxSize = HitBoxSize;	//radius
	m_fBorderAlpha = 0.0f;
	m_fBorderRotation = 0.0f;
	m_fSpeed = MOVEMENT_SPEED;

	m_bCanTarget = false;

	m_fMaxSpeed = MOVEMENT_SPEED;
	m_fMinSpeed = SLOW_SPEED;

	ForceSubscribePhysics();
	SetMass(120.0f);
}

void Player :: ProcessInput()
{
	m_Direction = NORMAL;

	if(keys[VK_SHIFT])
	{
		m_fSpeed = m_fMinSpeed;
		(m_fBorderAlpha < 1.0f)? m_fBorderAlpha += 0.05f : m_fBorderAlpha = 1.0f;
	}
	else
	{
		m_fSpeed = m_fMaxSpeed;
		(m_fBorderAlpha > 0.0f)? m_fBorderAlpha -= 0.03f : m_fBorderAlpha = 0.0f;
	}


	if( g_WeatherSys->GetCurrentWeather().wind > 1 )
	{
		if(!keys[VK_UP] && !keys[VK_DOWN] && !keys[VK_LEFT] && !keys[VK_RIGHT])
		{
			m_vPlayerSpeed = Vector();
			m_Direction = NORMAL;
		}
	}
	if(g_Weapon->HoldInPlace && g_GameManager->IsBomb())
		goto SkipBelow;
	if(keys[VK_UP])
	{
		m_vPlayerSpeed.y = m_fSpeed;
	}
	if(keys[VK_DOWN])
	{
		m_vPlayerSpeed.y = -m_fSpeed;
	}
	if(keys[VK_LEFT])
	{
		m_vPlayerSpeed.x = -m_fSpeed;
		m_Direction = LEFT;
	}
	if(keys[VK_RIGHT])
	{
		m_vPlayerSpeed.x = m_fSpeed;
		m_Direction = RIGHT;
	}

	// God mode during demo
#ifdef DEMO_VERSION
	if( keys['Q'] )
	{
		g_godMode = !g_godMode;
		keys['Q'] = false;
	}
	if( keys['W'] )
	{
		float Power = 2.0f;
		g_GameManager->SetPower(Power);
		keys['W'] = false;
	}
#endif

SkipBelow:
	// Shoot Button
	if( g_CutInSystem->IsDialog() || g_disableKeyboard )
		return;
	if(keys['Z'] && g_InGameStatus != NONE && g_InGameStatus != CLEAR)
	{
		g_Weapon->Shoot(g_GameManager->GetPowerStage());
		
		if((g_TimeManager->GetFPS() & 3) == 0)
		{
			g_AEngine->Set3DSoundPos(m_iSFX_ID, vecPosition.Normalize());
			g_AEngine->PlaySegment(m_iSFX_ID, 0);
		}
	}
	if(keys['X'] && g_GameManager->UseBomb())
	{
		g_CutInSystem->LaunchPlayerFlyIn();
		g_Weapon->Bomb();
	}
	if(keys['C'])
	{
		float Power = g_GameManager->GetPower();
		g_WeatherSys->ChangeWind(Power);
		g_GameManager->SetPower(Power);
		keys['C'] = false;
	}
}

void Player :: Update()
{
	if(m_fBorderRotation < 360)
		m_fBorderRotation += 0.5f;
	else
		m_fBorderRotation = 0.0f;

	g_Weapon->Update(keys[VK_SHIFT]);
}

void Player :: UpdateVectors()	// Leave blank
{
	Vector DV = vecPosition;
	DV += vecVelocity + m_vPlayerSpeed;
	if(DV.x >= SCREEN_BORDER_MAX_X || DV.x <= SCREEN_BORDER_MIN_X) 
		vecVelocity.x = 0, m_vPlayerSpeed.x = 0;
	if(DV.y >= SCREEN_BORDER_MAX_Y || DV.y <= SCREEN_BORDER_MIN_Y)
		vecVelocity.y = 0, m_vPlayerSpeed.y = 0;

	vecPosition += vecVelocity + m_vPlayerSpeed;
	g_Weapon->RelativePoint(vecVelocity.x + m_vPlayerSpeed.x, vecVelocity.y + m_vPlayerSpeed.y);

	if( g_WeatherSys->GetCurrentWeather().wind <= 1 )
	{
		vecVelocity = m_vPlayerSpeed;
		if(vecVelocity.Length() > 0)
		{
			vecAcceleration = -vecVelocity.Normalize() * 0.1f;
			vecVelocity += vecAcceleration;
			if(vecVelocity.Length() < 0.1f)
				vecVelocity = Vector();
		}
		m_vPlayerSpeed = vecVelocity;
	}
}

void Player :: UseWeapon(int index)
{
	g_Weapon->Read(WeaponPaths[index].c_str());
}

void Player :: ResetPosition()
{
	vecPosition = Vector(0.0f, -70.0f, 0.0f);
}

string Player :: GetName()
{
	return m_sName;
}

vector <string> Player :: GetCutinPath()
{
	return CutinPaths;
}

vector <string> Player :: GetWeaponPaths()
{
	return WeaponPaths;
}

int Player :: DefineImage(const char * Path)
{
	return (P1->m_iPlayerSpriteID = g_TexLoader->AddTexture(Path));
}

int Player :: DefineShotSFX(const char * Path)
{
	return (P1->m_iSFX_ID = g_AEngine->LoadSegment((char *)Path, false, -1));
}

void Player :: DefineSpeed(float max, float min)
{
	P1->m_fMinSpeed = min;
	P1->m_fMaxSpeed = max;
}

void Player :: PushWeaponPath(string Path)
{
	P1->WeaponPaths.push_back(Path);
}

void Player :: PushCutinPath(string Path)
{
	P1->CutinPaths.push_back(Path);
}

void Player :: Register(lua_State * State)
{
	P1->Init();
	luabind::module(State) 
	[
		luabind::class_<Player>("Player")
		.def(luabind::constructor<string>())
		.def("DefineImage", &Player::DefineImage)
		.def("DefineShotSFX", &Player::DefineShotSFX)
		.def("DefineSpeed", &Player::DefineSpeed)
		.def("AddWeaponFilePath", &Player::PushWeaponPath)
		.def("AddCutinPath", &Player::PushCutinPath)
	];
}

void Player :: Read(const char * Path)
{
	P1->CutinPaths.clear();
	P1->WeaponPaths.clear();
	lua_State * State = lua_open();
	luabind::open(State);
	luaL_openlibs(State);
	Player :: Register(State);
	luaL_dofile(State, Path);
	try
	{
		lua_close(State);
	}
	catch(exception &E)
	{
		perror("Player Memeory Gone");
		perror(E.what());
	}
}

#include "stdafx.h"
#include "bullet.h"

/* 
Changed Perspective of Rendering from xy to xz
*/

Vector vecPolarPoint = Vector(0,1,0);

void BulletDef :: Register(lua_State *State)
{
	luabind::module(State) 
	[
		luabind::class_<BulletDef>("BulletDef")
		.def(luabind::constructor<>())
		.def(luabind::constructor<BulletDef>())
		.def_readwrite("Position", &BulletDef::Position)
		.def_readwrite("Velocity", &BulletDef::Velocity)
		.def_readwrite("Acceleration", &BulletDef::Acceleration)
		.def_readwrite("HitBox", &BulletDef::HitBox)
		.def_readwrite("Scale", &BulletDef::Scale)
		.def_readwrite("Angle", &BulletDef::Angle)
		.def_readwrite("Homing", &BulletDef::Homing)
		.def_readwrite("TexID", &BulletDef::m_TexID)
		.def_readwrite("TexWidth", &BulletDef::m_TexWidth)
		.def_readwrite("Color", &BulletDef::m_ColorID)
		.def_readwrite("RelativeVelocity", &BulletDef::RVelocity)
		.def_readwrite("Damage", &BulletDef::m_iDamage)
		.def_readwrite("Spin", &BulletDef::m_bSpining)
		.def_readwrite("SpinAngle", &BulletDef::SpinAngle)
		.def_readwrite("HomeFactor", &BulletDef::HomeFactor)
		.def_readwrite("Control", &BulletDef::m_Control)
		.def_readwrite("Interval", &BulletDef::Interval)
		.def_readwrite("EnableBounce", &BulletDef::m_bBounce)
		.def_readwrite("BounceTimes", &BulletDef::m_iBounceMax)
		.def_readwrite("Mass", &BulletDef::m_fMass)
	];
}

Bullet :: ~Bullet()
{
}

void Bullet :: Render()
{
	if(m_bRemove)
		return;
	glDepthMask(GL_FALSE);
	glPushMatrix();
	glTranslatef(vecPosition.x, vecPosition.y, vecPosition.z);
	glRotatef(m_fAngle * 180.0f / PIE, 0, 0, 1);
	glScalef(m_fScale, m_fScale, m_fScale);
	glColor4f(1.0f, 1.0f, 1.0f, m_fAlpha);
	g_TexLoader->DrawXY(m_iBulletSpriteID, m_iColorID, m_fSpriteWidth);
	glDepthMask(GL_TRUE);
	glPopMatrix();
}

void Bullet :: Init()
{
	
}

void Bullet :: ReInit(BulletDef & Def)
{
	vecPosition = Def.Position;
	vecAcceleration = Def.Acceleration;
	if(Def.Angle != 0 && Def.Velocity.Length() != 0)
	{
		vecVelocity = vecPolarPoint.RotateFromPoint(Vector(), m_fAngle) * Def.Velocity.Length();
		m_fAngle = 0.0f;
	}
	else
	{
		m_fAngle = 0.0f;
		vecVelocity = Def.Velocity;
	}
	m_fHitBoxSize = Def.HitBox;
	m_fScale = Def.Scale;		// not needed
	m_bHoming = Def.Homing;

	m_iBulletSpriteID = Def.m_TexID;
	m_iColorID = Def.m_ColorID;
	m_fSpriteWidth = Def.m_TexWidth;
	m_iDamage = Def.m_iDamage;
	m_bRelease = !Def.m_Control;
	m_bRemove = false;
	m_bSpining = Def.m_bSpining;
	m_fSpinAngle = Def.SpinAngle;
	m_fHomeFactor = Def.HomeFactor;
	m_iBounceMax = Def.m_iBounceMax;
	m_bBounce = Def.m_bBounce;
	m_iBounceCount = 0;
	m_fAlpha = Def.m_fAlpha;
	SetMass(Def.m_fMass);
}

void Bullet :: Update()
{
	if(m_oTarget != NULL)
	{
		if(m_bHoming)
			vecAcceleration = -(vecPosition - m_oTarget->GetPosition()).Normalize() * 10.0f;
		if(m_oTarget->Skip())
		{
			m_oTarget = NULL;
			vecVelocity = Vector(0, 25, 0);
			m_bHoming = false;
		}
	}
	if(m_bBounce)
	{
		if(vecPosition.x < -240.0f || vecPosition.x > 240.0f)
		{
			vecVelocity.x = -vecVelocity.x;
			m_iBounceCount++;
		}	
		if(vecPosition.y > 260.0f || vecPosition.y < -260.0f)
		{
			vecVelocity.y = -vecVelocity.y;
			m_iBounceCount++;
		}
		
		if(m_iBounceCount >= m_iBounceMax)
		{
			m_iBounceCount = 0;
			m_bBounce = false;
		}
	}
	if(vecPosition.x < -320.0f || vecPosition.x > 320.0f || vecPosition.y > 340.0f || vecPosition.y < -340.0f)
		m_bRemove = true;
}

void Bullet :: UpdateVectors()
{
	while(m_bHoming && vecVelocity.Length() > m_fHomeFactor)	// set to 2.5
		vecVelocity *= 0.8f;
	vecPosition += vecVelocity;	
	Vector newPosition(GetPosition());
	
	if(m_bSpining)
		m_fAngle += m_fSpinAngle;
	else if(vecVelocity.Length() > 0)
		m_fAngle = vecPolarPoint.AngleBewteenVector(vecVelocity);
	if(m_fAngle < 0)
		m_fAngle += 2.0f * PIE;

	vecVelocity += vecAcceleration;
}

void Bullet :: SetTarget(CObject * Target)
{
	if(Target == NULL)
		return;
	if(m_oTarget == NULL && m_bHoming)
	{
		vecVelocity = Vector();
		m_oTarget = Target;
	}
}

void Bullet :: ApplyWind(Vector & Wind_D)
{
	vecAcceleration += Wind_D;
}

void Bullet :: Register(lua_State * State)
{
	luabind::module(State) 
	[
		luabind::class_<Bullet>("Bullet")
		.def(luabind::constructor<BulletDef>())
		.def_readwrite("Color", &Bullet::m_iColorID)
		.def("GetPosition", &Bullet::GetPosition)
		.def("GetVelocity", &Bullet::GetVelocity)
		.def("GetAcceleration", &Bullet::GetAcceleration)
		.def("SetPosition", &Bullet::SetPosition)
		.def("SetVelocity", &Bullet::SetVelocity)
		.def("SetAcceleration", &Bullet::SetAcceleration)
		.def("Removal", &Bullet::Removal)
		.def_readwrite("Release", &Bullet::m_bRelease)
	];
}

Bullet& Bullet :: operator= (Bullet& o)
{
	if( this != &o )
	{
		CObject:: operator= (o);

		m_iBulletSpriteID = o.m_iBulletSpriteID;
		m_iColorID = o.m_iColorID;
		m_fSpriteWidth = o.m_fSpriteWidth;
			
		m_fAlpha = o.m_fAlpha;
		m_fSpinAngle = o.m_fSpinAngle;
		m_fAngle = o.m_fAngle;
		m_fScale = o.m_fScale;

		m_bHoming = o.m_bHoming;
		m_bSpining = o.m_bSpining;
		m_bBounce = o.m_bBounce;

		m_iBounceCount = o.m_iBounceCount;
		m_iBounceMax = o.m_iBounceMax;
		m_fHomeFactor = o.m_fHomeFactor;
		LastUntilRelease = o.LastUntilRelease;

		m_oTarget = o.m_oTarget;
	}
	return *this;
}
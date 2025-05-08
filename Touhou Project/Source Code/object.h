#pragma once

/* Notes
This is the standard Template for all objects, 
so that many other things such bullets or enemy can be based on

you have to inherit this if you want to plug into the engine.
besure to update the movement if needed else it uses the standard movement
please do update processinput if your class needs any input
also update UpdateVectors() to blank if you don't need them
if needed define Update() for ususage

to remove from the list in engine set m_bRemove to true while updating
This also has pre-made collision detection but its not recommended if bullet are laser type, in that case
overload the funciton in the .h / .cpp to suit your thing.

uses distance and compares if its smaller than the hit boxes of both object
*/

#define Standard_HitBox 0.0f

class CObject
{

	friend class WeatherSystem;
	static WeatherSystem *mpWthSys; // defined in WeatherSystem.cpp
	int mPhysicsId;
protected:

	Vector vecPosition;			
	Vector vecVelocity;		
	Vector vecAcceleration;

	float m_fHitBoxSize;
	
	bool m_bRelease;
	bool m_bCanTarget;
	bool GrazeDetect;

	int m_iDamage;

public:
	bool m_bRemove;

	CObject(Vector &vecPos, Vector &vecVelo, Vector &vecAcc, float HitBoxSize, int DMG = 0)
		:	vecPosition(vecPos),
			vecVelocity(vecVelo),
			vecAcceleration(vecAcc),
			m_fHitBoxSize(HitBoxSize),
			m_bRemove(false), m_bRelease(true),
			m_bCanTarget(true), GrazeDetect(false),
			m_iDamage(DMG)
	{
		GrazeDetect = true;
		m_bRemove = false;
		if( mpWthSys != NULL )
			mPhysicsId = mpWthSys->Subscribe(this);
	}
	CObject()
		:	m_fHitBoxSize(Standard_HitBox),
			m_iDamage(0),
			m_bRemove(false), m_bRelease(true), GrazeDetect(false)
	{
		if( mpWthSys != NULL )
			mPhysicsId = mpWthSys->Subscribe(this);
	}
	virtual ~CObject()
	{
		if( mpWthSys != NULL )
			mpWthSys->Unsubscribe(mPhysicsId);
	}
	CObject(const CObject& o)
	{
		vecPosition = o.vecPosition;
		vecVelocity = o.vecVelocity;
		vecAcceleration = o.vecAcceleration;

		m_fHitBoxSize = o.m_fHitBoxSize;
		m_bRemove = o.m_bRemove;
		m_bRelease = o.m_bRelease;
		m_bCanTarget = o.m_bCanTarget;
		GrazeDetect = o.GrazeDetect;

		m_iDamage = o.m_iDamage;

		if( mpWthSys != NULL )
			mPhysicsId = mpWthSys->Subscribe(this);
	}
	virtual CObject& operator=(const CObject& o)
	{
		if( this != &o )
		{
			if( mpWthSys != NULL )
				mpWthSys->Unsubscribe(mPhysicsId);

			mPhysicsId = o.mPhysicsId;
			ResetPhysics();

			vecPosition = o.vecPosition;
			vecVelocity = o.vecVelocity;
			vecAcceleration = o.vecAcceleration;

			m_fHitBoxSize = o.m_fHitBoxSize;
			m_bRemove = o.m_bRemove;
			m_bRelease = o.m_bRelease;
			m_bCanTarget = o.m_bCanTarget;
			GrazeDetect = o.GrazeDetect;

			m_iDamage = o.m_iDamage;
		}
		return *this;
	}
	void ForceSubscribePhysics()
	{
		if( mpWthSys != NULL )
		{
			mpWthSys->Unsubscribe(mPhysicsId);
			mPhysicsId = mpWthSys->Subscribe(this);
		}
	}
	void ResetPhysics()
	{
		if( mpWthSys != NULL )
			mpWthSys->Reinit(this, mPhysicsId);
	}
	virtual void Render()
	{
	}
	virtual void Init() = 0;
	virtual void ProcessInput()
	{

	}
	virtual void UpdateVectors()
	{
		vecPosition += vecVelocity;
		vecVelocity += vecAcceleration;
	}
	virtual void Update()
	{

	}
	virtual void CollisionReaction()
	{

	}

	bool Skip()
	{
		return m_bRemove;
	}

	bool CanRelease()
	{
		return m_bRelease;
	}

	void Release()
	{
		m_bRelease = true;
	}

	bool CanDelete()
	{
		//if(m_bRemove && m_bRelease)
			
		return (m_bRemove && m_bRelease);
	}

	Vector GetPosition()
	{
		return vecPosition;
	}

	Vector GetVelocity()
	{
		return vecVelocity;
	}

	Vector GetAcceleration()
	{
		return vecAcceleration;
	}

	float GetHitboxSize()
	{
		return m_fHitBoxSize;
	}

	int GetDamage()
	{
		return m_iDamage;
	}

	void SetMass(float Weight)
	{
		if( Weight > -30 )
			mpWthSys->SetMass(mPhysicsId, Weight);
	}

	virtual void SetPosition(Vector & newPos)
	{
		vecPosition = newPos;
	}

	virtual void SetVelocity(Vector & newVel)
	{
		vecVelocity = newVel;
	}

	virtual void SetAcceleration(Vector & newAcc)
	{
		vecAcceleration = newAcc;
	}

	void SetHitbox(float Size, bool changeMass = true)
	{
		m_fHitBoxSize = Size;
		if( Size > -30 && changeMass )
			mpWthSys->SetMass(mPhysicsId, Size);
	}

	virtual bool Removal()
	{
		return (m_bRemove = true);
	}

	bool getGrazeDetect()
	{
		return GrazeDetect;
	}
	void ChangeGrazeDetect()
	{
		if(GrazeDetect)
			GrazeDetect = false;
	}
};


inline bool Col_ObjToObj(CObject & T1, CObject & T2)
{
	if((T1.GetPosition() - T2.GetPosition()).Length() < T1.GetHitboxSize() + T2.GetHitboxSize())
		return true;
	return false;
}
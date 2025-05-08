#pragma once

/* Notes
To use Bullet, Define BulletDef and Throw in
To Control Via instructions, use AddInstruction()
Instruction is read based on Time Frame.
*/

extern Vector vecPolarPoint;	// North Point

class BulletDef					// Definition to pass in
{
public:
	Vector Position, Velocity, Acceleration, RVelocity;
	int Follow;	//used for satellites in weapon script
	float HitBox, Scale, Angle, SpinAngle;
	bool Homing;

	int m_TexID;
	float m_TexWidth;
	int m_ColorID;
	int m_iDamage;

	bool m_UseRelative;
	bool m_bSpining;
	bool m_Control;

	float HomeFactor;
	float m_fAlpha;

	int Interval;

	bool m_bBounce;
	int m_iBounceMax;

	bool UntilRelease;

	float m_fMass;
	
	BulletDef()
		:	Position(), Velocity(), Acceleration(), RVelocity(),
			HitBox(0.0f), Scale(1.0f), Angle(0.0f), SpinAngle(0.0f),
			Homing(false), m_UseRelative(false), m_bSpining(false),
			m_TexID(0), m_iDamage(0), m_TexWidth(0),
			m_ColorID(0), HomeFactor(25.0f), m_fAlpha(1.0f),
			Follow(-1), m_Control(true), Interval(8), m_bBounce(false), m_iBounceMax(0), m_fMass(0)
	{
	}
	BulletDef(const BulletDef & Copy)
	{
		*this = Copy;
	}
	static void Register(lua_State * State);
};

class Bullet : public CObject
{
	int m_iBulletSpriteID;
	int m_iColorID;
	float m_fSpriteWidth;
	
	float m_fAlpha;
	float m_fSpinAngle;			// Spin How much	
	float m_fAngle;				// Rotate the bullet (screen stuff)
	float m_fScale;				// Determinds how big the bullet will appear

	bool m_bHoming;				// Homing Mode?
	bool m_bSpining;			// Use Spining?
	bool m_bBounce;

	int m_iBounceCount;
	int m_iBounceMax;
	float m_fHomeFactor;
	
	CObject * m_oTarget;

	bool LastUntilRelease;

public:
	Bullet(BulletDef & Def)
		:	CObject(Def.Position, Def.Velocity, Def.Acceleration, Def.HitBox, Def.m_iDamage),
			m_fAngle(Def.Angle), m_fScale(Def.Scale), m_fSpinAngle(Def.SpinAngle),
			m_bHoming(Def.Homing), m_bSpining(Def.m_bSpining),
			m_iBulletSpriteID(Def.m_TexID),
			m_iColorID(Def.m_ColorID), m_fHomeFactor(Def.HomeFactor),
			m_fSpriteWidth(Def.m_TexWidth), m_fAlpha(Def.m_fAlpha), m_bBounce(Def.m_bBounce), m_iBounceMax(Def.m_iBounceMax),
			LastUntilRelease(Def.UntilRelease)
	{
		m_iBounceCount = 0;
		m_bRelease = !Def.m_Control;
		m_oTarget = NULL;
		m_bCanTarget = false;
		if(m_fAngle != 0 && vecVelocity.Length() != 0)
		{
			vecVelocity = vecPolarPoint.RotateFromPoint(Vector(), m_fAngle) * Def.Velocity.Length();
			m_fAngle = 0.0f;
		}
		if(Def.m_fMass > 0)
			SetMass(Def.m_fMass);
	}

	~Bullet();
	void Render();
	void Init();
	void ReInit(BulletDef & Def);
	void Update();
	void UpdateVectors();
	void ApplyWind(Vector & Wind_D);

	void SetTarget(CObject * Target);
	
	void SetAcceleration(Vector &newAcc)
	{
		if(!m_bHoming)
			vecAcceleration = newAcc;
	}
	void SetAngle(float Angle)
	{
		m_fAngle = Angle;
	}
	bool Removal()
	{
		if(LastUntilRelease && !m_bRelease)
			return true;
		return (m_bRemove = true);
	}
	void SetScale(float Scale)
	{
		m_fScale = Scale;
	}

	Bullet& operator= (Bullet& o);
	// Export to script
	static void Register(lua_State * State);
};

#pragma once

/* Notes
This is enemy class that draw and move character
*/
class Enemy	: public CObject
{
protected:
	int m_iEnemySpriteID;
	
	float m_fSpriteWidth, currentangle;
	int m_iHitPoints, m_itotalhitpoints;
	bool on_hover,count,g_rotate;
	int onCount;
	vector <Enemy*> Control;

public:
	Vector rotatepoint, HeadToPosition;
	int ExploCounter;
	int m_iAnimationFrame;
	float angle,distance;
	Enemy(Vector & Pos, Vector & Vel, Vector & Acc, float HitBox, int HP, int TextureID, float SpriteWidth)
		:	CObject(Pos, Vel, Acc, HitBox),
			m_iHitPoints(HP),
			m_itotalhitpoints(HP),
			m_iEnemySpriteID(TextureID),
			m_fSpriteWidth(SpriteWidth),
			m_iAnimationFrame(0)
	{
		count = true; on_hover = false;
		ExploCounter = onCount = 0;
	    currentangle = angle = distance = 0.0f;
		HeadToPosition = rotatepoint = Vector(0,0,0);
		angle = -1;
		SetMass(100000);
	}
	~Enemy();
	void Init();
	void AddToControl(Enemy*);
	
	void Update();
	void UpdateVectors();
	void Render();
	void GoToPosition(Vector);
	
	int GetHp();
	int GetMaxHp();
	float getPercentage();
	
	void Onhover();
	void Sethover();
		
	void EnemyRotate();
	void SetRotate();
	void OffRotate();

	void ApplyDamage(int Dmg);
	void CollisionReaction();
	void OutOfBound();
	void UpdateDistance();

	bool Checkdistance();
};
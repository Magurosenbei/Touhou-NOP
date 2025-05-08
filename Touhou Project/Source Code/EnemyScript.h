#pragma once

#include "boss.h"
#include "HeadsupDisplay.h"

class EnemyScript
{
	vector <Enemy *> Enemies;
	int m_hp;
	bool m_release, m_dafault;
public:
	EnemyScript();
	void AddEnemy(int, Vector, float,int);
	void AddToEngine(bool,unsigned int);
	void AddReward();

	// Adding objects
	void AddAccel(Vector, unsigned int);
	void ChangeAnimation(int, unsigned int);
	void ChangePosition(unsigned int, Vector);
	void SetHover(unsigned int);
	void SetRotate(Vector, float, unsigned int);
	void OffRotate(unsigned int);
	void GoToPosition(Vector, unsigned int);
	void AddToControl(unsigned int, unsigned int);

	// Interacting with 
	bool isMoving(unsigned int);
	bool AtBorder(unsigned int);
	bool isDead(unsigned int, int);
	bool ComparePosition(Vector , Vector,int);
	bool CheckStatus(unsigned int);
	bool CheckDistance(unsigned int);

	// checking functions
	int getHp(unsigned int);
	Vector getposition(unsigned int);
	float getpercent(unsigned int);

	// Getting variables
	void Release(unsigned int);
	void getC(int);
	void LifebarSetup(unsigned int);


	// Releasing the elements
static void Register(lua_State*);
	// Registering the Lua
};


class EnemyWave
{
	vector <Enemy *> List;
public:
	EnemyWave();
	~EnemyWave();
	void AddEnemy(int id, float TexWidth, float HitBox, int HP, int frame);
	void SetPosition(int index, Vector Pos);
	void SetVelocity(int index, Vector Vel);
	void SetAcceleration(int index, Vector Acc);
	
	Vector GetPosition(int index);
	Vector GetVelocity(int index);
	Vector GetAcceleration(int index);

	int GetHp(int index);
	int Size();

	void ReleaseAll();
	void ThrowInAll();
	void Throw(int id, float TexWidth, float HitBox, int HP, int frame);
	void ThrowFromStore(int index);

	static void Register(lua_State * State);
};
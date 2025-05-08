#pragma once
/* Notes
This is the Game Engine that runs all the functions
You Process Keyboard Inputs and Mouse from here
Frame is drawn here too
StartUp of functions and other things also starts here
*/

#include "object.h"
#include "singleton.h"
#include "factory.h"

#include "bullet.h"
#include "enemy.h"

class CEngine : public Singleton <CEngine>
{
//------ Lists
public:
	list <Enemy *> EnemyList;					
	int m_Explosion;	
	bool m_bRendering;								// Share Access Violation
	Factory <Bullet, BulletDef> PlayerBullet;		// Factory
	Factory <Bullet, BulletDef> EnemyBullet;
public:
	CEngine();
	~CEngine();
	bool InitEngine();
	void RenderFrame();
	void UpdateGame();
	void ProcessInput();
	
	void InsertEnemyList(Enemy * ToBeInset);		

	Bullet * InsertBulletList(Bullet * ToBeInset);
	Bullet * InsertBulletList(BulletDef & Def);

	Bullet * InsertEnemyBullet(Bullet * ToBeInset);	
	Bullet * InsertEnemyBullet(BulletDef & Def);
	
	void UpdateObjects();								
	void UpdateBulletList();
	void UpdateEnemyBulletList();
	void UpdateEnemy();

	bool CollideEnemy(CObject & Target);				// returns true when collided
	bool CollideEnemyBullet(CObject & Target);			// returns true when collided 

	void RenderExplosion(Vector, int&);
};

extern CEngine * g_Engine;

DWORD WINAPI StageThread(LPVOID pParams);
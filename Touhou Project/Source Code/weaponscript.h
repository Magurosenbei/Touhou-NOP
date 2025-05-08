#pragma once

#include "bullet.h"

#define MIN_DISTANCE = 10.0f

class WeaponScript
{
	BulletDef Bt_Def;
	Bullet *BombB;
	Bullet *BombTemp;
	string m_sLabelName;

	int m_iTexID;
	float m_fTexWidth;

	float m_fDistance;			// Max
	float m_fDistanceLowest;	// Min
	float m_fCurrentDistance;	
	float m_fDistanceFactor;

	float m_fCurrentAngle;
	float m_fRotationAngle;
	float m_fOrbitAngle;

	float m_fFocusOrbitAngle;
	float m_fFocusRotationAngle;

	vector <int>	SatelliteFrame;
	vector <Vector> SatellitePosition;	// List of Position

	vector <int> LevelBasic_Order;
	vector <int> LevelOne_Order;			// Order of which satelite to draw
	vector <int> LevelTwo_Order;
	vector <int> LevelThree_Order;			// 
	vector <int> LevelFour_Order;

	list <BulletDef> LevelBasicList;
	list <BulletDef> LevelOneList;
	list <BulletDef> LevelTwoList;
	list <BulletDef> LevelThreeList;
	list <BulletDef> LevelFourList;

	vector <Bullet *> BombList;

public:
	bool HoldInPlace;
	bool PlayerDash;
	Vector DashSpeed;

	WeaponScript();
	WeaponScript(string Title);
	void Render();
	void LevelRender(vector <int> &Level);

	void Update(bool FocusMode);
	void RelativePoint(float dx, float dy);
	void Bomb();
	void Shoot(int PowerLevel);
	void ShootLevel(list <BulletDef> &List);
	// Script usage
	void DefineSatellite(int NumberOfSatellite, float StartAngle, float SpreadAngle, float Distance);
	void AddBullet(int LevelList, BulletDef Insert, int PositionId, bool Relative);
	void AddBombBullet(BulletDef Insert);
	void PushOrder(int LevelOrder, int index);
	// Make Texture
	int DefineImage(const char * FileName, float width = 0);
	void ChangeFrame(int index,int frame);
	int AddImage(const char * FileName);

	void AllowOrbit(bool Allow);	// Allow Satellites to orbit
	void SetOrbit(float Angle);		
	void SetFocusOrbit(float Angle);

	void AllowRotate(bool Allow);
	void SetRotation(float Angle);
	void SetFocusRotation(float Angle);

	void SetLowestDistance(float D);
	void SetDistanceFactor(float F);

	void Finalize();

	void Read(const char * Path);
	static void Register(lua_State * State);
};

extern WeaponScript * g_Weapon;
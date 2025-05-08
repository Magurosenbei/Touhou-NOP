#pragma once

class BulletWave
{
	vector <Bullet *> List;
public:
	BulletWave();
	~BulletWave();
	void AddBullet(BulletDef & Link);
	void SetBullet(int index, Bullet Bullet);
	void ReleaseAll();
	void ThrowInAll();
	void Throw(BulletDef & Throw);

	Vector GetPosition(int index);
	Vector GetVelocity(int index);
	Vector GetAcceleration(int index);
	void SetPosition(int index, Vector Pos);
	void SetVelocity(int index, Vector Vel);
	void SetAcceleration(int index, Vector Acc);
	void SetAngle(int index, float Angle);
	void SetScale(int index, float Scale);
	void SetHitBox(int index, float HitBox);
	int SizeOfWave();
	static void Register(lua_State * State);
};
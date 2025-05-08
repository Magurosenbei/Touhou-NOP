#pragma once

/* Notes
This is the player class
does nothing much except draw and move character

*/

#include "weaponscript.h"

#define MOVEMENT_SPEED 3.0f
#define SLOW_SPEED 1.0f
#define HitBoxSize 5.0f

class Player : public CObject
{
	string m_sName;
protected:
	int m_iPlayerSpriteID;		// must be 48 * 48 and 3 diffrent pose + hitbox
	int m_iBorderSpriteID;		// Default
	Texture * Player_Sprite;	// a pointer to the storage of all sprite images (3 images)
	Texture * Border_Sprite;	// a pointer to the border sprite (one only)

	Direction m_Direction;		// this determines which sprite will show up

	float m_fBorderRotation;	// this is to rotate the border
	float m_fBorderAlpha;		// this is used to make the border disappear and appear
	float m_fSpeed;				// Speed of movement

	float m_fMinSpeed;			//	Slow Speed
	float m_fMaxSpeed;			//	Normal Speed
	Vector m_vPlayerSpeed;		// Player's Movement Speed

	int m_iSFX_ID;

	vector <string> WeaponPaths;
	vector <string> CutinPaths;

public:
	Player(string Name);
	Player();
	~Player();
	
	void Render();
	void Init();
	void ProcessInput();
	void Update();
	void UpdateVectors();

	void UseWeapon(int index);
	void ResetPosition();
	string GetName();
	vector <string> GetCutinPath();				// Stores all the cutin paths
	vector <string> GetWeaponPaths();			// Stores all file Path of weapons

	//Script use
	int DefineImage(const char * Path);			// Returns the Image ID
	int DefineShotSFX(const char * Path);		// Returns the SFX ID
	void DefineSpeed(float max, float min);
	void PushWeaponPath(string Path);
	void PushCutinPath(string Path);

	static void Register(lua_State * State);
	void Read(const char * Path);

	void Reset()
	{
		Vector v =  Vector(0.0f, -150.0f, 0.0f) - vecPosition;
		g_Weapon->RelativePoint(v.x, v.y);
		vecPosition = Vector(0.0f, -150.0f, 0.0f);
	}
	///
	void SetPosition(Vector &newPos)
	{
		if(newPos.x >= SCREEN_BORDER_MAX_X || newPos.x <= SCREEN_BORDER_MIN_X || newPos.y >= SCREEN_BORDER_MAX_Y || newPos.y <= SCREEN_BORDER_MIN_Y) 
			return;
		
		Vector v =  newPos - vecPosition;
		g_Weapon->RelativePoint(v.x, v.y);
		vecPosition = newPos;
	}
};

extern Player * P1;
extern bool g_godMode;
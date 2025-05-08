#pragma once


// Modifiables
#define FLYIN_PLAYER_X				-360	// This is where the player textures start
#define FLYIN_PLAYER_Y				-280

#define FLYIN_PLAYER_LIFE			0.005f	// How much to reduce life?
#define FLYIN_PLAYER_SPEED_X		10		// How much to move in the X-direction?
#define FLYIN_PLAYER_SPEED_Y		50		// How much to move in the Y-direction?

#define FLYIN_ENEMY_X				120		// This is where the enemy textures start
#define FLYIN_ENEMY_Y				280

#define FLYIN_ENEMY_LIFE			0.005f	// How much to reduce life?
#define FLYIN_ENEMY_SPEED_X			-10		// How much to move in the X-direction?
#define FLYIN_ENEMY_SPEED_Y			-50		// How much to move in the Y-direction?


// Manages flyins in the game. Singleton method.
// This class is part of the CutIn system. Use the CutInSystem interface instead.
class FlyInManager
{
public:

	// Takes in dialog manager's texture manager.
	// Returns a flyin manager or NULL pointer if manager is already created.
	static FlyInManager* Init(Texture& texman);
	static void Deinit(FlyInManager*& what);

	void RegisterLua(lua_State& L);
	void Reset();

	void Render();
	void Launch(const bool move = true); // launches player's flyin
	void StopAll(); // immediately stops all flyins


private:

	// Locked
	FlyInManager(Texture& texman);
	~FlyInManager();
	FlyInManager(const FlyInManager&);
	const FlyInManager& operator=(const FlyInManager&);


	// Lua Callbacks
	static int Lua_Set(lua_State* L);
	static int Lua_Launch(lua_State* L);
	static int Lua_StopAll(lua_State* L);


private:

	// Template for FlyIn
	struct tFlyIn
	{
		int id;
		float life;
		Vector pos;
		bool move;
	};


	static FlyInManager *mpInst;
	Texture& mTexManager;
	tFlyIn mPlayer, mEnemy;
};
#pragma once

#include <list>
#include <vector>
#include "object.h"


// Modifables
// Game Arena
#define ITEM_TOP			1000
#define ITEM_BOTTOM			-300
#define ITEM_LEFT			-270
#define ITEM_RIGHT			270


enum ItemType { NORMAL_ITEM, BONUS_ITEM, SPECIAL_ITEM };
// Normal items are well, normal. They get deducted when dropped by player.
// Bonus items are extras that do not deduct when dropped by player.
// Only one special item can appear onscreen at anytime.

enum DropType { ENEMY_DROP, BOSS_DROP, PLAYER_DROP };
// Enemy drop items for enemy and boss: DropEnemy() and DropBoss()
// Boss drop items only drop for boss: DropBoss()
// Player drop items only drop on: DropPlayer()


// Item Definition
struct ItemDef
{
	// Constructors
	ItemDef(const int& itemtype, const int& droptype, const float& droprate, const int& score, const int& life,
		const int& spell, const float& power, const int& frame, const float& width, const float& hitbox, const float& mass = 0)
		: dropRate(droprate), score(score), life(life), spell(spell), power(power), frame(frame), width(width),
		hitbox(hitbox), mass(mass) { itemType = (ItemType)itemtype; dropType = (DropType)droptype; }

	ItemDef() : dropRate(0), score(0), life(0), spell(0), power(0), frame(0), width(0), hitbox(0), mass(0) {}

	// Properties
	ItemType itemType;
	DropType dropType;
	float dropRate;

	// Effects
	int score;
	int life;
	int spell;
	float power;

	// Texture
	int frame;
	float width;
	float hitbox;
	float mass;
};


// This class manages all the items dropped & picked up
// in game. Singleton method. Don't bother about object
// creation, it will automatically be managed.
class ItemSystem
{
public:

	// Initialize
	static ItemSystem* Init(const std::string& tdir);
	static void Deinit(ItemSystem*& what);

	// Register
	void Register(lua_State& L);
	void Reset();

	// Render
	void Step(GamePlay& gp, CObject& player);
	void Render();

	// Drops
	void DropEnemy(const Vector& pos);
	void DropBoss(const Vector& pos);

	// This function is different from the above two. It
	// drops based on score, spell and power passed in.
	// It also deducts the amount dropped. Doesn't drop
	// more than amount specificed.
	void DropPlayer(const Vector& pos, GamePlay& gp);


private:

	// Locked
	ItemSystem(const std::string& tdir);
	~ItemSystem();
	ItemSystem(const ItemSystem&);
	const ItemSystem& operator=(const ItemSystem&);

	// Lua Callbacks
	static int Lua_Add(lua_State* L); // item type, drop type, drop rate, score, life, spell, power, frame, width, hitbox, mass
	static int Lua_SetThrow(lua_State* L); // max x move, max y move, gravity
	static int Lua_SetAmount(lua_State* L); // enemy, boss, player, variation
	static int Lua_SetTexture(lua_State* L); // item texture filepath
	static int Lua_SetMagnetic(lua_State* L); // magnetic line, absorption speed

	// Internals
	const ItemDef* PollDrop(const DropType& dt);
	const Vector PollScale();


private:

	// Template for item drops
	class tItem : public CObject
	{
	public:

		tItem(const ItemDef* base, Vector& gravity, const Vector& scale, const Vector& pos)
			: CObject( Vector(pos), gravity, Vector(), base->hitbox),
			base(base), scale(scale), absorb(false), step(0)
			{
				if( base->mass != 0 )
				{
					SetMass(base->mass);
					SetVelocity( gravity * (1.0f / base->mass) ); // bigger items drop slower
				}
				else
				{
					SetMass(base->mass - 30);
				}
			}

		tItem() : CObject() {}
		~tItem() {}
		void Init() {}

		void Render(Texture& texman);

		const ItemDef *base;
		Vector scale; // affects the curvature of the drop
		bool absorb; // affects movement towards player
		int step;
	};


	// Managers
	static ItemSystem *mpInst;
	const std::string mTexDir;
	Texture mTexManager;

	// Objects
	std::vector< ItemDef* > mItemList;
	std::list< tItem* > mDropList;

	// We use a list instead of vector because it has O(1) when deleting elements.

	// States
	bool mMagnet; // this prevents continuous magnetic field
	bool mAbsorb; // this tells our items to move towards player

	// Attributes
	int mEnemyDropAmount;
	int mBossDropAmount;
	int mPlayerDropAmount;

	int mDropVariation;
	float mMaxXMove;
	float mMaxYMove;

	int mMagneticLine;
	float mGravity;
	float mAbsorbSpd;

	bool mLocked;
};

extern ItemSystem *g_ItemSys;
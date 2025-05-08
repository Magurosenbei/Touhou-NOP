
// ItemSystem.cpp
#include "stdafx.h"


// Some Math Macros
// Just in case, they are not defined
#ifndef PI
#define PI				3.1416f
#endif

#define ANG2RAD(x)		( x * PI / 180.0f )


ItemSystem *g_ItemSys = ItemSystem::Init("Textures/");


// Initialize Static Members
ItemSystem *ItemSystem::mpInst = NULL;


// tItem's functions
void ItemSystem::tItem::Render(Texture& texman)
{
	glPushMatrix();
	glTranslatef(vecPosition.x, vecPosition.y, 0); 
	texman.DrawXY(0, base->frame, base->width);
	glPopMatrix();
}


// ItemSystem's functions
ItemSystem* ItemSystem::Init(const std::string& tdir)
{
	if( mpInst == NULL )	return new ItemSystem(tdir);
	else					return NULL;
}

void ItemSystem::Deinit(ItemSystem*& what)
{
	if( what == mpInst && mpInst != NULL )
		delete mpInst;

	what = NULL;
}


void ItemSystem::Register(lua_State& L)
{
	// Register script functions
	lua_register(&L, "AddItem", Lua_Add);
	lua_register(&L, "SetItemThrow", Lua_SetThrow);
	lua_register(&L, "SetItemDropAmount", Lua_SetAmount);
	lua_register(&L, "SetItemTexture", Lua_SetTexture);
	lua_register(&L, "SetItemMagnet", Lua_SetMagnetic);

	// Register ItemType globals
	lua_pushnumber(&L, NORMAL_ITEM); lua_setglobal(&L, "NORMAL_ITEM");
	lua_pushnumber(&L, BONUS_ITEM); lua_setglobal(&L, "BONUS_ITEM");
	lua_pushnumber(&L, SPECIAL_ITEM); lua_setglobal(&L, "SPECIAL_ITEM");

	// Register DropType globals
	lua_pushnumber(&L, ENEMY_DROP); lua_setglobal(&L, "ENEMY_DROP");
	lua_pushnumber(&L, BOSS_DROP); lua_setglobal(&L, "BOSS_DROP");
	lua_pushnumber(&L, PLAYER_DROP); lua_setglobal(&L, "PLAYER_DROP");
}

void ItemSystem::Reset()
{
	while( mLocked ) {};
	mLocked = true;

	mTexManager.ClearAllTextures();

	{
		// Delete all item definitions
		std::vector< ItemDef* >::iterator it = mItemList.begin();
		std::vector< ItemDef* >::iterator end = mItemList.end();

		for( ; it != end; ++it )
			delete (*it);
	}

	{
		// Delete all item drops
		std::list< tItem* >::iterator it = mDropList.begin();
		std::list< tItem* >::iterator end = mDropList.end();

		for( ; it != end; ++it )
			delete (*it);
	}

	mItemList.clear();
	mDropList.clear();

	mLocked = false;

	mMagnet = false;
	mAbsorb = false;

	mEnemyDropAmount = 0;
	mBossDropAmount = 0;
	mPlayerDropAmount = 0;

	mDropVariation = 1;
	mMaxXMove = 1;
	mMaxYMove = 1;

	mGravity = -10;
	mMagneticLine = 1000;
	mAbsorbSpd = 10;
}


void ItemSystem::Step(GamePlay& gp, CObject& player)
{
	if( g_InGameStatus == PAUSE )
		return;

	// Since if player has just come up the magnetic line, so we start absorbing items
	if( player.GetPosition().y > mMagneticLine && !mMagnet )		mAbsorb = true;
	else															mAbsorb = false;

	// But if the player continues to remain on the line, it no longer is absorbing items
	if( player.GetPosition().y <= mMagneticLine )					mMagnet = false;
	else															mMagnet = true;


	while( mLocked ) {};
	mLocked = true;

	std::list< tItem* >::iterator it = mDropList.begin();
	std::list< tItem* >::iterator end = mDropList.end();

	while(it != end)
	{
		tItem* im = *it;

		// Check if we need to absorb this object
		if( im->absorb || mAbsorb )
		{
			im->absorb = true;
			im->SetVelocity( Vector() );

			// We normalize our vector, because a change in player position will not affect the movement towards the player
			im->SetPosition( im->GetPosition() + (player.GetPosition() - im->GetPosition()).Normalize() * mAbsorbSpd );
			im->UpdateVectors();
		}
		else
		{
			// Don't let our step go above 180, because it is the angle
			// in our equation where it will move down at max speed
			if( ++im->step > 180 )
				im->step = 180;

			// Move our item in a fireworks sort of fashion
			// We directly affect position because it is too
			// troublesome to calculate via velocity
			float x = im->GetPosition().x + im->scale.x;
			float y = im->GetPosition().y + cos( ANG2RAD(im->step) ) * im->scale.y;
			im->SetPosition( Vector(x, y, 0) );
			im->UpdateVectors();
		}


		// Increment our player's attributes
		bool collided = Col_ObjToObj(*im, player);
		if( collided )
		{
			gp.m_iLife += im->base->life;
			gp.m_iBomb += im->base->spell;
			gp.m_iScore += im->base->score;
			gp.m_iPowerLevel += im->base->power;
			g_AEngine->Set3DSoundPos(GETITEM, im->GetPosition().Normalize());
			g_AEngine->PlaySegment(GETITEM, 0);
		}

		// Delete our item if it goes out of our boundary
		if( im->GetPosition().y > ITEM_TOP || im->GetPosition().y < ITEM_BOTTOM
			|| im->GetPosition().x < ITEM_LEFT || im->GetPosition().x > ITEM_RIGHT || collided )
		{
			delete im;
			mDropList.erase(it++);
			continue;
		}

		++it;
	}

	mLocked = false;
}

void ItemSystem::Render()
{
	while( mLocked ) {};
	mLocked = true;

	std::list< tItem* >::iterator it = mDropList.begin();
	std::list< tItem* >::iterator end = mDropList.end();

	glColor4f(1, 1, 1, 1);

	for( ; it != end; ++it )
		(*it)->Render(mTexManager);

	mLocked = false;
}


void ItemSystem::DropEnemy(const Vector& pos)
{
	// No items to poll, prevent endless loop
	if( mItemList.empty() )		return;
	int total = mEnemyDropAmount + (rand() % mDropVariation - mDropVariation / 2);

	while( mLocked ) {};
	mLocked = true;

	for( int i=0; i<total; ++i )
		mDropList.push_back( new tItem(PollDrop(ENEMY_DROP), Vector(0, mGravity, 0), PollScale(), pos) );

	mLocked = false;
}

void ItemSystem::DropBoss(const Vector& pos)
{
	// No items to poll, prevent endless loop
	if( mItemList.empty() )		return;
	int total = mBossDropAmount + (rand() % mDropVariation - mDropVariation / 2);

	while( mLocked ) {};
	mLocked = true;

	for( int i=0; i<total; ++i )
		mDropList.push_back( new tItem(PollDrop(BOSS_DROP), Vector(0, mGravity, 0), PollScale(), pos) );

	mLocked = false;
}


void ItemSystem::DropPlayer(const Vector& pos, GamePlay& gp)
{
	// No items to poll, prevent endless loop
	if( mItemList.empty() )		return;
	int total = mPlayerDropAmount + (rand() % mDropVariation - mDropVariation / 2);

	while( mLocked ) {};
	mLocked = true;

	for( int i=0; i<total; ++i )
	{
		// Check if our player's attributes would allow selected item to drop
		// But we only check if the drop is not a bonus item. Bonus items are free.
		const ItemDef *im = PollDrop(PLAYER_DROP);
		if( im->itemType != BONUS_ITEM && gp.m_iScore >= im->score
			&& gp.m_iPowerLevel >= im->power && gp.m_iBomb >= im->spell )
		{
			gp.m_iBomb -= im->spell; gp.m_iScore -= im->score; gp.m_iPowerLevel -= static_cast<int>(im->power);
			mDropList.push_back( new tItem(im, Vector(0, mGravity, 0), PollScale(), pos) );
		}
		else if( im->itemType == BONUS_ITEM )
		{
			mDropList.push_back( new tItem(im, Vector(0, mGravity, 0), PollScale(), pos) );
		}
		else
		{
			--i;
		}
	}

	mLocked = false;
}


ItemSystem::ItemSystem(const std::string& tdir) : mTexDir(tdir), mLocked(false)
{
	mpInst = this;
}

ItemSystem::~ItemSystem()
{
	mpInst = NULL;

	{
		// Delete all item definitions
		std::vector< ItemDef* >::iterator it = mItemList.begin();
		std::vector< ItemDef* >::iterator end = mItemList.end();

		for( ; it != end; ++it )
			delete (*it);
	}

	{
		// Delete all item drops
		std::list< tItem* >::iterator it = mDropList.begin();
		std::list< tItem* >::iterator end = mDropList.end();

		for( ; it != end; ++it )
			delete (*it);
	}
}


int ItemSystem::Lua_Add(lua_State* L)
{
	// Add an item definition
	ItemDef *im = new ItemDef( (int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), (float)lua_tonumber(L, 3),
		(int)lua_tonumber(L, 4), (int)lua_tonumber(L, 5), (int)lua_tonumber(L, 6), (float)lua_tonumber(L, 7),
		(int)lua_tonumber(L, 8), (float)lua_tonumber(L, 9), (float)lua_tonumber(L, 10), (float)lua_tonumber(L, 11) );

	// Error checking just in case
	if( im->dropRate <= 0 )		im->dropRate = 0;
	if( im->score < 0 )			im->score = 0;
	if( im->life < 0 )			im->life = 0;
	if( im->spell < 0 )			im->spell = 0;
	if( im->power < 0 )			im->power = 0;
	if( im->frame < 0 )			im->frame = 0;
	if( im->width < 0 )			im->width = 0;
	if( im->hitbox < 0 )		im->hitbox = 0;
	if( im->mass < 0 )			im->mass = 0;

	mpInst->mItemList.push_back(im);
	return 0;
}

int ItemSystem::Lua_SetThrow(lua_State* L)
{
	mpInst->mMaxXMove = (float)lua_tonumber(L, 1);
	mpInst->mMaxYMove = (float)lua_tonumber(L, 2);
	mpInst->mGravity = (float)lua_tonumber(L, 3);

	// Safely ignore to prevent errors
	if( mpInst->mMaxXMove < 1 )			mpInst->mMaxXMove = 1;
	if( mpInst->mMaxYMove < 1 )			mpInst->mMaxYMove = 1;
	if( mpInst->mGravity > -1 )			mpInst->mGravity = -1;
	return 0;
}

int ItemSystem::Lua_SetAmount(lua_State* L)
{
	mpInst->mEnemyDropAmount = (int)lua_tonumber(L, 1);
	mpInst->mBossDropAmount = (int)lua_tonumber(L, 2);
	mpInst->mPlayerDropAmount = (int)lua_tonumber(L, 3);
	mpInst->mDropVariation = (int)lua_tonumber(L, 4);

	// Safely ignore to prevent errors
	if( mpInst->mEnemyDropAmount < 0 )		mpInst->mEnemyDropAmount = 0;
	if( mpInst->mBossDropAmount < 0 )		mpInst->mBossDropAmount = 0;
	if( mpInst->mPlayerDropAmount < 0 )		mpInst->mPlayerDropAmount = 0;
	if( mpInst->mDropVariation < 1 )		mpInst->mDropVariation = 1;
	return 0;
}

int ItemSystem::Lua_SetTexture(lua_State* L)
{
	// Add texture to list
	mpInst->mTexManager.ClearAllTextures();
	std::string path = mpInst->mTexDir + lua_tostring(L, 1);
	Texture &t = mpInst->mTexManager;

	// Check if texture exists
	FILE *fin = fopen(path.c_str(), "rb");
	if( fin == NULL )
		return 0;

	fclose(fin); // Clear our pointer
	t.AddTexture(path, false); // Load our texture
	return 0;
}

int ItemSystem::Lua_SetMagnetic(lua_State* L)
{
	mpInst->mMagneticLine = (int)lua_tonumber(L, 1);
	mpInst->mAbsorbSpd = (float)lua_tonumber(L, 2);

	// Safely ignore to prevent errors
	if( mpInst->mAbsorbSpd < 1 )			mpInst->mAbsorbSpd = 1;
	return 0;
}


const ItemDef* ItemSystem::PollDrop(const DropType& dt)
{
	for(;;)
	{
		ItemDef *im = mItemList[ rand() % mItemList.size() ];

		// Don't allow a special item to appear twice onscreen at anytime
		if( im->itemType == SPECIAL_ITEM )
		{
			std::list< tItem* >::iterator it = mDropList.begin();
			std::list< tItem* >::iterator end = mDropList.end();
			bool repeat = false;

			for( ; it != end; ++it )
				if( (*it)->base->itemType == SPECIAL_ITEM )
					repeat = true;

			if( repeat )		continue;
		}

		// Boss & Player drop
		if(	 ( rand() % 1000000 < im->dropRate * 10000 && im->dropType == dt && (dt == BOSS_DROP || dt == PLAYER_DROP) )
			// Enemy drop
			|| ( rand() % 1000000 < im->dropRate * 10000 && im->dropType == ENEMY_DROP && (dt == ENEMY_DROP || dt == BOSS_DROP) )  )
			return im;
	}
}

const Vector ItemSystem::PollScale()
{
	float x = (rand() % ((int)mMaxXMove * 1000) - (mMaxXMove * 500)) / 1000.0f;
	float y = (rand() % ((int)mMaxYMove * 1000) + (mMaxYMove * 1000)) / 1000.0f;
	return Vector(x, y, 0);
}
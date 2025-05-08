
// FlyIn.cpp
#include "stdafx.h"
#include "Dialog.h"
#include "FlyIn.h"


// Lua Globals
#define FLYIN_PLAYER	0
#define FLYIN_ENEMY		1
#define FLYIN_NONE		0
#define FLYIN_MOVE		1

extern bool g_disableKeyboard;

// Initialize Static Members
FlyInManager *FlyInManager::mpInst = NULL;


FlyInManager* FlyInManager::Init(Texture& texman)
{
	if( mpInst == NULL )	return new FlyInManager(texman);
	else					return NULL;
}

void FlyInManager::Deinit(FlyInManager*& what)
{
	if( what == mpInst && mpInst != NULL )
		delete mpInst;

	what = NULL;
}


void FlyInManager::RegisterLua(lua_State& L)
{
	// Register script functions
	lua_register(&L, "SetFlyIn", Lua_Set);
	lua_register(&L, "LaunchFlyIn", Lua_Launch);
	lua_register(&L, "StopAllFlyIn", Lua_StopAll);

	// Register flyin global variables
	lua_pushnumber(&L, FLYIN_PLAYER);
	lua_setglobal(&L, "FLYIN_PLAYER");

	lua_pushnumber(&L, FLYIN_ENEMY);
	lua_setglobal(&L, "FLYIN_ENEMY");

	lua_pushnumber(&L, FLYIN_NONE);
	lua_setglobal(&L, "FLYIN_NONE");

	lua_pushnumber(&L, FLYIN_MOVE);
	lua_setglobal(&L, "FLYIN_MOVE");
}

void FlyInManager::Reset()
{
	mPlayer.id = 0;
	mPlayer.life = 0.0f;
	mPlayer.pos.x = 0.0f;
	mPlayer.pos.y = 0.0f;

	mEnemy.id = 0;
	mEnemy.life = 0.0f;
	mEnemy.pos.x = 0.0f;
	mEnemy.pos.y = 0.0f;
}


void FlyInManager::Render()
{
	if( mPlayer.life > 0.0f )
	{
		if( mPlayer.move )
		{
			mPlayer.pos.x += FLYIN_PLAYER_SPEED_X * g_TimeManager->GetInterval();
			mPlayer.pos.y += FLYIN_PLAYER_SPEED_Y * g_TimeManager->GetInterval();
		}

		mPlayer.life -= FLYIN_PLAYER_LIFE;

		glPushMatrix();
		glTranslatef(mPlayer.pos.x, mPlayer.pos.y, 0);
		glColor4f(1.0f, 1.0f, 1.0f, mPlayer.life);
		mTexManager.DrawXY(mPlayer.id);
		glPopMatrix();
	}
	if( mEnemy.life > 0.0f )
	{
		if( mEnemy.move )
		{
			mEnemy.pos.x += FLYIN_ENEMY_SPEED_X * g_TimeManager->GetInterval();
			mEnemy.pos.y += FLYIN_ENEMY_SPEED_Y * g_TimeManager->GetInterval();
		}

		mEnemy.life -= FLYIN_ENEMY_LIFE;

		glPushMatrix();
		glTranslatef(mEnemy.pos.x, mEnemy.pos.y, 0);
		glColor4f(1.0f, 1.0f, 1.0f, mEnemy.life);
		mTexManager.DrawXY(mEnemy.id);
		glPopMatrix();
	}
}

void FlyInManager::Launch(const bool move)
{
	mPlayer.life = 1.0f;
	mPlayer.pos.x = (float)FLYIN_PLAYER_X + mpInst->mTexManager[ mpInst->mPlayer.id ]->width / 2.0f;
	mPlayer.pos.y = (float)FLYIN_PLAYER_Y + mpInst->mTexManager[ mpInst->mPlayer.id ]->height / 2.0f;
	mPlayer.move = move;
}

void FlyInManager::StopAll()
{
	mPlayer.life = 0.0f;
	mEnemy.life = 0.0f;
}


FlyInManager::FlyInManager(Texture& texman)
	:	mTexManager(texman)
{
	mpInst = this;
	Reset();
}

FlyInManager::~FlyInManager()
{
	mpInst = NULL;
}


int FlyInManager::Lua_Set(lua_State* L)
{
	// Fly in on player's side
	if( FLYIN_PLAYER == (int)lua_tonumber(L, 1) )
		mpInst->mPlayer.id = (int)lua_tonumber(L, 2);

	// Fly in on enemy's side
	else if( FLYIN_ENEMY == (int)lua_tonumber(L, 1) )
		mpInst->mEnemy.id = (int)lua_tonumber(L, 2);

	return 0;
}

int FlyInManager::Lua_Launch(lua_State* L)
{
	// Fly in on player's side
	if( FLYIN_PLAYER == (int)lua_tonumber(L, 1) )
	{
		mpInst->Launch( ((int)lua_tonumber(L, 2) == FLYIN_NONE ? false : true) );
	}

	// Fly in on enemy's side
	else if( FLYIN_ENEMY == (int)lua_tonumber(L, 1) )
	{
		mpInst->mEnemy.life = 1.0f;
		mpInst->mEnemy.pos.x = (float)FLYIN_ENEMY_X - mpInst->mTexManager[ mpInst->mEnemy.id ]->width / 2.0f;
		mpInst->mEnemy.pos.y = (float)FLYIN_ENEMY_Y - mpInst->mTexManager[ mpInst->mEnemy.id ]->height / 2.0f;
		mpInst->mEnemy.move = ((int)lua_tonumber(L, 2) == FLYIN_NONE ? false : true);
	}

	return 0;
}

int FlyInManager::Lua_StopAll(lua_State*)
{
	g_disableKeyboard = false;
	mpInst->StopAll();
	return 0;
}
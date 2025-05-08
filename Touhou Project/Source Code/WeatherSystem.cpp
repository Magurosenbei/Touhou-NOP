
// WeatherSystem.cpp
#include "stdafx.h"


// Just in case this is not defined
#ifndef ANG2RAD
#define ANG2RAD(x)		( x * PIE / 180.0f )
#endif


WeatherSystem *g_WeatherSys = WeatherSystem::Init("Textures/");

// Initialize Static Members
WeatherSystem *WeatherSystem::mpInst = NULL;
WeatherSystem *CObject::mpWthSys = NULL;
GLUquadric *WeatherSystem::mpQuadObj = NULL;


WeatherSystem* WeatherSystem::Init(const std::string& texdir)
{
	if( mpInst == NULL )	return new WeatherSystem(texdir);
	else					return NULL;
}

void WeatherSystem::Deinit(WeatherSystem*& what)
{
	if( what == mpInst && mpInst != NULL )
		delete mpInst;

	what = NULL;
}


void WeatherSystem::Register(lua_State& L)
{
	// Register script functions
	lua_register(&L, "SetWeatherAutoChangeTime", Lua_SetAutoTime);
	lua_register(&L, "SetWeatherWindFactor", Lua_SetWindFactor);
	lua_register(&L, "AddWeatherTexture", Lua_AddTexture);
	lua_register(&L, "SetWeatherChange", Lua_SetChange);
	lua_register(&L, "AddWeather", Lua_Add);

	srand(static_cast<unsigned int>(time(NULL)));
}

void WeatherSystem::Reset()
{
	mObjects.clear();
	mAvailIds.clear();
	mWeathers.clear();
	mForecast.clear();

	mTexManager.ClearAllTextures();

	mNextAutoChangeTime = 0;
	mNextSmallChangeTime = 0;
	mNextForcedChangeTime = 0;

	mChangeRate = 100;
	mPowerDeducted = 0;
	mCooldownTime = 0;

	mMinTime = 0;
	mMaxTime = 0;
	mWindFactor = 1;

	Weather w;
	w.wind = 0; w.angle = 0;
	mForecast.push_back(w);

	mLocked = false;
}


void WeatherSystem::Step()
{
	// ignore weather system, because no weathers added
	if( mWeathers.empty() )
		return;

	while( mLocked ) {};
	mLocked = true;

	// Auto change wind if necessary
	ChangeWind();

	Weather &w = mForecast.front();

	// Make a small change our wind sometimes to make it realistic
	// Don't change too often, if not, it looks weird
	if( time(0) > mNextSmallChangeTime && w.wind > 0 )
	{
		w.angle = w.angle + rand() % 2 - 1;
		w.wind = w.wind + rand() % 2 - 1;

		// Wait about 10 seconds until next small change
		mNextSmallChangeTime = time(0) + (rand() % 10 + 10);
	}


	Vector acc = CurrWindAcceleration() * mWindFactor;
	Vector tmpvel;

	// Update our objects
	std::vector< tPhyObj >::iterator it = mObjects.begin();
	std::vector< tPhyObj >::iterator end = mObjects.end();

	for( ; it != end; ++it )
	{
		if( it->obj == NULL )								continue; // Prevent access violation
		if( it->velocity.x > 10 || it->velocity.y > 10 )	continue; // We don't want our wind to move too quickly
		if( it->velocity.x < -10 || it->velocity.y < -10 )	continue; // We don't want our wind to move too quickly
		if( it->mass > 10000 )								continue; // Don't affect stuff with super huge mass

		// Store current velocity
		tmpvel = it->velocity;

		// Apply current wind
		// Wind affects lighter objects more than heavier objects, hence, the inverse mass
		it->velocity += acc * 1.0f / (it->mass + 30);

		// Update our object
		it->obj->SetVelocity(it->obj->GetVelocity() + it->velocity - tmpvel);
	}

	mLocked = false;
}

void WeatherSystem::DebugDraw() const
{
	std::vector< tPhyObj >::const_iterator it = mObjects.begin();
	std::vector< tPhyObj >::const_iterator end = mObjects.end();

	float radius = 0;
	for( ; it != end; ++it )
	{
		// Skip empty slots
		if( it->obj == NULL )		continue;
		radius = it->obj->GetHitboxSize();

		// Begin drawing
		glPushMatrix();
			glTranslatef( it->obj->GetPosition().x, it->obj->GetPosition().y, 0 );
			gluDisk(mpQuadObj, radius-1, radius, 20, 20);
		glPopMatrix();
	}
}


void WeatherSystem::ChangeWind(float& pw)
{
	// ignore weather system, because no weathers added
	if( mWeathers.empty() )
		return;

	// Don't change yet, cooldown time is not up
	if( time(0) < mNextForcedChangeTime || pw < mPowerDeducted )
		return;

	while( mLocked ) {};
	mLocked = true;

	// Force a wind change
	ChangeWind(true);

	// Penalty no matter what
	pw -= mPowerDeducted;
	mNextForcedChangeTime = time(0) + mCooldownTime;

	mLocked = false;
}


bool WeatherSystem::IsWeather() const
{
	return ( GetCurrentWeather().texId == -1 );
}

const Weather& WeatherSystem::GetCurrentWeather() const
{
	return mForecast.front();
}

const std::deque< Weather >& WeatherSystem::GetWeatherList() const
{
	return mForecast;
}


WeatherSystem::WeatherSystem(const std::string& texdir) : mTexDir(texdir)
{
	mpInst = CObject::mpWthSys = this;
	mpQuadObj = gluNewQuadric();
	Reset();
}

WeatherSystem::~WeatherSystem()
{
	mpInst = CObject::mpWthSys = NULL;
	gluDeleteQuadric(mpQuadObj);
}


void WeatherSystem::BuildWeather()
{
	// Clear our current list
	mForecast.clear();

	// First weather should be zero to prevent errors
	Weather w = mWeathers[ rand() % mWeathers.size() ];
	w.wind = 0; w.angle = 0;
	mForecast.push_back(w);

	// Create a new weather list
	for( int i=1; i<3; ++i )
	{
		Weather w = mWeathers[ rand() % mWeathers.size() ];
		w.wind = ( rand() % (w.maxWind - w.minWind + 1) ) + w.minWind;
		w.angle = rand() % 360;
		mForecast.push_back(w);
	}
}

void WeatherSystem::ChangeWind(const bool& forced)
{
	bool change = false;

	// Check our time if wind change is not forced
	// Take a chance and poll a value if forced
	if( !forced )		change = (time(0) > mNextAutoChangeTime ? true : false);
	else				change = (rand() % 100 < mChangeRate ? true : false);

	if( !change )		return;

	// Change wind
	mForecast.pop_front();

	// Get a random weather from our predefined weather list
	Weather w = mWeathers[ rand() % mWeathers.size() ];
	w.wind = ( rand() % (w.maxWind - w.minWind + 1) ) + w.minWind;
	w.angle = rand() % 360;
	mForecast.push_back(w);

	// Get next auto change time
	mNextAutoChangeTime = time(0) + ( rand() % (mMaxTime - mMinTime + 1) ) + mMinTime;

	// Reset our objects
	std::vector< tPhyObj >::iterator it = mObjects.begin();
	std::vector< tPhyObj >::iterator end = mObjects.end();

	for( ; it != end; ++it )
	{
		// Skip empty slots
		if( it->obj == NULL )		continue;

		it->velocity = Vector();
	}
}

Weather& WeatherSystem::CurrWeather()
{
	return mForecast.front();
}

Vector WeatherSystem::CurrWindAcceleration()
{
	Weather &w = CurrWeather();
	return Vector(cos( ANG2RAD(w.angle) ), sin( ANG2RAD(w.angle) ), 0).Normalize() * (float)w.wind;
}


int WeatherSystem::Lua_SetWindFactor(lua_State* L)
{
	mpInst->mWindFactor = (float)lua_tonumber(L, 1);
	if( mpInst->mWindFactor < 0 )	mpInst->mWindFactor = 1;
	return 0;
}

int WeatherSystem::Lua_AddTexture(lua_State* L)
{
	// Add texture to list
	std::string path = mpInst->mTexDir + lua_tostring(L, 1);
	Texture &t = mpInst->mTexManager;

	// Check if texture exists
	FILE *fin = fopen(path.c_str(), "rb");
	if( fin == NULL )
		return 0;

	fclose(fin); // Clear our pointer

	// Load a "null" texture if this is the first texture to be loaded
	// We are only using this slot for error checks
	if( t.Size() == 0 )		t.AddTexture(path, false);
	int x = (int)t.AddTexture(path, false);

	// Return texture id
	lua_pushnumber(L, x);
	return 1;
}

int WeatherSystem::Lua_SetAutoTime(lua_State* L)
{
	mpInst->mMinTime = (time_t)lua_tonumber(L, 1);
	mpInst->mMaxTime = (time_t)lua_tonumber(L, 2);
	if( mpInst->mMaxTime < mpInst->mMinTime )	mpInst->mMaxTime = mpInst->mMinTime;
	return 0;
}

int WeatherSystem::Lua_Add(lua_State* L)
{
	Weather w( (int)lua_tonumber(L, 1), mpInst->mTexManager );
	w.maxWind = (int)lua_tonumber(L, 2);
	w.minWind = (int)lua_tonumber(L, 3);

	// Non-existent texture requested
	if( w.texId <= 0 || w.texId >= mpInst->mTexManager.Size() )
		return 0;

	// Safely ignore weird values
	if( w.maxWind < w.minWind )		w.maxWind = w.minWind;
	if( w.maxWind < 0 )				w.maxWind = 0;
	if( w.minWind < 0 )				w.minWind = 0;

	mpInst->mWeathers.push_back(w);
	mpInst->BuildWeather();
	return 0;
}

int WeatherSystem::Lua_SetChange(lua_State* L)
{
	mpInst->mChangeRate = (int)lua_tonumber(L, 1);
	mpInst->mPowerDeducted = (float)lua_tonumber(L, 2);
	mpInst->mCooldownTime = (time_t)lua_tonumber(L, 3);

	// Safely ignore weird values
	if( mpInst->mChangeRate < 0 )		mpInst->mChangeRate = 100;
	if( mpInst->mPowerDeducted < 0 )	mpInst->mPowerDeducted = 0;
	if( mpInst->mCooldownTime > 600 )	mpInst->mCooldownTime = 0;
	return 0;
}


int WeatherSystem::Subscribe(CObject* obj)
{
	// In case some idiot tries to pass in a NULL object
	if( obj == NULL )		return -1;

	while( mLocked ) {};
	mLocked = true;

	int index = 0;

	// Fill in at the end, because there are no available slots
	if( mAvailIds.empty() )
	{
		mObjects.push_back( tPhyObj(obj, obj->GetHitboxSize()) );
		index = (int)mObjects.size() - 1;
	}

	// Fill up the available slots first
	else
	{
		index = mAvailIds.back();
		mObjects[ index ] = tPhyObj(obj, obj->GetHitboxSize());
		mAvailIds.pop_back();
	}

	mLocked = false;
	return index;
}

void WeatherSystem::Unsubscribe(const int& id)
{
	// Error check, just in case
	if( id < 0 || id >= (int)mObjects.size() || mObjects[id].obj == NULL )
		return;

	while( mLocked ) {};
	mLocked = true;

	// Fill our current slot as NULL
	// We don't delete because it will collapse all IDs after our current by 1
	mObjects[id].obj = NULL;
	mAvailIds.push_back(id);

	mLocked = false;
}

void WeatherSystem::Reinit(CObject* obj, const int& id)
{
	// Error check, just in case
	if( id < 0 || id >= (int)mObjects.size() || mObjects[id].obj == NULL )
		return;

	mObjects[id].obj = obj;
	mObjects[id].mass = mObjects[id].obj->GetHitboxSize();
	mObjects[id].velocity = Vector();
}


void WeatherSystem::SetMass(const int& phyid, const float& mass)
{
	// Error check, just in case
	if( phyid >= 0 && phyid < (int)mObjects.size() && mObjects[phyid].obj != NULL )
		mObjects[phyid].mass = mass;
}
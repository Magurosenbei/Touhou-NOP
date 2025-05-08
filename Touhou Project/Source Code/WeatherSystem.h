
// WeatherSystem.h
#ifndef _WEATHER_SYSTEM_H_
#define _WEATHER_SYSTEM_H_


#include <vector>


struct Weather
{
public:

	int wind; // wind power
	int angle; // treats the postive x-axis as zero, counter-clockwise direction

	const Texture* const texMan; // weather texture manager
	const int texId; // weather texture


	// Methods
	Weather() : texId(-1), texMan(NULL) {}
	Weather(const int& id, const Texture& tex) : texId(id), texMan(&tex) {}
	Weather(const Weather& w)
		: wind(w.wind), angle(w.angle), texMan(w.texMan),
		texId(w.texId), maxWind(w.maxWind), minWind(w.minWind) {}

	const Weather& operator=(const Weather& w)
	{
		if( this != &w )
		{
			wind = w.wind;
			angle = w.angle;
			maxWind = w.maxWind;
			minWind = w.minWind;
		}
		return *this;
	}

private:

	friend class WeatherSystem;
	int maxWind, minWind;
};


class CObject;

// Creates a weather system that stimulate the wind
// conditions for all CObjects. Singleton method.
// It only stores a copy of CObject.

// Note: It is important that all objects are pointers,
// especially if you use an STL container, because
// there are no copy constructors or assignment
// operators for CObject.
class WeatherSystem
{
public:

	// Friends
	friend class CObject;

	// Initialize
	static WeatherSystem* Init(const std::string& texdir);
	static void Deinit(WeatherSystem*& what);

	// Register
	void Register(lua_State& L);
	void Reset();

	// Render
	void Step(); // move our physics objects
	void DebugDraw() const; // draws based on hitbox size

	// Input
	void ChangeWind(float& pw); // deducts power as well

	// Accessor
	bool IsWeather() const; // check if weather system was initialized
	const Weather& GetCurrentWeather() const;
	const std::deque< Weather >& GetWeatherList() const;


private:

	// Locked
	WeatherSystem(const std::string& texdir);
	~WeatherSystem();
	WeatherSystem(const WeatherSystem&);
	const WeatherSystem& operator=(const WeatherSystem&);

	// Internals
	void BuildWeather();
	void ChangeWind(const bool& forced = false);

	Weather& CurrWeather();
	Vector CurrWindAcceleration();


	// Lua Callbacks
	static int Lua_SetWindFactor(lua_State* L); // Factor
	static int Lua_AddTexture(lua_State* L); // Weather HUD image (Returns weather type)
	static int Lua_SetAutoTime(lua_State* L); // Max time, min time
	static int Lua_Add(lua_State* L); // Weather type, max wind, min wind
	static int Lua_SetChange(lua_State* L); // Change rate, power deducted, cooldown time


	// CObject Callbacks
	int Subscribe(CObject* obj);
	void Unsubscribe(const int& id);
	void Reinit(CObject* obj, const int& id);

	// Mass data is already set by default to hitbox size
	void SetMass(const int& phyid, const float& mass);


private:

	// Template for Physics Objects
	struct tPhyObj
	{
		tPhyObj(CObject* obj, const float& mass) : obj(obj), mass(mass) {}
		tPhyObj() : obj(NULL), mass(0) {}

		float mass;
		CObject* obj;
		Vector velocity; // we use this so that we can factor off the wind velocity from the object on wind change
	};


	// Static Members
	static WeatherSystem *mpInst;
	static GLUquadric *mpQuadObj;

	// Physics
	std::vector< tPhyObj > mObjects;
	std::vector< int > mAvailIds;

	// Weather
	Texture mTexManager;
	const std::string mTexDir;
	std::vector< Weather > mWeathers;
	std::deque< Weather > mForecast;

	// Timers
	time_t mNextAutoChangeTime;
	time_t mNextSmallChangeTime;
	time_t mNextForcedChangeTime;

	// Attributes
	int mChangeRate;
	float mPowerDeducted;
	time_t mCooldownTime;

	time_t mMinTime;
	time_t mMaxTime;
	float mWindFactor;

	// Thread
	bool mLocked;
};

extern WeatherSystem *g_WeatherSys;

#endif
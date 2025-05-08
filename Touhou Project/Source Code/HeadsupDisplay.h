#pragma once

#include "enemy.h"

#define ChangeSpeed 30
#define ChangeBackground 3200
#define Life 1000
#define FullBar 300

enum WeatherState
{
	Calm,
	CalmtoWindy,
	Windy,
	WindytoStorm,
	Storm,
	StormtoCalm,
	CalmtoStorm,
	WindytoCalm,
	StormtoWindy
};
class HeadsupDisplay: public Singleton <HeadsupDisplay>
{
	int* highscore , counter;
	// just counters
	int Arrow, Noarrow, Compass, LifeStar, Weathersprites;
	int volume, lifebar, mainpic ,movable;
	// texture holders
	float C_Length;
	bool isplaying,symbol,switchpic;
	WeatherState status;
	Enemy * boss;
public:
	HeadsupDisplay();
	bool Init();
	// Initializing
	void DisplayHighScore();
	void DisplayScore();
	void ShowBombLeft();
	void ShowLifeLeft();
	void ShowPowerLevel();
	void DisplayGrazeCounter();
	void RenderCompass(bool ,float, int, int);
	void RenderVolumeControl();
	void RenderLifeBar(string);
	void RenderBackground();
	// Rendering of HUD
	void DisplayAll();
	void ChangeWeather(int);
	void DisplayWeatherStatus(string);
	void UpdateLifebar();
	void Getboss(Enemy *);
	void RecieveImage(int , int);
	//Assist/Update Functions
	int StringToInt(string);
};
extern HeadsupDisplay * gHUD;	// this is to allow text printing anywhere.

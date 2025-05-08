#pragma once

/* T
his class computes game play, 
may modify to suit scripting systems
*/

#define Max_Power	130		// maybe script this is better?
#define Max_Bomb	5		
#define Shoot_Score 10
#define Graze_Score 1

#define BIG true
#define SMALL false

#include "singleton.h"

class GamePlay : public Singleton <GamePlay>
{
	friend class WeaponScript;
	friend class ItemSystem;
	int		m_iLife;		// number of life default 3
	float	m_iPowerLevel;	// amount of power / increase attack type or change dmg
	int		m_iPowerStage;
	bool	m_bPowerAdd;

	int		m_iBomb;		// number of bombs
	int		m_iScore;		// Total Score show at the top
	int		m_iGraze;		// Number of close encounters
	float	m_iGrazeRadius;	// GrazeRaidus
	
	bool	m_bBomb;		// is bomb activated?
	bool	m_bGodMode;		// God Mode during Bomb etc...
	bool	m_bGameOver;	// No lifes?

	int		m_iGodTimer;	//	Counter for God
	int		m_iBombTimer;	//	Counter for Bomb

protected:
	int		d_Life;			// Default number of life
	int		d_Bomb;			// Default number of Bomb

	int		m_Life_Plus;		// Increase number of Life per Life up
	int		m_Bomb_Plus;		// Increase number of Bomb per Bomb up
	int		m_Points_Plus;		// Increase number of score per Point up
	float	m_Small_Power_Plus;	// Increase number of power per Small Power up
	int		m_Big_Power_Plus;	// Increase number of power per Big Power up
	int		m_Bomb_Timer;		// Time lasting for Bombing	
	int		m_God_Mode_Timer;	// Stay in God Mode for how long
	int		m_PowerStage;		// Default Weapon Level
	float	m_Graze_Radius;		// Points earned if Player dodge Bullets in close distance

	vector <string> CharaPathList;

public:
	GamePlay();
	GamePlay(GamePlay &);
	~GamePlay();

	// Basic Get functions
	int GetLife() const;
	float GetPower() const;
	int GetPowerStage() const;
	int GetGraze() const;
	int GetScore() const;
	int GetBomb() const;
	float GetGrazeRadius() const;

	bool IsBomb() const;
	bool IsGod() const;		// when not god mode or something return false
	bool GameOver() const;	// not ended will return false

	int CalculateScore(bool EndGame) const;	// calculates the total score

	void UpdateCounters();
	// Updates if event happens
	void AddPower(bool Size);			// depending on size of object, increase certain amount of Power	
	void AddBomb();						// increase bomb count  by 1
	void AddLife();						// increase life by 1
	bool UseBomb();						// Enable GodMode and reduce bomb by 1
	void AddScore();					// increase score by each hit on enemy
	void AddPointScore();				// increase Score by point item dropped - scripted
	void AddBonus(int Bonus);			// increase Score by spell bonus (play succeed the bullet pattern
	void AddGraze();				// increase Score through Graze Radius
	void SetPower(float & Pw);
	void BonusGrace();					// increase Score if Bullets within Grace
	void Die();							//	Decrease Life
	void GodMode(bool x)
	{
		m_bGodMode = x;
	}


	void FData();

	void PushPlayerPath(string Path);
	string GetCharacterPath(int index);
	void Read(const char * Path);
	static void Register(lua_State * State);

	void Reset();

};

extern GamePlay * g_GameManager;
extern time_t g_diePause;
extern bool g_die;
extern int MyScore;
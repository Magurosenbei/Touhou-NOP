#include "stdafx.h"
#include "GamePlay.h"
#include "player.h"
#include "RtScript.h"

GamePlay * g_GameManager;
time_t g_diePause = 0;
bool g_die = false;
int MyScore = 0;

GamePlay :: GamePlay()
{
	m_bPowerAdd = true;
}

GamePlay ::	GamePlay(GamePlay &Copy)
{
	m_iPowerStage = Copy.m_PowerStage;
	m_iLife = Copy.d_Life;
	m_iPowerLevel = 0;
	m_iBomb = Copy.d_Bomb;
	m_iScore = 0;
	m_iGraze = 0;
	m_iGrazeRadius = Copy.m_Graze_Radius;
	m_bGodMode = false;
	m_bGameOver = false;
	m_iGodTimer = Copy.m_Bomb_Timer;
	m_iBombTimer = Copy.m_God_Mode_Timer;
	m_bBomb = false;
	m_bPowerAdd = true;

	CharaPathList = Copy.CharaPathList;
}

GamePlay ::	~GamePlay()
{
}

int GamePlay :: GetLife() const
{
	return m_iLife;
}

float GamePlay :: GetPower() const
{
	return m_iPowerLevel;
}

int GamePlay :: GetPowerStage() const
{
	return m_iPowerStage;
}

int GamePlay :: GetGraze() const
{
	return m_iGraze;
}

int GamePlay :: GetScore() const
{
	return m_iScore;
}

int GamePlay :: GetBomb() const
{
	return m_iBomb;
}

float GamePlay :: GetGrazeRadius() const
{
	return m_iGrazeRadius;
}

void GamePlay :: SetPower(float & Pw)
{
	m_iPowerLevel = Pw;
}
bool GamePlay :: IsBomb() const
{
	return m_bBomb;
}
bool GamePlay :: IsGod() const
{
	return m_bGodMode;
}

bool GamePlay :: GameOver() const
{
	return m_bGameOver;
}

int GamePlay :: CalculateScore(bool EndGame) const
{
	if(EndGame)	// if the game has ended? 
		if(m_iLife > 0)
			return ((m_iScore * 50) + (m_iGraze * 10)) * m_iLife * 100;	//if ended, take life into consideration
	
	return (m_iScore * 50) + (m_iGraze * 10);		// return this if game not end or life = 0;
}

void GamePlay :: UpdateCounters()
{
	if(g_TimeManager->GetFPS() != 0)
		return;
	
	if(m_bBomb)
		(m_iBombTimer < 4)? m_iBombTimer++ : m_bBomb = false;
	if(m_bGodMode)
		(m_iGodTimer < 5)? m_iGodTimer++ : m_bGodMode = false;

	if(m_bBomb == false)
		m_iBombTimer = 0;
	if(m_bGodMode == false)
		m_iGodTimer = 0;

	if(m_iPowerLevel > 4)
		m_iPowerLevel = 4;
	m_iPowerStage = static_cast<int>(m_iPowerLevel);
}

void GamePlay :: AddPower(bool Size)
{
	if(Size)
		m_iPowerLevel += m_Big_Power_Plus;
	else
		m_iPowerLevel += m_Small_Power_Plus;

	if(m_iPowerLevel >= 4)
		m_iPowerLevel = 4;
	
	float(m_iPowerStage) = m_iPowerLevel / 10;
	if(m_iPowerStage < 1)
		m_iPowerStage = 0;
	if(m_iPowerStage < 2)
		m_iPowerStage = 1;
	else if(m_iPowerStage < 6)
		m_iPowerStage = 2;
	else if(m_iPowerStage < 10)
		m_iPowerStage = 3;
	else if(m_iPowerStage < 14)
		m_iPowerStage = 4;
}

void GamePlay :: AddLife()
{
	m_iLife+=m_Life_Plus;
}

void GamePlay :: AddBomb()
{
	(m_iBomb >= Max_Bomb)? m_iBomb = Max_Bomb : m_iBomb+=m_Bomb_Plus;
}
	
bool GamePlay :: UseBomb()
{
	if(m_iBomb > 0 && !m_bBomb)
	{
		if(!g_ScriptReader->m_EnableGOD)
			m_iBomb--;
		m_bBomb = m_bGodMode = true;	
		return true;		// able to bomb
	}
	return false;	// if unable to bomb
}

void GamePlay :: AddScore()
{
	if(m_iScore < 100000000)
		m_iScore += Shoot_Score;
	else
		m_iScore = 99999999;
}

void GamePlay :: AddGraze()
{
	m_iGraze++;
}

void GamePlay :: AddPointScore()
{
	m_iScore += m_Points_Plus;
}

void GamePlay :: AddBonus(int Bonus)
{
	m_iScore += Bonus;
}

void GamePlay :: Die()
{
	/*
	give grace time for player to bomb
	call UseBomb() and bomb-- again.
	*/
	if(!m_bGodMode && !g_ScriptReader->m_EnableGOD)
	{
		if( !g_die )
		{
			g_tmpInGameStatus = g_InGameStatus;
			g_InGameStatus = PAUSE;
			g_diePause = time(0) + 1;
			g_die = true;
		}
		else
		{
			g_ItemSys->DropPlayer(P1->GetPosition(), *this);
			m_iLife--;
			m_bGodMode = true;
			g_AEngine->Set3DSoundPos(DIE, Vector());
			g_AEngine->PlaySegment(DIE, 0);
			P1->Reset();

			g_InGameStatus = g_tmpInGameStatus;
			g_tmpInGameStatus = DEFAULT_INGAMESTATUS;
			g_die = false;
		}
	}
	UpdateCounters();
	//while(m_iLife > 0);
}

void GamePlay :: Read(const char * Path)
{
	lua_State * State = lua_open();
	luabind::open(State);
	luaL_openlibs(State);
	GamePlay :: Register(State);
	luaL_dofile(State, Path);
	lua_close(State);
}

void GamePlay :: FData()
{
	g_GameManager = new GamePlay(*this);
}

void GamePlay :: PushPlayerPath(string Path)
{
	CharaPathList.push_back(Path);
}

string GamePlay :: GetCharacterPath(int index)
{
	if(index < 0)
		return CharaPathList[0];
	else
		return CharaPathList[index];
}

void GamePlay :: Register(lua_State *State)
{
	luabind::module(State) 
	[
		luabind::class_<GamePlay>("GameSystem")
		.def(luabind::constructor<>())							// Constructor
		.def("FinaliseData", &GamePlay::FData)					// Function
		.def("AddPlayerPath", &GamePlay::PushPlayerPath)
		.def_readwrite("DefaultLife", &GamePlay::d_Life)		// Variable
		.def_readwrite("DefaultBomb", &GamePlay::d_Bomb)
		.def_readwrite("LifePlus", &GamePlay::m_Life_Plus)
		.def_readwrite("BombPlus", &GamePlay::m_Bomb_Plus)
		.def_readwrite("PointPlus", &GamePlay::m_Points_Plus)
		.def_readwrite("SmallPowerPlus", &GamePlay::m_Small_Power_Plus)
		.def_readwrite("BigPowerPlus", &GamePlay::m_Big_Power_Plus)

		.def_readwrite("BombTimer", &GamePlay::m_Bomb_Timer)
		.def_readwrite("GodModeTimer", &GamePlay:: m_God_Mode_Timer)

		.def_readwrite("PowerStage", &GamePlay:: m_PowerStage)
		.def_readwrite("GrazeRadius", &GamePlay:: m_Graze_Radius)
	];
}
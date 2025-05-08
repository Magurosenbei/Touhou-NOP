#include "stdafx.h"
#include "RtScript.h"
#include "wave.h"
#include "player.h"
#include "weaponscript.h"
#include "EnemyScript.h"
#include "HeadsupDisplay.h"

RuntimeScript * g_ScriptReader;
bool g_disableKeyboard = false;


RuntimeScript :: RuntimeScript()
{
	m_iLevel = 0;
	m_EnableGOD = false;
}

bool RuntimeScript :: BootUp(void * pParam)
{
	g_disableKeyboard = true;
	pParam;

	State = lua_open();
	luabind::open(State);
	luaL_openlibs(State);
	Vector :: Register(State);
	BulletWave :: Register(State);
	BulletDef :: Register(State);
	EnemyScript :: Register(State);
	EnemyWave :: Register(State);
	g_CutInSystem->RegisterLua(*State);
	g_WeatherSys->Register(*State);
	g_ItemSys->Register(*State);

	luabind::module(State) 
	[
		luabind::def("Wait", &RuntimeScript::Wait)
	];
	luabind::module(State) 
	[
		luabind::def("AddImage", &RuntimeScript::AddImage)
	];
	luabind::module(State) 
	[
		luabind::def("StartBossMode", &RuntimeScript::DeclareBossMode)
	];
	luabind::module(State) 
	[
		luabind::def("StageClear", &RuntimeScript::DeclareClear)
	];
	luabind::module(State) 
	[
		luabind::def("GetPlayerPosition", &RuntimeScript::GetPlayerPosition)
	];
	luabind::module(State)
	[
		luabind::def("PlaySFX", &RuntimeScript::PlaySFX)
	];
	luabind::module(State)
	[
		luabind::def("LoadSFX", &RuntimeScript::LoadSFX)
	];

	luabind::module(State)
	[
		luabind::def("Lock", &RuntimeScript::Lock)
	];

	luabind::module(State)
	[
		luabind::def("UnLock", &RuntimeScript::Unlock)
	];

	luabind::module(State)
	[
		luabind::def("GODMODE_ON", &RuntimeScript::DisableGODMODE)
	];

	luabind::module(State)
	[
		luabind::def("GODMODE_OFF", &RuntimeScript::EnableGODMODE)
	];

	luabind::module(State)
	[
		luabind::def("SwitchBGM", &RuntimeScript::SwitchBGM)
	];

	luabind::module(State)
	[
		luabind::def("CheckGame", &RuntimeScript::ForceBootOff)
	];
	luabind::module(State)
	[
		luabind::def("SetBackground", &RuntimeScript::SetBackground)
	];

	luaL_dofile(State, StagePath[m_iLevel].c_str());
	lua_getglobal(State, "BootUp");
	ASSERT( lua_isfunction(State, -1) , "BootUp() cannot be found!" );
	lua_pcall(State, 0, 0, 0); 
	lua_getglobal(State, "StageMusic");
	ASSERT( lua_isstring(State, -1) , "StageMusic requires a valid media path!" );
	BGMPATH = lua_tostring(State, -1);

	g_AEngine->StopSegment(BGM, true);
	g_AEngine->LoadSegment(BGMPATH, true, BGM);
	LeaveCriticalSection(&_critSection);
	Sleep(10);
	EnterCriticalSection(&_critSection);
	g_AEngine->PlaySegment(BGM, -1);

	return true;
}

void RuntimeScript :: SeparateProcess(void * pParam)
{
	DebugPrint("\nStage Process Sepatated\n");
	DebugPrint("\nWaiting\n");
	Sleep(1000);
	DebugPrint("\nStarted\n");
	pParam;
	luaL_dofile(g_ScriptReader->State, g_ScriptReader->StagePath[g_ScriptReader->m_iLevel].c_str());
	lua_getglobal(g_ScriptReader->State, "Stage");
	
	ASSERT( lua_isfunction(g_ScriptReader->State, -1) , "Stage() cannot be found!" );
	lua_pcall(g_ScriptReader->State, 0, 0, 0);
}

bool RuntimeScript :: BootDown()
{
	g_CutInSystem->Reset();
	g_WeatherSys->Reset();
	g_ItemSys->Reset();
	lua_close(State);
	g_AEngine->StopSegment(BGM, true);
	//mTexManager.ClearAllTextures();
	DebugPrint("\nStage Script Closed\n");
	if(g_GameManager->GetLife() > -1)
	{
		if(++m_iLevel == static_cast<int>(StagePath.size()))
		{
			MyScore = g_GameManager->GetScore();
			g_InGameStatus = COMPLETE;
			g_GameStatus = MENU;
		}
	}
	g_AEngine->SwitchBGMOff();
	return true;
}

void RuntimeScript :: Wait(int milis)
{
	if(g_GameStatus == GAME)
		Sleep(milis);
}

void RuntimeScript :: AddStage(string Path)
{
	g_ScriptReader->StagePath.push_back(Path);
}

string RuntimeScript :: GetCurrentStagePath()
{
	return StagePath[m_iLevel];
}

void RuntimeScript :: ReadScriptDef()
{
	lua_State * State = lua_open();
	luabind::open(State);
	luaL_openlibs(State);
	luabind::module(State) 
	[
		luabind::def("AddStage", &RuntimeScript::AddStage)
	];
	luaL_dofile(State, "Data/Main.th");
	lua_close(State);
}

bool RuntimeScript :: UseStage(int index)
{
	if(index < 0 || index > static_cast<int>(StagePath.size()))
		return false;
	m_iLevel = index;
	return true;
}

int RuntimeScript :: AddImage(string Path)
{
	return g_TexLoader->AddTexture(Path);
}

void RuntimeScript :: DeclareBossMode()
{
	g_InGameStatus = BOSS;
}

void RuntimeScript :: DeclareClear()
{
	g_InGameStatus = CLEAR;
}

Vector RuntimeScript :: GetPlayerPosition()
{
	return P1->GetPosition();
}

void RuntimeScript :: Lock()
{
	EnterCriticalSection(&_critSection);
	DebugPrint("\nLocked\n");
}

void RuntimeScript :: Unlock()
{
	LeaveCriticalSection(&_critSection);
	DebugPrint("\nOpened\n");
}

void RuntimeScript :: PlaySFX(int ID)
{
	g_AEngine->PlaySegment(ID, 0);
	g_AEngine->Set3DSoundPos(ID, Vector());
}

int RuntimeScript :: LoadSFX(string Path)
{
	return g_AEngine->LoadSegment(Path, false);
}

void RuntimeScript :: EnableGODMODE()
{
	g_ScriptReader->m_EnableGOD = true;
}

void RuntimeScript :: DisableGODMODE()
{
	g_ScriptReader->m_EnableGOD = false;
}

void RuntimeScript :: SwitchBGM(string Path)
{
	g_AEngine->SwitchBGMOff();
	g_AEngine->StopSegment(BGM, true);
	EnterCriticalSection(&_critSection);
	g_AEngine->LoadSegment(Path, true, BGM);
	LeaveCriticalSection(&_critSection);
	Sleep(10);
	EnterCriticalSection(&_critSection);
	g_AEngine->PlaySegment(BGM, -1);
	LeaveCriticalSection(&_critSection);

}

bool RuntimeScript :: ForceBootOff()
{
	if(g_GameStatus != GAME)
		return true;
	else
		return false;
}
void RuntimeScript::SetBackground(int Mainpic, int MovingPic)
{
	gHUD->RecieveImage(Mainpic, MovingPic);
}
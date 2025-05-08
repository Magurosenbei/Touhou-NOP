#pragma once

/*
Main Stage 
Process Tree
-----------
DefineScriptPath-- Where  is the file
BootUp			-- Start Up
SeparateProcess	-- Create thread
BootDown		-- Clean up
*/


#include "singleton.h"

class RuntimeScript : public Singleton <RuntimeScript>
{
public:
	lua_State * State;
	string BGMPATH;
	
	int m_iLevel;
	bool m_EnableGOD;
	Texture mTexManager;

	vector <string> StagePath;
//---------------------
	RuntimeScript();
	bool BootUp(void * pParam);					//	Initialize Auto Settles with Level
	bool BootDown();							//	Clean up Same thing
	bool UseStage(int index);					//	Forcefully Select an Practice Stage
	string GetCurrentStagePath();
	void ReadScriptDef();
	
	bool Complete();
	static int AddImage(string Path);
	static void SeparateProcess(void * pParam);
	static void Wait(int milis);
	static void AddStage(string Path);
	static void DeclareBossMode();
	static void DeclareClear();
	static Vector GetPlayerPosition();

	static void Lock();
	static void Unlock();

	static void PlaySFX(int ID);
	static int LoadSFX(string Path);
	static void SwitchBGM(string Path);
	static void EnableGODMODE();
	static void DisableGODMODE();
	static bool ForceBootOff();

	static void SetBackground(int , int);
};

extern RuntimeScript * g_ScriptReader;
extern bool g_disableKeyboard;
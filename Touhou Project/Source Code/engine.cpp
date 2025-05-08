#include "stdafx.h"
#include "engine.h"
#include "player.h"
#include "RtScript.h"
#include "HeadsupDisplay.h"
/*
Implemented Commented parts of Game Status in Render, ProcessInput and Update
*/

CEngine * g_Engine;

CEngine :: CEngine()
{
}

CEngine :: ~CEngine()
{
	EnemyList.clear();
	PlayerBullet.List.clear();
	EnemyBullet.List.clear();
}

bool CEngine :: InitEngine()
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);							//	Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								//	The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	//	Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);				//	Really Nice Point Smoothing

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);									//blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//type of blending
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	glEnable(GL_POINT_SPRITE_ARB);						//POINT_SPRITE_ARB
	float quadratic[] = { 0.01f, 0.01f, 0.00001f };
	glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);

	g_Camera->Position_Camera(	0.0f, 0.0f, -180.0f,				//	eye
								0.0f, 0.0f, 0.0f,				//	view
								0.0f, 1.0f, 0.0f);				//	up

	if(Running)
		return true;
	bool no_error = true;
	g_TexLoader = new Texture();
	g_Weapon = new WeaponScript;
	(no_error ==  g_AEngine->InitDirectXAudio())? no_error : no_error = false;
	g_AEngine->LoadDefaultTracks();
	g_GameMouse->Init();
	g_ScriptReader->ReadScriptDef();
	(no_error ==  g_Interface->Init())? no_error : no_error = false;
	gHUD = &HeadsupDisplay :: Instance();
	gHUD->Init();
	g_GameManager->Read("Data/GameData.th");
	m_Explosion = g_TexLoader->AddTexture("Textures/Character/Explosion.png");
	m_bRendering = false;
	return no_error;
}

void CEngine :: RenderFrame()
{
	m_bRendering = true;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	g_Camera->Look();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	g_Interface->Flatten();	// Anything that is Rasterized in 2D goes between here

#ifdef DEMO_VERSION

	if( g_godMode )
		g_GameManager->GodMode(true);
#endif

	glViewport(-120,0,800,600);
	glPushMatrix();
	glTranslatef(Window_Res_W / 2.0f, Window_Res_H / 2.0f, 1.0f);	// Centralize

	if(g_GameStatus == GAME && g_InGameStatus != CLEAR)	// Draws every Game Object in the list
	{
		gHUD->RenderBackground();
		P1->Render();
		for(list<Bullet *> :: iterator index = PlayerBullet.List.begin(); index != PlayerBullet.List.end(); ++index)
			(*index)->Render();
		
		for(list<Enemy *>	 ::	iterator index = EnemyList.begin(); index != EnemyList.end(); ++index)
		{
			if(!(*index)->CanDelete())
				(*index)->Render();	
			else if ((*index)->ExploCounter < 4)
				RenderExplosion((*index)->GetPosition(),(*index)->ExploCounter);
		}
	
		for(list<Bullet *> :: iterator index = EnemyBullet.List.begin(); index != EnemyBullet.List.end(); ++index)
			(*index)->Render();

		g_ItemSys->Render();
	}
	glViewport(0, 0, Window_Res_W, Window_Res_H);
	if(g_GameStatus == GAME)
	{
		g_CutInSystem->Render();
		g_Interface->HUD();
		gHUD->DisplayAll();
	}
	if(g_GameStatus != GAME)
		g_Interface->RenderBackGround();

	if((g_InGameStatus == LOSE || g_InGameStatus == COMPLETE))
		g_Interface->GameOverScreen();

	if(g_GameStatus == GAME || g_GameStatus == HIGH_SCORE || g_GameStatus == HIGH_SCORE_ENTRY)
	{
		if( g_GameStatus == HIGH_SCORE_ENTRY )
			g_Interface->HighScoreEntry();
		g_Interface->DisplayHighScore();
	}

	if(g_GameStatus == MENU)
		g_Interface->MainSelectionScreen();
	else if(g_InGameStatus == PAUSE && !g_die)
		g_Interface->PauseMenu();

	if((g_GameStatus != GAME || g_InGameStatus == PAUSE)  && !g_die)
		g_GameMouse->Render();

	glPopMatrix();
	m_bRendering = false;
	g_Interface->DeFlatten();	// Returns to normal
	glFlush();
}

void CEngine :: UpdateGame()
{
	// Updates
	if(g_InGameStatus == BOOT)
	{
		P1->Read(g_GameManager->GetCharacterPath(g_Interface->GetSelectedCharacter()).c_str());
		P1->UseWeapon(g_Interface->GetSelectedWeapon());
		g_ScriptReader->BootUp(NULL);			// Init the script
		//_beginthread(&RuntimeScript::SeparateProcess, 0, NULL);
		hMutex[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RuntimeScript::SeparateProcess , NULL, 0, NULL);
		g_InGameStatus = NORMAL_STAGE;
	}
	if(g_InGameStatus == CLEAR)
	{
		g_AEngine->StopSegment(BGM, false);
		g_ScriptReader->BootDown();
		EnemyBullet.List.clear();
		EnemyList.clear();
		if(g_InGameStatus != LOSE && g_InGameStatus != COMPLETE)
			g_InGameStatus = BOOT;
	}
	if(g_GameStatus != GAME || g_InGameStatus == PAUSE)
		g_GameMouse->Update();

	if(g_InGameStatus == NORMAL_STAGE || g_InGameStatus == BOSS || g_CutInSystem->IsDialog())
	{
		P1->Update();
		P1->UpdateVectors();
		UpdateBulletList();

		if(CollideEnemy(*P1))
			g_GameManager->Die();

		if(CollideEnemyBullet(*P1))
			g_GameManager->Die();

		UpdateEnemyBulletList();
		UpdateEnemy();
	}
	g_GameManager->UpdateCounters();

	if(g_GameStatus != GAME)
	{
		TerminateThread(hMutex[0], 0);
		g_ScriptReader->UseStage(0);
		g_CutInSystem->Reset();
		g_WeatherSys->Reset();
		g_ItemSys->Reset();
		PlayerBullet.List.clear();
		EnemyBullet.List.clear();
		EnemyList.clear();
		//if(g_InGameStatus == LOSE || g_InGameStatus == COMPLETE)
		g_GameManager->Read("Data/GameData.th");
	}
	if(g_GameStatus == GAME && g_GameManager->GetLife() < 0)
	{
		g_AEngine->SwitchBGMOff();
		MyScore = g_GameManager->GetScore();
		g_GameStatus = MENU;
		g_InGameStatus = LOSE;
	}
}

void CEngine :: ProcessInput()
{
	g_Interface->GameFinishInPut();
	if(g_GameStatus != GAME || g_InGameStatus == PAUSE)
	{
		g_GameMouse->ProcessInput();
		g_Interface->KeyboardProcessInput();
	}

	if(keys[VK_ESCAPE] && g_GameStatus == GAME)
	{
		(g_InGameStatus != PAUSE)? g_InGameStatus = PAUSE : g_InGameStatus = NORMAL_STAGE;
		keys[VK_ESCAPE] = false;
	}

	g_Interface->ProcessInput();

	if(keys['Z'] && g_CutInSystem->IsDialog())
	{
		g_CutInSystem->NextDialog();
		keys['Z'] = false;
	}
	else if(g_GameStatus == GAME && g_InGameStatus != PAUSE)
		P1->ProcessInput();
}

void CEngine :: UpdateEnemy()
{
	for(list <Enemy *> ::iterator index = EnemyList.begin(); index != EnemyList.end(); ++index)
	{
		if((*index)->ExploCounter < 1 || (*index)->ExploCounter > 3)
		{
			(*index)->UpdateVectors();	// update as usual
			(*index)->Update();
			(*index)->OutOfBound();
		}
	}
}

void CEngine :: UpdateBulletList()
{
	for(list <Bullet *> :: iterator index = PlayerBullet.List.begin(); index != PlayerBullet.List.end(); index++)
	{
		if((*index)->Skip())
			continue;
		if(EnemyList.size() > 0)
		{
			list <Enemy *> :: iterator it = EnemyList.begin();
			while((*it) != EnemyList.back())
			{
				if((*it)->GetHp() > 0 && !(*it)->Skip())
					break;
				else
					it++;
			}
			(*index)->SetTarget((*it));
		}
		if(CollideEnemy(*(*index)))	// Remove if collide enemy
		{
			(*index)->Removal();
			(*index)->Release();
			continue;
		}
		(*index)->UpdateVectors();	
		(*index)->Update();
	}
}

void CEngine :: UpdateEnemyBulletList()
{
	for(list <Bullet *> :: iterator index = EnemyBullet.List.begin(); index != EnemyBullet.List.end(); ++index)
	{
		if((*index)->Skip())
		{
			(*index)->Release();
			continue;
		}
		(*index)->UpdateVectors();
		(*index)->Update();
	}
}

bool CEngine :: CollideEnemy(CObject & Target)
{
	for(list<Enemy *> :: iterator index = EnemyList.begin(); index != EnemyList.end(); ++index)
		if(Col_ObjToObj(*(*index), Target) && !(*index)->Skip())
		{
			(*index)->ApplyDamage(Target.GetDamage());
			(*index)->CollisionReaction();
			return true;
		}
	return false;
}

bool CEngine :: CollideEnemyBullet(CObject & Target)
{
	for(list <Bullet *> :: iterator index = EnemyBullet.List.begin(); index != EnemyBullet.List.end(); ++index)
	{
		if(Vector(P1->GetPosition() - (*index)->GetPosition()).Length() < g_GameManager->GetGrazeRadius() && (*index)->getGrazeDetect())
		{
			g_GameManager->AddGraze();
			g_AEngine->Set3DSoundPos(GRAZE, P1->GetPosition().Normalize());
			g_AEngine->PlaySegment(GRAZE, 0);
			(*index)->ChangeGrazeDetect();
		}

		if(Col_ObjToObj(*(*index), Target))
			return (*index)->Removal();
	}
	return false;
}

void CEngine :: InsertEnemyList(Enemy * ToBeInsert)
{
	EnemyList.push_back(ToBeInsert);
}

Bullet * CEngine :: InsertBulletList(Bullet * ToBeInsert)
{
	return PlayerBullet.Create(ToBeInsert);
}

Bullet * CEngine :: InsertBulletList(BulletDef & Def)
{
	return PlayerBullet.Create(Def);
}

Bullet * CEngine :: InsertEnemyBullet(Bullet * ToBeInsert)
{
	return EnemyBullet.Create(ToBeInsert);
}

Bullet * CEngine :: InsertEnemyBullet(BulletDef & Def)
{
	return EnemyBullet.Create(Def);
}

void CEngine ::RenderExplosion(Vector temp, int &frame)
{
	glPushMatrix();
		glTranslatef(temp.x, temp.y, temp.z);
		g_TexLoader->DrawXY(m_Explosion, frame, 64);
	glPopMatrix();
	if (g_TimeManager->GetFPS() % 15 == 0 )
		frame++;
}
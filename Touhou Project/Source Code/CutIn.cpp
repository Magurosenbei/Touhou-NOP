
// CutIn.cpp
#include "stdafx.h"
#include "CutIn.h"
#include "Dialog.h"
#include "FlyIn.h"


CutInSystem *g_CutInSystem = CutInSystem::Init("Textures/");


// Initialize Static Members
CutInSystem *CutInSystem::mpInst = NULL;


CutInSystem* CutInSystem::Init(const std::string& tdir)
{
	if( mpInst == NULL )	return new CutInSystem(tdir);
	else					return NULL;
}

void CutInSystem::Deinit(CutInSystem*& what)
{
	if( what == mpInst && mpInst != NULL )
		delete mpInst;

	what = NULL;
}


void CutInSystem::RegisterLua(lua_State& L)
{
	mpDialog->RegisterLua(L);
	mpFlyIn->RegisterLua(L);
}

void CutInSystem::Reset()
{
	mpDialog->Reset();
	mpFlyIn->Reset();
}


void CutInSystem::NextDialog()
{
	mpDialog->Next();
}

void CutInSystem::LaunchPlayerFlyIn()
{
	mpFlyIn->Launch();
}

void CutInSystem::StopAllFlyIns()
{
	mpFlyIn->StopAll();
}


void CutInSystem::Render()
{
	mpDialog->Render();
	mpFlyIn->Render();
}

bool CutInSystem::IsDialog() const
{
	return mpDialog->mIsSession;
}


CutInSystem::CutInSystem(const std::string& tdir)
{
	mpInst = this;
	mpDialog = DialogManager::Init(tdir);
	mpFlyIn = FlyInManager::Init( mpDialog->mTexManager );
}

CutInSystem::~CutInSystem()
{
	mpInst = NULL;
	FlyInManager::Deinit(mpFlyIn);
	DialogManager::Deinit(mpDialog);
}
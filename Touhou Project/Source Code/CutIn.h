#pragma once


class DialogManager;
class FlyInManager;

// Manages everything about cutins: dialogs and flyin. Singleton method.
class CutInSystem
{
public:

	// Takes in application's texture directory.
	// Returns a cutin system or NULL pointer if it is already created.
	static CutInSystem* Init(const std::string& tdir);
	static void Deinit(CutInSystem*& what);

	void RegisterLua(lua_State& L);
	void Reset();

	void NextDialog();
	void LaunchPlayerFlyIn();
	void StopAllFlyIns();

	void Render();
	bool IsDialog() const;


private:

	// Locked
	CutInSystem(const std::string& tdir);
	~CutInSystem();
	CutInSystem(const CutInSystem&);
	const CutInSystem& operator=(const CutInSystem&);


	static CutInSystem *mpInst;
	DialogManager *mpDialog;
	FlyInManager *mpFlyIn;
};

extern CutInSystem *g_CutInSystem;
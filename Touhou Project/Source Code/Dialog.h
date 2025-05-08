#pragma once


// Modifiables
#define DIALOG_LINE			-150	// The y-point where our dialog textures sit
#define DIALOG_OFFSET		15.0f	// How much to offset inactive dialog texture?
#define DIALOG_LEFT			-360	// The bottom leftmost x-point
#define DIALOG_RIGHT		120		// The bottom rightmost x-point
// Note: The above two are also used as IDs, therefore they cannot have the same number.

#define DIALOG_HEIGHT		100		// How big is the dialog text box?
#define DIALOG_PADDING_X	5		// Dialog box side padding
#define DIALOG_PADDING_Y	10		// Dialog box vertical padding


class CutInSystem;

// Manages dialogs during the game. Singleton method.
// This class is part of the CutIn system. Use the CutInSystem interface instead.
class DialogManager
{
public:

	// Takes in application's texture directory.
	// Returns a dialog manager or NULL pointer if manager is already created.
	static DialogManager* Init(const std::string& tdir);
	static void Deinit(DialogManager*& what);

	void RegisterLua(lua_State& L);
	void Reset();

	bool Render(); // returns true if a dialog screen is taking place
	void Next();


private:

	// Locked
	DialogManager(const std::string& tdir);
	~DialogManager();
	DialogManager(const DialogManager&);
	const DialogManager& operator=(const DialogManager&);


	// Lua Callbacks
	static int Lua_AddSet(lua_State* L);
	static int Lua_AddTexture(lua_State* L);
	static int Lua_AddMessage(lua_State* L);
	static int Lua_GetCurrentPlayer(lua_State* L);
	static int Lua_StartSession(lua_State* L);
	static int Lua_EndSession(lua_State* L);

	// Tools
	static std::string Lowercase(const std::string& what);
	static std::string Trim(const std::string& x, const bool do_middle = true, const std::string& symbol = " \f\n\r\t\v");


private:

	// Friends
	friend class CutInSystem;

	// Template for dialog texture
	struct tDialogData // supposed to inherit from TextureData, but not, because of last minute bug
	{
		int set; // Texture set
		std::string path; // File path
		~tDialogData() {};
	};

	// Template for dialog screen
	struct tDialog
	{
		tDialog( const int& id, const std::string& msg, const float& offset = DIALOG_OFFSET )
			:	id(id), msg(msg), offset(offset) {}

		int id;
		float offset;
		std::string msg;
	};


	Texture mTexManager;
	std::vector< tDialogData > mTexData; // this variable is for a last minute bug, so consequently it is a quick fix
	const std::string mTexDir;
	static DialogManager *mpInst;

	int mCurrSet, mPrevDialog;
	std::map< int, int > mTexSet;
	bool mIsStop, mIsSession, mLocked;
	std::vector< tDialog > mDialogList;
};
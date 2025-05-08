
// Dialog.cpp
#include "stdafx.h"
#include "Dialog.h"
#include "player.h"


string g_ErrorMessage;

// Initialize Static Members
DialogManager *DialogManager::mpInst = NULL;


DialogManager* DialogManager::Init(const std::string& tdir)
{
	if( mpInst == NULL )	return new DialogManager(tdir);
	else					return NULL;
}

void DialogManager::Deinit(DialogManager*& what)
{
	if( what == mpInst && mpInst != NULL )
		delete mpInst;

	what = NULL;
}


void DialogManager::RegisterLua(lua_State& L)
{
	// Register script functions
	lua_register(&L, "AddDialogSet", Lua_AddSet);
	lua_register(&L, "AddDialogTexture", Lua_AddTexture);
	lua_register(&L, "AddDialogMessage", Lua_AddMessage);
	lua_register(&L, "GetCurrentPlayer", Lua_GetCurrentPlayer);
	lua_register(&L, "StartDialogSession", Lua_StartSession);
	lua_register(&L, "EndDialogSession", Lua_EndSession);

	// Register alignment global variables
	lua_pushnumber(&L, DIALOG_LEFT);
	lua_setglobal(&L, "DIALOG_LEFT");

	lua_pushnumber(&L, DIALOG_RIGHT);
	lua_setglobal(&L, "DIALOG_RIGHT");
}

void DialogManager::Reset()
{
	mTexManager.ClearAllTextures();
	mTexData.clear();
	mDialogList.clear();
	mTexSet.clear();

	mIsSession = false;
	mCurrSet = 0;
}


bool DialogManager::Render()
{
	// Dialog is not taking place
	if( !mIsSession )
		return false;

	// In case, our lua thread is still adding while we're reading
	while( mLocked ) {};
	mLocked = true;


	std::vector< tDialog >::iterator it = mDialogList.begin();
	std::vector< tDialog >::iterator end = mDialogList.end();

	for( ; it != end; ++it )
	{
		// Retrieve data to compare values with
		TextureData &t = *mTexManager[ it->id ];
		float x = t.width / 2.0f;
		float y = t.height / 2.0f + DIALOG_LINE;
		int ort = mTexSet.find( mTexData[it->id].set )->second;

		// Animate our dialog to make it look nice
		// We're just moving them front and back.
		std::vector< tDialog >::iterator iit = it;
		if( ++iit != end )
		{
			// Change fading value depending on dialog
			if( mTexData[it->id].set != mPrevDialog )	glColor4f(0.3f, 0.3f, 0.3f, 0.8f);
			else										glColor4f(0.7f, 0.7f, 0.7f, 1.0f);

			// Move backwards
			if( mTexData[it->id].set != mPrevDialog && it->offset < DIALOG_OFFSET )
				it->offset += DIALOG_OFFSET / 5.0f;
		}
		else
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

			// Move forwards
			if( it->offset > 0.5f )
				it->offset -= DIALOG_OFFSET / 5.0f;
		}

		// Find our dialog texture position
		if( ort == DIALOG_LEFT )			x += DIALOG_LEFT - it->offset, y -= it->offset;
		else								x += DIALOG_RIGHT - t.width + it->offset, y -= it->offset;

		// Render our dialog texture
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		mTexManager.DrawXY( it->id );
		glPopMatrix();
	}


	// Draw a dialog box
	// Don't let textures affect our box
	glDisable(GL_TEXTURE_2D);

	float w = (DIALOG_RIGHT - DIALOG_LEFT) / 2.0f;
	float h = DIALOG_HEIGHT / 2.0f;

	glPushMatrix();
		glTranslatef(DIALOG_LEFT + w, DIALOG_LINE - h, 0.0f);
		glColor4f(0.0f, 0.0f, 0.0f, 0.6f);

		glBegin(GL_QUADS);
		glVertex2f( -w, -h );
		glVertex2f( w, -h );
		glVertex2f( w, h );
		glVertex2f( -w, h );
		glEnd();
	glPopMatrix();

	// Restore OpenGL State
	glEnable(GL_TEXTURE_2D);

	// Print our dialog message
	glColor3f(1.0f, 1.0f, 1.0f);
	g_Font->Print(mDialogList[ mDialogList.size() - 1 ].msg,	// Message
		DIALOG_LEFT + DIALOG_PADDING_X,							// Position X
		DIALOG_LINE - 16 - DIALOG_PADDING_Y,					// Position Y
		1.0f,													// Scale factor
		DIALOG_RIGHT - DIALOG_LEFT - DIALOG_PADDING_X);			// Wrap Width


	// Unlock module
	mLocked = false;
	return true;
}


void DialogManager::Next()
{
	mIsStop = false;
}


DialogManager::DialogManager(const std::string& tdir)
	:	mTexDir(tdir), mCurrSet(0), mIsSession(false), mLocked(false)
{
	mpInst = this;
}

DialogManager::~DialogManager()
{
	mpInst = NULL;
}


int DialogManager::Lua_AddSet(lua_State* L)
{
	int x = (int)lua_tonumber(L, 1);
	if( x != DIALOG_LEFT && x != DIALOG_RIGHT)		x = DIALOG_LEFT;
	mpInst->mTexSet.insert(  std::pair< int, int >(++mpInst->mCurrSet, x)  );

	// Return set id
	lua_pushnumber(L, mpInst->mCurrSet);
	return 1;
}

int DialogManager::Lua_AddTexture(lua_State* L)
{
	// Add texture to list
	std::string path = mpInst->mTexDir + lua_tostring(L, 1);
	Texture &t = mpInst->mTexManager;

	// Check if texture exists
	FILE *fin = fopen(path.c_str(), "rb");
	if( fin == NULL )
	{
		g_ErrorMessage = "Unable to load dialog texture: " + path.substr( path.find_last_of("\\/") );
		return 0;
	}

	fclose(fin); // Clear our pointer

	// Load a "null" texture if this is the first texture to be loaded
	// We are only using this slot for error checks
	if( t.Size() == 0 )
	{
		t.AddTexture(path, false);
		mpInst->mTexData.push_back(tDialogData());
	}

	// Add our texture
	int x = (int)t.AddTexture(path, false);

	// Improved version
	tDialogData tdata;
	tdata.set = mpInst->mCurrSet;
	tdata.path = Lowercase(Trim(path, true, "\\/"));
	mpInst->mTexData.push_back(tdata);

	// This version has a bug
	// Set our texture data to our dialog texture data
	//tDialogData *tdata = new tDialogData;
	//tdata = (tDialogData*)t[x];
	//tdata->set = mpInst->mCurrSet;
	//t.SetTextureData(x, tdata);

	// Return texture id
	lua_pushnumber(L, x);
	return 1;
}

int DialogManager::Lua_AddMessage(lua_State* L)
{
	// In case, our main thread is still reading while we're adding
	while( mpInst->mLocked ) {}
	mpInst->mLocked = true;

	float offset = DIALOG_OFFSET;
	int index = (int)lua_tonumber(L, 1);
	if( index <= 0 || index >= mpInst->mTexManager.Size() )
	{
		g_ErrorMessage = "Non-existent dialog texture!";
		mpInst->mLocked = false; // Remember to unlock
		return 0;
	}

	tDialogData dt = mpInst->mTexData[index];

	// Check for any repeats in our dialog set
	std::vector< tDialog >::iterator it = mpInst->mDialogList.begin();
	std::vector< tDialog >::iterator end = mpInst->mDialogList.end();
	for( ; it != end; ++it )
	{
		if( dt.set != mpInst->mTexData[it->id].set )	continue;

		// Check if removed is current
		// Don't offset it, just let it fill in the current space
		std::vector< tDialog >::iterator iit = it;
		if( ++iit == end )		offset = 0.0f;

		// Will never be repeated more than twice, so just continue on anyway.
		mpInst->mDialogList.erase(it);
		break;
	}


	// Check if opposite dialog is current
	// Don't offset it, just let it fill in the current space
	if( mpInst->mPrevDialog == dt.set )
	{
		offset = 0.0f;
		int x = mpInst->mDialogList[ mpInst->mDialogList.size()-1 ].id;
		tDialogData ddt = mpInst->mTexData[x];

		if( mpInst->mTexSet.find( ddt.set )->second != mpInst->mTexSet.find( dt.set )->second )
			mpInst->mPrevDialog = ddt.set;
	}

	// Keep our previous dialog's set id
	// We only keep it if it is not on the same side, or not the current space
	// This will actually show which person, the other side is talking to
	if( offset != 0.0f && !mpInst->mDialogList.empty() )
	{
		int x = mpInst->mDialogList[ mpInst->mDialogList.size()-1 ].id;
		tDialogData ddt = mpInst->mTexData[x];

		if( mpInst->mTexSet.find( ddt.set )->second != mpInst->mTexSet.find( dt.set )->second )
			mpInst->mPrevDialog = ddt.set;
	}

	// Add message into our list
	mpInst->mDialogList.push_back(  tDialog(index, lua_tostring(L, 2), offset)  );

	// Unlock module
	mpInst->mLocked = false;

	// Halt script until user has pressed next
	for( mpInst->mIsStop = true; mpInst->mIsStop; ) { Sleep(10); };
	return 0;
}

int DialogManager::Lua_GetCurrentPlayer(lua_State* L)
{
	std::vector< std::string > cutin = P1->GetCutinPath();
	std::vector< std::string >::iterator it, end = cutin.end();
	int size = mpInst->mTexManager.Size();

	// Find the set that contains the texture
	for(int i=0; i<size; ++i )
	{
		for( it = cutin.begin(); it != end; ++it )
		{
			if( mpInst->mTexData[i].path != Lowercase(Trim(*it, true, "\\/")) )
				continue;

			// Return our set number
			lua_pushnumber(L, mpInst->mTexData[i].set);
			return 1;
		}
	}

	return 0;
}

int DialogManager::Lua_StartSession(lua_State*)
{
	mpInst->mIsSession = true;
	return 0;
}

int DialogManager::Lua_EndSession(lua_State*)
{
	mpInst->mIsSession = false;
	mpInst->mDialogList.clear();
	mpInst->mIsSession = false;
	mpInst->mCurrSet = 0;
	return 0;
}


std::string DialogManager::Lowercase(const std::string& what)
{
	std::string tmp = what;
	int size = (int)tmp.size();

	// Change all uppercase letters to lowercase
	for( int i=0; i<size; ++i )
	{
		if(tmp[i] >= 'A' && tmp[i] <= 'Z')
			tmp[i] += ('a' - 'A');
	}

	return tmp;
}

std::string DialogManager::Trim(const std::string& x, const bool do_middle, const std::string& symbol)
{
	std::string copy = x, tmp = "";
	int count = 0, i = 0;
	bool flag = false;

	// Remove trailing whitespaces
	count = (int)( copy.find_last_not_of(symbol) );
	copy.erase(count+1);

	count = (int)( copy.find_first_not_of(symbol) );
	copy.erase(0, count);

	// Return the copy instead of tmp because tmp was not touched
	if( !do_middle )
		return copy;

	// Remove double whitespaces in middle of string, if requested
	count = (int)( copy.size() );
	for( i=0; i < count; ++i )
	{
		if(  flag && isspace( copy[i] )  )
			continue;

		flag = false;
		tmp += copy[i];

		if(  isspace( copy[i] )  )
			flag = true;
	}

	return tmp;
}

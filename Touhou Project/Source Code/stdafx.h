// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#pragma warning (disable:4702)
#pragma warning (disable:4701)
#pragma warning (disable:4099)
#pragma warning (disable:4201)
#pragma warning (disable:4996)
#pragma warning (disable:4239) //shut up the compiler

// Compile settings
#define DEMO_VERSION

// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <cstdio>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>
// Essentials
#include <math.h>						// Header File For Windows Math Library		( ADD )
#include <stdio.h>						// Header File For Standard Input/Output	( ADD )
#include <stdarg.h>						// Header File For Variable Argument Routines	( ADD )
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <Mmsystem.h>
#include <time.h>
#include <cguid.h>						// for GUID_NULL

// Lua bind
extern "C" 
{
#include "Lua/lua.h"
#include "Lua/lualib.h"
#include "Lua/lauxlib.h"
}
#include <luabind/config.hpp>
#include <luabind/class.hpp>
#include <luabind/operator.hpp>
#include <luabind/function.hpp>
#include <luabind/open.hpp>

// DirectX
#include "DirectX/d3d9types.h"
#include "DirectX/dsound.h"
#include "DirectX/dmusicc.h"
#include "DirectX/dmusici.h"

using namespace std;

// OpenGL libraries
#include "OpenGl/GLee.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "OpenGl/glext.h"

#include "vector.h"
#include "array.h"
#include "texture.h"
#include "camera.h"

#include "GameMouse.h"
#include "font.h"
#include "CutIn.h"
#include "FlyIn.h"
#include "Dialog.h"
#include "TimeFrameManager.h"
#include "WeatherSystem.h"
#include "interface.h"
#include "AudioEngine.h"
#include "GamePlay.h"
#include "ItemSystem.h"
#include "engine.h"

#define BUFFER_SIZE (4096 * 10)

#define ShowMouse false

#define SCREEN_BORDER_MAX_X	230.0f
#define	SCREEN_BORDER_MAX_Y 260.0f

#define	SCREEN_BORDER_MIN_X -230.0f
#define	SCREEN_BORDER_MIN_Y -260.0f


extern bool	keys[256];			// Array Used For The Keyboard Routine
extern bool	LMouseButton;
extern bool	RMouseButton;
extern HANDLE hMutex[2];
extern CRITICAL_SECTION _critSection;
extern bool	Running;




enum GameStatus {MENU, HIGH_SCORE, HIGH_SCORE_ENTRY, GAME};									// Overall Status
enum InGameStatus {NORMAL_STAGE, DIALOG, PAUSE, BOSS, CLEAR, COMPLETE, NONE, BOOT, LOSE};	// Only use this when status is GAME
enum Direction {NORMAL, LEFT, RIGHT, UP, DOWN, DOWN_LEFT, UP_LEFT, DOWN_RIGHT, U_RIGHT};	// Direction										// MoveMent Direction

#define DEFAULT_GAMESTATUS MENU
#define DEFAULT_INGAMESTATUS NONE
extern GameStatus	g_GameStatus;
extern InGameStatus g_InGameStatus;
extern InGameStatus g_tmpInGameStatus;

//solved static problems look at the main first few lines for details
//extern bla bla;
//define somewhere datatype :: blabla; once

// Windows resoultion and position definition
#define Window_Res_W 800	//width		i prefer 1280 * 720 but my screen cannot support
#define Window_Res_H 600	//height

#define Window_Pos_X 0		//on your screen
#define Window_Pos_Y 0

#define DebugPrint OutputDebugString


// This is microsoft's toys =P
extern HDC			hDC;		// Private GDI Device Context
extern HGLRC		hRC;		// Permanent Rendering Context
extern HWND			hWnd;		// Holds Our Window Handle
extern HINSTANCE	hInstance;	// Holds The Instance Of The Application


#define NDEBUG

// Debugging Tools
#ifdef _MSC_VER
 #ifdef NDEBUG

	#define ASSERT(exp, msg)	{}
	#define VERIFY(exp, msg)	(exp);

 #else // NDEBUG

	#define ASSERT(exp, msg)	if( !(exp) ) std::_Debug_message(L#msg, _CRT_WIDE(__FILE__), __LINE__);
	#define VERIFY(exp, msg)	if( !(exp) ) std::_Debug_message(L#msg, _CRT_WIDE(__FILE__), __LINE__);

 #endif // end NDEBUG
#else // _MSC_VER
 #ifdef NDEBUG

	#define ASSERT(exp, msg)	{}
	#define VERIFY(exp, msg)	(exp);

 #else // NDEBUG

	#include <cassert>

	#define ASSERT(exp, msg)		assert( (exp) );
	#define VERIFY(exp, msg)		assert( (exp) );

 #endif // end NDEBUG
#endif // end _MSC_VER


// Disable some warnnings
#ifdef _MSC_VER
 #pragma warning(disable:4127)
#endif




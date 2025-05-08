#include "stdafx.h"
#include "Main.h"
#include "grid.h"
#include "player.h"
#include "boss.h"
#include "RtScript.h"

/*
Fixed Logic Error in WndProc
*/

//BookMarks
//BOOKMARK_1//			Add Objects to engine

//-----Definitions------
#pragma warning (disable:4239)
//---------Prototype-----------
DWORD WINAPI UpdateThread(LPVOID pParams);
//-----------------------------
//lua_State * myLuaState;
GameStatus	g_GameStatus = DEFAULT_GAMESTATUS;
InGameStatus g_InGameStatus = DEFAULT_INGAMESTATUS;
InGameStatus g_tmpInGameStatus = DEFAULT_INGAMESTATUS;

bool	active = true;		// Window Active Flag Set To true By Default
bool	fullscreen = true;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool	Running = false;
bool keys[256];			// Array Used For The Keyboard Routine
bool LMouseButton = false;
bool RMouseButton = false;

HDC			hDC		=	NULL;		
HGLRC		hRC		=	NULL;		
HWND		hWnd	=	NULL;		
HINSTANCE	hInstance;		

HANDLE hMutex[2];
CRITICAL_SECTION _critSection;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

void ResizeScene(int width, int height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
		height=1;										// Making Height Equal One

	glViewport(0,0,width,height);							// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(float)width/(float)height, 0.1f, 500.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}
//--------------------------------------------------------------------------
void KillGLWindow()								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(ShowMouse);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);

		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass(WindowClass,hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (true) Or Windowed Mode (false)	*/
 //---------------------------------------------------------------------------
bool CreateGLWindow(int width, int height, BYTE bits, bool fullscreenflag, int	nCmdShow)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left		= (long)0;			// Set Left Value To 0
	WindowRect.right	= (long)width;		// Set Right Value To Requested Width
	WindowRect.top		= (long)0;				// Set Top Value To 0
	WindowRect.bottom	= (long)height;		// Set Bottom Value To Requested Height

	fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOUHOUPROJECT));			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= WindowClass;								// Set The Class Name

	if(!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;											// Return false
	}
	
	if(fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields		= DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if(MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				fullscreen=false;		// Windowed Mode Selected.  Fullscreen = false
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return false;									// Return false
			}
		}
	}
	ShowCursor(ShowMouse);
	if(fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle = (WS_EX_TOPMOST | WS_POPUP | WS_OVERLAPPED);						// Windows Style
		ShowCursor(ShowMouse);										
	}
	else
	{
		dwExStyle	= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;						// Window Extended Style
		dwStyle		= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_OVERLAPPED | WS_SYSMENU;	// Windows Style (no Resize)
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);		// Adjust Window To true Requested Size

	// Create The Window
	if((hWnd = CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								WindowClass,						// Class Name
								WindowTitle,						// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								Window_Pos_X, Window_Pos_Y,			// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)) == false)					// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if((hDC = GetDC(hWnd)) == false)							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

	if((PixelFormat = ChoosePixelFormat(hDC,&pfd)) == false)	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

	if((hRC = wglCreateContext(hDC)) == false)				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}
	ShowWindow(hWnd, nCmdShow);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ResizeScene(width, height);					// Set Up Our Perspective GL Screen

//----------
	if(Running)
		return true;
	if(!g_Engine->InitEngine())			// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}
	return true;									// Success
}
//-------------------------------------------------------------------------
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch(uMsg)									// Check For Windows Messages
	{
//BOOKMARK_1//----------Add Here
		case WM_CREATE:
		{
			if(Running)	// if it re-sized, don't create anything
				return 0;
			
			g_Camera = &Camera :: Instance();
			g_TimeManager = &TimeFrameManager :: Instance();
			g_GameMouse = &InGameMouse :: Instance();
			g_ScriptReader = &RuntimeScript :: Instance();
			g_Interface = &Interface :: Instance();
			g_Engine = &CEngine :: Instance();
			g_AEngine = &AudioEngine :: Instance();
			g_GameManager = &GamePlay :: Instance();

			
			//g_Engine->InsertObjectList(new Grid);
			
			return 0;
		}
//--------------------
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			(!HIWORD(wParam))? active = true : active = false;			
			return 0;								// Return To The Message Loop
		}

		case WM_MOVE:
		{
													// Auto Pause Game
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch(wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{								
			keys[wParam] = true;					// If So, Mark It As true
			ASSERT( !keys[VK_ESCAPE] , "")
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = false;// If So, Mark It As false
			return 0;								// Jump Back
		}

		case WM_SYSKEYDOWN:
		{
			keys[VK_MENU] = true;
		}

		case WM_SYSKEYUP:
		{
			keys[VK_MENU] = false;
		}

		case WM_LBUTTONDOWN:
		{
			LMouseButton = true;
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			RMouseButton = true;
			return 0;
		}
		case WM_LBUTTONUP:
		{
			LMouseButton = false;
			return 0;
		}
		case WM_RBUTTONUP:
		{
			RMouseButton = false;
			return 0;
		}
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);	// Pass All Unhandled Messages To DefWindowProc
}

//----------------------------------------Tried to make this void but it seems compiler will complain so just leave as int
int WINAPI WinMain(	HINSTANCE, HINSTANCE, LPSTR, int nCmdShow)			
{
	MSG	msg;									
	srand(static_cast<int>(time(NULL)));
	//if(MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)	// Ask The User Which Screen Mode They Prefer		
	//	fullscreen = false;													// Windowed Mode

	if(!CreateGLWindow(Window_Res_W,Window_Res_H,32,fullscreen, nCmdShow))	// Create Our OpenGL Window
		return 0;															// Quit If Window Was Not Created

	Running = true;
	
	//g_Engine->InsertEnemyList(new Boss(Vector(0.0f, 0.0f, 0.0f), Vector(), Vector(), 20.0f, 1000, g_TexLoader->AddTexture("Textures/Character/Player/Marisa/Marisa_Player.png"), 48, 0));
	
	InitializeCriticalSection(&_critSection);
	DWORD UpdateID;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UpdateThread , NULL, 0, &UpdateID);
	for(;Running;)											// Loop That Runs While done=false
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if(msg.message==WM_QUIT)				// Have We Received A Quit Message?
				break;								// If So done=true
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			if(active)								// Program Active?
			{
				SwapBuffers(hDC);
				EnterCriticalSection(&_critSection);	// Snatch
				g_AEngine->BuildSegments();	
				Texture::Build();
				g_Engine->RenderFrame();
				LeaveCriticalSection(&_critSection);	// Give up
			}
		}
	}
	// Shutdown
	WeatherSystem :: Deinit(g_WeatherSys);
	g_AEngine->StopSegment(0, true);
	g_AEngine->Shutdown();
	Running = false;
	KillGLWindow();									// Kill The Window
	return static_cast<int>(msg.wParam);			// Exit The Program
}

DWORD WINAPI UpdateThread(LPVOID pParams)
{
	pParams;
	long StartPt, EndPt;
	for(;Running;)
	{
		StartPt = g_TimeManager->GetThisTime();
		EnterCriticalSection(&_critSection);			// Go Snatch Thread Place
		
		g_Engine->ProcessInput();	
		g_WeatherSys->Step();						// Wind System
		g_ItemSys->Step(*g_GameManager, *P1);			
		g_Engine->UpdateGame();	
		LeaveCriticalSection(&_critSection);			// Give up Seat									
		g_TimeManager->UpdateTimeFrame();
		EndPt = (1000 / 60) - (g_TimeManager->GetThisTime() - StartPt);
		if(EndPt > 0)						// Delay
			Sleep(EndPt);
	}
	return NULL;
}


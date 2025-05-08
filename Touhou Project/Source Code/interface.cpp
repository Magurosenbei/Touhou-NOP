#include "stdafx.h"
#include "interface.h"
#include "RtScript.h"

Interface * g_Interface;

Interface :: Interface()
{
	MenuScreen_ID = MAIN_MENU_SCREEN;
	Button_ID = Hover_ID = NONE_BUTTON;
	CharacterChoice = 0;					// default character selected
	WeaponChoice = 0;						// default weapon selected
	MinCharacterSelection = 0;				// Minimum no of Characters involved in the game + 1
	MaxCharacterSelection = 1;				// Maximum no of Characters involved in the game + 1
	MinWeaponSelection	= 0;				// Minimum no of Weapon involved in the game + 1
	MaxWeaponSelection	= 1;				// Maximum no of Weapon involved in the game + 1

	PlayerNameEntryLimit = 13;				// Maximum no of Characters can entered for the HighScore entry

	CurrentLevelSelected = 0;
	
	m_iHelpsDisplay = new int [5];
//	m_iWeaponScreenShot = new int [MaxWeaponSelection + 1 + MaxCharacterSelection + 1];
	m_iCharacterPortrait = new int[MaxCharacterSelection + 1];

	DisplayHelp = false;
	ReadHighScoreData = true;
	PromptQuit = false;
	DirArrButton = 0;

	ButtonSize.x = 128;
	ButtonSize.y = 25;
	PromptTitleSize.x = 170;
	PromptTitleSize.y = 20;

	vec2ButtonPosition[START_BUTTON] = Vector2D(220, 155);				// Start
	vec2ButtonPosition[SELECT_STAGE_BUTTON] = Vector2D(160, 100);		// Select Stage
	vec2ButtonPosition[HIGH_SCORE_BUTTON] = Vector2D(100, 45);			// View High Score
	vec2ButtonPosition[HELP_BUTTON] = Vector2D(40, -10);				// Help
	vec2ButtonPosition[EXIT_BUTTON] = Vector2D(-20, -65);				// Exit

	vec2ButtonPosition[CONTINUE_BUTTON] = Vector2D(-100, 170);			// Continue in pause
	vec2ButtonPosition[RETURN_MAIN_BUTTON] = Vector2D(-100, 110);		// Return to Main From pause

	vec2ButtonPosition[YES_BUTTON] = Vector2D(-100, 20);				// Yes
	vec2ButtonPosition[NO_BUTTON] = Vector2D(-100, -10);				// No
	
	vec2MenuPosition[ARE_YOU_SURE_PROMPT] = Vector2D(-100, 50);			// Question Prompt

	vec2ButtonPosition[RETURN_BUTTON] = Vector2D(300, -270);			// Return from Stage Selection or Character Selection or Help Screen

	vec2ButtonPosition[RIGHT_ARROW_BUTTON] = Vector2D(175,0);			// Toggle between Character Selection (Left)
	vec2ButtonPosition[LEFT_ARROW_BUTTON] = Vector2D(-190,0);			// Toggle between Character Selection (Right)
	
	vec2MenuPosition[PRESS_Z_TO_SELECT_HELP] = Vector2D(0,-275);		// Character Selection Help
	vec2MenuPosition[SELECT_STAGE_TITLE] = Vector2D(0,275);				// Select Stage Title
	vec2MenuPosition[SELECT_CHARACTER_TITLE] = Vector2D(0,275);			// Select Character Title
	vec2MenuPosition[HELP_TITLE] = Vector2D(0,275);

	vec2ButtonPosition[CHARACTER_WEAPON_PORTRAIT_DISPLAY] = Vector2D(0,0);	// Character Portrait
	vec2DisplayPosition[HELP_DISPLAY] = Vector2D(0,0);					// Help Display

	vec2ButtonPosition[GENERAL_HELP_BUTTON]		= Vector2D(300,-20);	// General Help
	vec2ButtonPosition[CONTROL_HELP_BUTTON]		= Vector2D(300,-70);	// Control Help
	vec2ButtonPosition[INTERFACE_HELP_BUTTON]	= Vector2D(300,-120);	// Interface Help
	vec2ButtonPosition[SCREENSHOT_HELP_BUTTON]  = Vector2D(300,-170);	// Screenshot Help
	vec2ButtonPosition[CREDITS_HELP_BUTTON]		= Vector2D(300,-220);	// Credit Help
}

Interface :: ~Interface()
{
	glDeleteLists(m_FontBase, 256);
	delete [] m_iCharacterPortrait;
	delete [] m_iHelpsDisplay;
}

bool Interface :: Init()
{
	g_Font = new Font("Textures/Font/font.png");
	m_iBackGroundTexId = g_TexLoader->AddTexture("Textures/GUI/BackGround.png", false);
	m_iHudTexId = g_TexLoader->AddTexture("Textures/GUI/HUD.png", false);
	m_iLifeBombStar = g_TexLoader->AddTexture("Textures/GUI/stars.png", false);
	m_iMainMenuButtons = g_TexLoader->AddTexture("Textures/MenuOptions/MainMenu.png", false);
	m_iPauseMenuButtons = g_TexLoader->AddTexture("Textures/MenuOptions/PauseMenu.png", false);
	m_iHelpMenuButtons = g_TexLoader->AddTexture("Textures/MenuOptions/HelpMenu.png", false);
	m_iPromptOptionsButtons = g_TexLoader->AddTexture("Textures/MenuOptions/PromptOptions.png", false);

	m_iPromptTitles = g_TexLoader->AddTexture("Textures/MenuOptions/PromptTitles.png", false);
	m_iCharacterPortrait[0] = g_TexLoader->AddTexture("Textures/Character/Player/reimu/ReimuR.png", false);
	m_iCharacterPortrait[1] = g_TexLoader->AddTexture("Textures/Character/Player/marisa/Marisa.png", false);
	m_iHelpsDisplay[0] = g_TexLoader->AddTexture("Textures/MenuOptions/GeneralHelp.png");
	m_iHelpsDisplay[1] = g_TexLoader->AddTexture("Textures/MenuOptions/ControlsHelp.png");
	m_iHelpsDisplay[2] = g_TexLoader->AddTexture("Textures/MenuOptions/InterfaceHelp.png");
	m_iHelpsDisplay[3] = g_TexLoader->AddTexture("Textures/MenuOptions/ScreenshotHelp.png");
	m_iHelpsDisplay[4] = g_TexLoader->AddTexture("Textures/MenuOptions/Creditshelp.png");

	m_iWeaponScreenShot[0][0] = g_TexLoader->AddTexture("Textures/GUI/w1.png", false);
	m_iWeaponScreenShot[0][1] = g_TexLoader->AddTexture("Textures/GUI/w2.png", false);
	m_iWeaponScreenShot[1][0] = g_TexLoader->AddTexture("Textures/GUI/w3.png", false);
	m_iWeaponScreenShot[1][1] = g_TexLoader->AddTexture("Textures/GUI/w4.png", false);

	m_iDirButtons = g_TexLoader->AddTexture("Textures/MenuOptions/DirArrow.png");

	m_iBadEnd = g_TexLoader->AddTexture("Textures/GUI/BadEnd.png");
	m_iGoodEnd = g_TexLoader->AddTexture("Textures/GUI/GoodEnd.png");

	SelectSFX_ID = g_AEngine->LoadSegment("Audio/SFX/select.wav", false);
	g_AEngine->BuildSegments();
	g_AEngine->Set3DSoundPos(SelectSFX_ID, Vector(0, -5, 0));
	CancelSFX_ID = g_AEngine->LoadSegment("Audio/SFX/cancel.wav", false);
	g_AEngine->BuildSegments();
	g_AEngine->Set3DSoundPos(CancelSFX_ID, Vector(0, -5, 0));
	MainBGM_Path = "Audio/Eastern Night.wav";


	int temp = 10000000;
	for(int a=0; a<10; a++)
	{
		*TopPlayerName[a] = "Nobody";
		TopPlayerScore[a] = temp;
		temp/=2;
	}

	MaxStage = int(g_ScriptReader->StagePath.size());

	ReadHighScoreFile();

	return true;
}

void Interface :: MainMenu()
{
	glPushMatrix();

	// Start Button
	if(Hover_ID == START_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[START_BUTTON].x, vec2ButtonPosition[START_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iMainMenuButtons, START_BUTTON, ButtonSize.x);
	glPopMatrix();

	// Select Stage Button
	if(Hover_ID == SELECT_STAGE_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[SELECT_STAGE_BUTTON].x, vec2ButtonPosition[SELECT_STAGE_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iMainMenuButtons, SELECT_STAGE_BUTTON, ButtonSize.x);
	glPopMatrix();

	// Hall Of Fame Button
	if(Hover_ID == HIGH_SCORE_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[HIGH_SCORE_BUTTON].x, vec2ButtonPosition[HIGH_SCORE_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iMainMenuButtons, HIGH_SCORE_BUTTON, ButtonSize.x);
	glPopMatrix();

	// Help Button
	if(Hover_ID == HELP_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[HELP_BUTTON].x, vec2ButtonPosition[HELP_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iMainMenuButtons, HELP_BUTTON, ButtonSize.x);
	glPopMatrix();

	// Exit Button
	if(Hover_ID == EXIT_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[EXIT_BUTTON].x, vec2ButtonPosition[EXIT_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iMainMenuButtons, EXIT_BUTTON, ButtonSize.x);
	glPopMatrix();

	glPopMatrix();
}

void Interface :: Pause()
{
	glPushMatrix();

	// Continue Button
	if(Hover_ID == CONTINUE_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[CONTINUE_BUTTON].x, vec2ButtonPosition[CONTINUE_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iPauseMenuButtons, static_cast<int>(CONTINUE_BUTTON - CONTINUE_BUTTON), ButtonSize.x);
	glPopMatrix();

	// Return To Main Menu Button
	if(Hover_ID == RETURN_MAIN_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[RETURN_MAIN_BUTTON].x, vec2ButtonPosition[RETURN_MAIN_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iPauseMenuButtons, static_cast<int>(RETURN_MAIN_BUTTON - CONTINUE_BUTTON), ButtonSize.x);
	glPopMatrix();

	glPopMatrix();
}

void Interface :: PromptQuitSelection()
{
	glPushMatrix();

	// Question Prompt
	glColor4f(1,1,1,1);

	glPushMatrix();
	glTranslatef(vec2MenuPosition[ARE_YOU_SURE_PROMPT].x, vec2MenuPosition[ARE_YOU_SURE_PROMPT].y, 0);
	g_TexLoader->DrawXY(m_iPromptTitles, ARE_YOU_SURE_PROMPT, PromptTitleSize.x);
	glPopMatrix();

	// Yes Button
	if(Hover_ID == YES_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[YES_BUTTON].x, vec2ButtonPosition[YES_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iPromptOptionsButtons, static_cast<int>(YES_BUTTON - YES_BUTTON), ButtonSize.x);
	glPopMatrix();

	// No Button
	if(Hover_ID == NO_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[NO_BUTTON].x, vec2ButtonPosition[NO_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iPromptOptionsButtons, static_cast<int>(NO_BUTTON - YES_BUTTON), ButtonSize.x);
	glPopMatrix();

	glPopMatrix();
}

void Interface :: PauseMenu()
{
	BlendQuad();
	if(PromptQuit == false)
		Pause();
	else
		PromptQuitSelection();
}

void Interface :: CharacterSelection()
{
	glPushMatrix();

	// Return Button
	if(Hover_ID == RETURN_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[RETURN_BUTTON].x, vec2ButtonPosition[RETURN_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iPromptOptionsButtons, static_cast<int>(RETURN_BUTTON - YES_BUTTON), ButtonSize.x);
	glPopMatrix();

	// Character Select Help
	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslatef(vec2MenuPosition[PRESS_Z_TO_SELECT_HELP].x, vec2MenuPosition[PRESS_Z_TO_SELECT_HELP].y, 0);
	g_TexLoader->DrawXY(m_iPromptTitles, PRESS_Z_TO_SELECT_HELP, PromptTitleSize.x);
	glPopMatrix();

	// Character Select Title
	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslatef(vec2MenuPosition[SELECT_CHARACTER_TITLE].x, vec2MenuPosition[SELECT_CHARACTER_TITLE].y, 0);
	g_TexLoader->DrawXY(m_iPromptTitles, SELECT_CHARACTER_TITLE, PromptTitleSize.x);
	glPopMatrix();

	// Character Portrait
	if(Hover_ID == CHARACTER_WEAPON_PORTRAIT_DISPLAY || DirArrButton == 1 || DirArrButton == 2)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[CHARACTER_WEAPON_PORTRAIT_DISPLAY].x, vec2ButtonPosition[CHARACTER_WEAPON_PORTRAIT_DISPLAY].y, 0);
	g_TexLoader->DrawXY(m_iCharacterPortrait[CharacterChoice]);
	glPopMatrix();


	// Left Arrow Button
	if(Hover_ID == RIGHT_ARROW_BUTTON || DirArrButton == 2)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[RIGHT_ARROW_BUTTON].x, vec2ButtonPosition[RIGHT_ARROW_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iDirButtons, static_cast<int>(LEFT_ARROW_BUTTON-RIGHT_ARROW_BUTTON), 96 );
	glPopMatrix();

	// Right Arrow Button
	if(Hover_ID == LEFT_ARROW_BUTTON || DirArrButton == 1)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[LEFT_ARROW_BUTTON].x, vec2ButtonPosition[LEFT_ARROW_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iDirButtons, static_cast<int>(RIGHT_ARROW_BUTTON-RIGHT_ARROW_BUTTON), 96 );
	glPopMatrix();	

	glPopMatrix();
}

void Interface :: StageSelection()
{
	glPushMatrix();

	// Return Button
	if(Hover_ID == RETURN_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[RETURN_BUTTON].x, vec2ButtonPosition[RETURN_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iPromptOptionsButtons, static_cast<int>(RETURN_BUTTON - YES_BUTTON), ButtonSize.x);
	glPopMatrix();

	// Left Arrow Button
	if(Hover_ID == RIGHT_ARROW_BUTTON || DirArrButton == 2)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[RIGHT_ARROW_BUTTON].x, vec2ButtonPosition[RIGHT_ARROW_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iDirButtons, static_cast<int>(LEFT_ARROW_BUTTON-RIGHT_ARROW_BUTTON), 96 );
	glPopMatrix();

	// Right Arrow Button
	if(Hover_ID == LEFT_ARROW_BUTTON || DirArrButton == 1)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[LEFT_ARROW_BUTTON].x, vec2ButtonPosition[LEFT_ARROW_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iDirButtons, static_cast<int>(RIGHT_ARROW_BUTTON-RIGHT_ARROW_BUTTON), 96 );
	glPopMatrix();	


	if(Hover_ID == CHARACTER_WEAPON_PORTRAIT_DISPLAY || DirArrButton == 1 || DirArrButton == 2)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);
	sprintf(stageLevel, "%i", CurrentLevelSelected+1);
	g_Font->Print("Stage : " + string(stageLevel), -100,0,2,0,0);

	glPopMatrix();
}

void Interface :: WeaponSelection()
{
	glPushMatrix();

	// Return Button
	if(Hover_ID == RETURN_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[RETURN_BUTTON].x, vec2ButtonPosition[RETURN_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iPromptOptionsButtons, static_cast<int>(RETURN_BUTTON - YES_BUTTON), ButtonSize.x);
	glPopMatrix();

	// Character Select Help
	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslatef(vec2MenuPosition[PRESS_Z_TO_SELECT_HELP].x, vec2MenuPosition[PRESS_Z_TO_SELECT_HELP].y, 0);
	g_TexLoader->DrawXY(m_iPromptTitles, PRESS_Z_TO_SELECT_HELP, PromptTitleSize.x);
	glPopMatrix();

	// Character Select Title
	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslatef(vec2MenuPosition[SELECT_CHARACTER_TITLE].x, vec2MenuPosition[SELECT_CHARACTER_TITLE].y, 0);
	g_TexLoader->DrawXY(m_iPromptTitles, SELECT_CHARACTER_TITLE+4, PromptTitleSize.x);
	glPopMatrix();

	// Weapon Display
	if(Hover_ID == CHARACTER_WEAPON_PORTRAIT_DISPLAY || DirArrButton == 1 || DirArrButton == 2)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[CHARACTER_WEAPON_PORTRAIT_DISPLAY].x, vec2ButtonPosition[CHARACTER_WEAPON_PORTRAIT_DISPLAY].y, 0);
	g_TexLoader->DrawXY(m_iWeaponScreenShot[CharacterChoice][WeaponChoice]);
	glPopMatrix();


	// Left Arrow Button
	if(Hover_ID == RIGHT_ARROW_BUTTON || DirArrButton == 1)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[RIGHT_ARROW_BUTTON].x, vec2ButtonPosition[RIGHT_ARROW_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iDirButtons, static_cast<int>(LEFT_ARROW_BUTTON-RIGHT_ARROW_BUTTON), 96 );
	glPopMatrix();

	// Right Arrow Button
	if(Hover_ID == LEFT_ARROW_BUTTON || DirArrButton == 2)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[LEFT_ARROW_BUTTON].x, vec2ButtonPosition[LEFT_ARROW_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iDirButtons, static_cast<int>(RIGHT_ARROW_BUTTON-RIGHT_ARROW_BUTTON), 96 );
	glPopMatrix();	

	glPopMatrix();
}

void Interface :: HelpDisplay()
{
	glPushMatrix();
	BlendQuad();

	// Return Button
	if(Hover_ID == RETURN_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[RETURN_BUTTON].x, vec2ButtonPosition[RETURN_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iPromptOptionsButtons, static_cast<int>(RETURN_BUTTON - YES_BUTTON), ButtonSize.x);
	glPopMatrix();

	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslatef(vec2MenuPosition[HELP_TITLE].x, vec2MenuPosition[HELP_TITLE].y, 0);
	g_TexLoader->DrawXY(m_iPromptTitles, HELP_TITLE, PromptTitleSize.x);
	glPopMatrix();

	// General Help Button
	if(Hover_ID == GENERAL_HELP_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[GENERAL_HELP_BUTTON].x, vec2ButtonPosition[GENERAL_HELP_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iHelpMenuButtons, static_cast<int>(GENERAL_HELP_BUTTON - GENERAL_HELP_BUTTON), ButtonSize.x);
	glPopMatrix();

	// Control Help Button
	if(Hover_ID == CONTROL_HELP_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[CONTROL_HELP_BUTTON].x, vec2ButtonPosition[CONTROL_HELP_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iHelpMenuButtons, static_cast<int>(CONTROL_HELP_BUTTON - GENERAL_HELP_BUTTON), ButtonSize.x);
	glPopMatrix();

	// Interface Help Button
	if(Hover_ID == INTERFACE_HELP_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[INTERFACE_HELP_BUTTON].x, vec2ButtonPosition[INTERFACE_HELP_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iHelpMenuButtons, static_cast<int>(INTERFACE_HELP_BUTTON - GENERAL_HELP_BUTTON), ButtonSize.x);
	glPopMatrix();

	// Screen Shot Help Button
	if(Hover_ID == SCREENSHOT_HELP_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[SCREENSHOT_HELP_BUTTON].x, vec2ButtonPosition[SCREENSHOT_HELP_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iHelpMenuButtons, static_cast<int>(SCREENSHOT_HELP_BUTTON - GENERAL_HELP_BUTTON), ButtonSize.x);
	glPopMatrix();

	// Credit Help Button
	if(Hover_ID == CREDITS_HELP_BUTTON)
		glColor4f(1,1,1,1);
	else
		glColor4f(0.6f,0.6f,0.6f,0.6f);

	glPushMatrix();
	glTranslatef(vec2ButtonPosition[CREDITS_HELP_BUTTON].x, vec2ButtonPosition[CREDITS_HELP_BUTTON].y, 0);
	g_TexLoader->DrawXY(m_iHelpMenuButtons, static_cast<int>(CREDITS_HELP_BUTTON - GENERAL_HELP_BUTTON), ButtonSize.x);
	glPopMatrix();


	// Display Help
	glColor4f(1,1,1,1);

	glPushMatrix();
	glScalef(0.75,1,1);
	glTranslatef(vec2DisplayPosition[HELP_DISPLAY].x, vec2DisplayPosition[HELP_DISPLAY].y, 0);
	g_TexLoader->DrawXY(m_iHelpsDisplay[Help_ID]);
	glPopMatrix();

	glPopMatrix();
}

void Interface :: MainSelectionScreen()
{
	switch(MenuScreen_ID)
	{
	case MAIN_MENU_SCREEN:				MainMenu();
									break;
	case STAGE_SELECTION_SCREEN:		StageSelection();
									break;
	case CHARACTER_SELECTION_SCREEN:	CharacterSelection();
									break;
	case WEAPON_SELECTION_SCREEN:		WeaponSelection();
									break;
	case HELP_SCREEN:					HelpDisplay();
									break;
	}
}

int Interface :: GetSelectedWeapon()
{
	return WeaponChoice;
}

int Interface :: GetSelectedCharacter()
{
	return CharacterChoice;
}

void Interface :: WeaponSelectionProcessInput()
{
	switch(Button_ID)
	{		
	case RIGHT_ARROW_BUTTON:
		if(++WeaponChoice > MaxWeaponSelection)
			WeaponChoice = MinWeaponSelection;
		break;

	case LEFT_ARROW_BUTTON:
		if(--WeaponChoice < MinWeaponSelection)
			WeaponChoice = MaxWeaponSelection;
		break;

	case RETURN_BUTTON:		
		MenuScreen_ID = CHARACTER_SELECTION_SCREEN;
		DirArrButton = 0;
		break;

	case CHARACTER_WEAPON_PORTRAIT_DISPLAY:
		g_AEngine->StopSegment(BGM, false);
		
		g_GameStatus = GAME;
		g_InGameStatus = BOOT;
		MenuScreen_ID = NONE_SCREEN;

		g_ScriptReader->UseStage(CurrentLevelSelected);
		DirArrButton = 0;
		break;
	}
	Button_ID = NONE_BUTTON;
}

void Interface :: CharacterSelectionProcessInput()
{
	switch(Button_ID)
	{		
	case RIGHT_ARROW_BUTTON:
		if(++CharacterChoice > MaxCharacterSelection)
			CharacterChoice = MinCharacterSelection;
		break;

	case LEFT_ARROW_BUTTON:
		if(--CharacterChoice < MinCharacterSelection)
			CharacterChoice = MaxCharacterSelection;
		break;

	case RETURN_BUTTON:		
		MenuScreen_ID = MAIN_MENU_SCREEN;
		DirArrButton = 0;
		break;

	case CHARACTER_WEAPON_PORTRAIT_DISPLAY:
		MenuScreen_ID = WEAPON_SELECTION_SCREEN;
		DirArrButton = 0;
		break;
	}
	Button_ID = NONE_BUTTON;
}

void Interface :: StageSelectionProcessInput()
{
	switch(Button_ID)
	{		
	case RETURN_BUTTON:		
		CurrentLevelSelected=1;
		MenuScreen_ID = MAIN_MENU_SCREEN;
		break;

	case LEFT_ARROW_BUTTON:
		if(--CurrentLevelSelected < 1)
			CurrentLevelSelected = MaxStage;
		break;

	case RIGHT_ARROW_BUTTON:
		if(++CurrentLevelSelected >MaxStage)
			CurrentLevelSelected = 0;
		break;

	case CHARACTER_WEAPON_PORTRAIT_DISPLAY:
		MenuScreen_ID = CHARACTER_SELECTION_SCREEN;
		break;
	}
	Button_ID = NONE_BUTTON;
}

void Interface :: MainMenuProcessInput()
{
	switch(MenuScreen_ID)
	{
	case MAIN_MENU_SCREEN:
		switch(Button_ID)
		{
		case START_BUTTON:
			MenuScreen_ID = CHARACTER_SELECTION_SCREEN;
			MouseCenter = true;
			CenterMousePosition();
			break;

		case SELECT_STAGE_BUTTON:
			MenuScreen_ID = STAGE_SELECTION_SCREEN;
			MouseCenter = true;
			CenterMousePosition();
			break;

		case HELP_BUTTON:
			MenuScreen_ID = HELP_SCREEN;
			DisplayHelp = true;
			MouseCenter = true;
			CenterMousePosition();
			break;

		case HIGH_SCORE_BUTTON:
			g_GameStatus = HIGH_SCORE;	// Show High Score
			MouseCenter = true;
			CenterMousePosition();
			break;

		case EXIT_BUTTON:
			StoreHighScoreFile();
			Running = false;
			break;
		}
		Button_ID = NONE_BUTTON;
		break;

	case CHARACTER_SELECTION_SCREEN:
		CharacterSelectionProcessInput();
		break;

	case WEAPON_SELECTION_SCREEN:
		WeaponSelectionProcessInput();
		break;

	case STAGE_SELECTION_SCREEN:
		StageSelectionProcessInput();
		break;

	case HELP_SCREEN:
		HelpProcessInput();
		break;

	default: break;

	}
}

void Interface :: HelpProcessInput()
{
	switch(Button_ID)
	{
	case GENERAL_HELP_BUTTON:
	case CONTROL_HELP_BUTTON:
	case INTERFACE_HELP_BUTTON:
	case SCREENSHOT_HELP_BUTTON:
	case CREDITS_HELP_BUTTON:
		Help_ID = Button_ID - GENERAL_HELP_BUTTON;
		break;

	case RETURN_BUTTON:	
		MenuScreen_ID = MAIN_MENU_SCREEN;
		DisplayHelp = false;
		break;
	}
	Button_ID = NONE_BUTTON;
}

void Interface :: PauseMenuProcessInput()
{
	switch(Button_ID)
	{
	case CONTINUE_BUTTON:	
		g_InGameStatus = NORMAL_STAGE;	// return back need change later
		break;

	case RETURN_MAIN_BUTTON:	
		PromptQuit = true;
		break;
	}
	Button_ID = NONE_BUTTON;
}

void Interface :: PromptMenuProcessInput()
{
	switch(Button_ID)
	{		
	case YES_BUTTON:
		g_AEngine->SwitchBGMOff();
		g_AEngine->StopSegment(BGM, true);
		g_AEngine->LoadSegment(MainBGM_Path.c_str(), true, BGM);
		LeaveCriticalSection(&_critSection);
		Sleep(100);
		EnterCriticalSection(&_critSection);
		g_AEngine->PlaySegment(BGM, -1);
		MenuScreen_ID = MAIN_MENU_SCREEN;
		g_GameStatus = MENU;
		g_InGameStatus = NONE;
		PromptQuit = false;
		CurrentLevelSelected = 0;
		break;

	case NO_BUTTON:				
		PromptQuit = false;
		break;
	}
	Button_ID = NONE_BUTTON;
}

void Interface :: CenterMousePosition()
{
	if(MouseCenter == true)
	{
		g_GameMouse->SetPosition(Vector2D(0,0));
		MouseCenter = false;
	}
}

void Interface :: KeyboardProcessInput()
{		
	if(MenuScreen_ID == CHARACTER_SELECTION_SCREEN || MenuScreen_ID == WEAPON_SELECTION_SCREEN)
	{
		if(keys[VK_RIGHT])
		{
			DirArrButton = 2;
			Button_ID = Hover_ID = RIGHT_ARROW_BUTTON;
			if(MenuScreen_ID == CHARACTER_SELECTION_SCREEN)
			{
				if(++CharacterChoice > MaxCharacterSelection)
					CharacterChoice = MinCharacterSelection;
			}
			else if (MenuScreen_ID == WEAPON_SELECTION_SCREEN)
			{	
				if(++WeaponChoice > MaxWeaponSelection)
					WeaponChoice = MinWeaponSelection;
			}			
			keys[VK_RIGHT] = false;
			g_AEngine->PlaySegment(SelectSFX_ID, 0);
			OnSoundSelectEffect = true;
		}
		else if(keys[VK_LEFT])
		{
			DirArrButton = 1;
			Button_ID = Hover_ID = LEFT_ARROW_BUTTON;
			if (MenuScreen_ID == CHARACTER_SELECTION_SCREEN)
			{	
				if(--CharacterChoice < MinCharacterSelection)
					CharacterChoice = MaxCharacterSelection;
			}
			else if (MenuScreen_ID == WEAPON_SELECTION_SCREEN)
			{	
				if(--WeaponChoice < MinWeaponSelection)
					WeaponChoice = MaxWeaponSelection;
			}		
			keys[VK_LEFT] = false;
			g_AEngine->PlaySegment(SelectSFX_ID, 0);
			OnSoundSelectEffect = true;
		}
	}

	int temp_ID = Hover_ID = tempHover_ID;
	if( keys[VK_DOWN] )
	{
		DirArrButton = 0;
		g_AEngine->PlaySegment(SelectSFX_ID, 0);
		if(g_GameStatus == MENU && g_InGameStatus == NONE)
		{
			if(MenuScreen_ID == MAIN_MENU_SCREEN)
			{
				if(++temp_ID > EXIT_BUTTON)
					temp_ID = START_BUTTON;
				tempHover_ID = Hover_ID = temp_ID;
			}
			if(MenuScreen_ID == CHARACTER_SELECTION_SCREEN || MenuScreen_ID == WEAPON_SELECTION_SCREEN)
			{
				if(temp_ID != RETURN_BUTTON)
					temp_ID = RETURN_BUTTON;
				tempHover_ID = Hover_ID = temp_ID;
			}
			else if(MenuScreen_ID == STAGE_SELECTION_SCREEN)
			{
				if(++temp_ID > RETURN_BUTTON)
					temp_ID = RETURN_BUTTON;
				tempHover_ID = Hover_ID = temp_ID;
			}
			else if(MenuScreen_ID == HELP_SCREEN)
			{
				if(++temp_ID > CREDITS_HELP_BUTTON || temp_ID < GENERAL_HELP_BUTTON)
					temp_ID = RETURN_BUTTON;
				tempHover_ID = Hover_ID = temp_ID;
			}			
		}

		if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == false)
		{
			if(++temp_ID > RETURN_MAIN_BUTTON)
				temp_ID = CONTINUE_BUTTON;
			tempHover_ID = Hover_ID = temp_ID;
		}

		if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == true)
		{
			if(++temp_ID > NO_BUTTON)
				temp_ID = YES_BUTTON;
			tempHover_ID = Hover_ID = temp_ID;
		}
		keys[VK_DOWN] = false;
	}

	else if(keys[VK_UP])
	{
		DirArrButton = 0;
		g_AEngine->PlaySegment(SelectSFX_ID, 0);
		if(g_GameStatus == MENU && g_InGameStatus == NONE)
		{
			if(MenuScreen_ID == MAIN_MENU_SCREEN)
			{
				if(--temp_ID < START_BUTTON)
					temp_ID = EXIT_BUTTON;
				tempHover_ID = Hover_ID = temp_ID;
			}
			else if(MenuScreen_ID == CHARACTER_SELECTION_SCREEN || MenuScreen_ID == WEAPON_SELECTION_SCREEN)
			{
				if(temp_ID != RETURN_BUTTON)
					temp_ID = RETURN_BUTTON;
				tempHover_ID = Hover_ID = temp_ID;
			}
			else if(MenuScreen_ID == STAGE_SELECTION_SCREEN)
			{
				if(--temp_ID < RETURN_BUTTON)
					temp_ID = RETURN_BUTTON;
				tempHover_ID = Hover_ID = temp_ID;
			}
			else if(MenuScreen_ID == HELP_SCREEN)
			{
				if(--temp_ID < RETURN_BUTTON)
					temp_ID = CREDITS_HELP_BUTTON;
				tempHover_ID = Hover_ID = temp_ID;
			}
		}

		if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == false)
		{
			if(--temp_ID < CONTINUE_BUTTON)
				temp_ID = RETURN_MAIN_BUTTON;
			tempHover_ID = Hover_ID = temp_ID;
		}

		if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == true)
		{
			if(--temp_ID < YES_BUTTON)
				temp_ID = NO_BUTTON;
			tempHover_ID = Hover_ID = temp_ID;
		}
		keys[VK_UP] = false;
	}

	if(keys[VK_RETURN] || keys['Z'])
	{
		DirArrButton = 0;
		g_AEngine->PlaySegment(SelectSFX_ID, 0);
		Button_ID = Hover_ID;
		if(MenuScreen_ID == CHARACTER_SELECTION_SCREEN || MenuScreen_ID == WEAPON_SELECTION_SCREEN)
		{
			if(Hover_ID != RETURN_BUTTON)
				Button_ID = Hover_ID = CHARACTER_WEAPON_PORTRAIT_DISPLAY;
			MainMenuProcessInput();
		}
		else
		{	
			if(g_GameStatus == MENU && g_InGameStatus == NONE)
				MainMenuProcessInput();

			if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == false)
				PauseMenuProcessInput();

			if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == true)
				PromptMenuProcessInput();
		}
		keys[VK_RETURN] = false;
		keys['Z'] = false;
	}

	if(keys['X'] || keys[VK_ESCAPE])
	{
		DirArrButton = 0;
		g_AEngine->PlaySegment(CancelSFX_ID, 0);
		Button_ID = Hover_ID = RETURN_BUTTON;
		if(g_GameStatus == MENU && g_InGameStatus == NONE)
			MainMenuProcessInput();
		else if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == false)
		{
			Button_ID = Hover_ID = CONTINUE_BUTTON;
			PauseMenuProcessInput();
		}
		else if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == true)
		{
			Button_ID = Hover_ID = NO_BUTTON;
			PromptMenuProcessInput();
		}
		else if(g_GameStatus == HIGH_SCORE)
		{
			g_GameStatus = MENU;
			MenuScreen_ID = MAIN_MENU_SCREEN;
		}
		keys['X'] = false;
		keys[VK_ESCAPE] = false;
	}
}

void Interface :: ProcessInput()
{
	mousePos = g_GameMouse->GetGameMousePosition();
	if(mousePosRecord != mousePos)
	{
		DirArrButton = 0;
		if(g_GameStatus == MENU)
		{
			g_AEngine->LoadSegment(MainBGM_Path.c_str(), true, BGM);
			g_AEngine->PlaySegment(BGM, -1);
			if(MenuScreen_ID == MAIN_MENU_SCREEN)
				for(int a = START_BUTTON; a <= EXIT_BUTTON; a++)
				{
					if( vec2ButtonPosition[a].x - mousePos.x <= (ButtonSize.x / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((ButtonSize.x * -1 ) /2) )
						if( vec2ButtonPosition[a].y - mousePos.y <= (ButtonSize.y / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((ButtonSize.y * -1 ) /2) )
						{
							tempHover_ID = Hover_ID = a;
							mousePosRecord = mousePos;
							break;
						}
					Hover_ID = NONE_BUTTON;
				}
			else if(MenuScreen_ID == CHARACTER_SELECTION_SCREEN || MenuScreen_ID == WEAPON_SELECTION_SCREEN)
			{
				for(int a = RETURN_BUTTON; a <= CHARACTER_WEAPON_PORTRAIT_DISPLAY; a++)
				{
					if(a == RETURN_BUTTON)
					{
						if( vec2ButtonPosition[a].x - mousePos.x <= (ButtonSize.x / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((ButtonSize.x * -1 ) /2) )
							if( vec2ButtonPosition[a].y - mousePos.y <= (ButtonSize.y / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((ButtonSize.y * -1 ) /2) )
							{
								tempHover_ID = Hover_ID = a;
								mousePosRecord = mousePos;
								break;
							}
						tempHover_ID = Hover_ID = NONE_BUTTON;
					}
					else if(a == LEFT_ARROW_BUTTON || a == RIGHT_ARROW_BUTTON)
					{
						if( vec2ButtonPosition[a].x - mousePos.x <= (ButtonSize.x / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((ButtonSize.x * -1 ) /2) )
							if( vec2ButtonPosition[a].y - mousePos.y <= (96 / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((92 * -1 ) /2) )
							{
								tempHover_ID = Hover_ID = a;
								mousePosRecord = mousePos;
								break;
							}
						tempHover_ID = Hover_ID = NONE_BUTTON;
					}
					else if(a == CHARACTER_WEAPON_PORTRAIT_DISPLAY)
					{
						if( vec2ButtonPosition[a].x - mousePos.x <= (210 / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((210 * -1 ) /2) )
							if( vec2ButtonPosition[a].y - mousePos.y <= (320 / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((320 * -1) /2) )
							{
								tempHover_ID = Hover_ID = a;
								mousePosRecord = mousePos;
								break;
							}
						tempHover_ID = Hover_ID = NONE_BUTTON;
					}
				}
			}
			else if(MenuScreen_ID == STAGE_SELECTION_SCREEN)
				for(int a = RETURN_BUTTON; a <= CHARACTER_WEAPON_PORTRAIT_DISPLAY; a++)
				{
					if(a == RETURN_BUTTON)
					{
						if( vec2ButtonPosition[a].x - mousePos.x <= (ButtonSize.x / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((ButtonSize.x * -1 ) /2) )
							if( vec2ButtonPosition[a].y - mousePos.y <= (ButtonSize.y / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((ButtonSize.y * -1 ) /2) )
							{
								tempHover_ID = Hover_ID = a;
								mousePosRecord = mousePos;
								break;
							}
						tempHover_ID = Hover_ID = NONE_BUTTON;
					}
					else if(a == LEFT_ARROW_BUTTON || a == RIGHT_ARROW_BUTTON)
					{
						if( vec2ButtonPosition[a].x - mousePos.x <= (ButtonSize.x / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((ButtonSize.x * -1 ) /2) )
							if( vec2ButtonPosition[a].y - mousePos.y <= (96 / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((92 * -1 ) /2) )
							{
								tempHover_ID = Hover_ID = a;
								mousePosRecord = mousePos;
								break;
							}
						tempHover_ID = Hover_ID = NONE_BUTTON;
					}
					else if(a == CHARACTER_WEAPON_PORTRAIT_DISPLAY)
					{
						if( vec2ButtonPosition[a].x - mousePos.x <= (210 / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((210 * -1 ) /2) )
							if( vec2ButtonPosition[a].y - mousePos.y <= (320 / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((320 * -1) /2) )
							{
								tempHover_ID = Hover_ID = a;
								mousePosRecord = mousePos;
								break;
							}
						tempHover_ID = Hover_ID = NONE_BUTTON;
					}
				}
			else if(MenuScreen_ID == HELP_SCREEN)
				for(int a = RETURN_BUTTON; a <= CREDITS_HELP_BUTTON; a++)
				{
					if( vec2ButtonPosition[a].x - mousePos.x <= (ButtonSize.x / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((ButtonSize.x * -1 ) /2) )
						if( vec2ButtonPosition[a].y - mousePos.y <= (ButtonSize.y / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((ButtonSize.y * -1 ) /2) )
						{
							tempHover_ID = Hover_ID = a;
							mousePosRecord = mousePos;
							break;
						}
					Hover_ID = NONE_BUTTON;
				}

		}

		else if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == false)
		{
			for(int a = CONTINUE_BUTTON; a <= RETURN_MAIN_BUTTON; a++)
			{
				if( vec2ButtonPosition[a].x - mousePos.x <= (ButtonSize.x / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((ButtonSize.x * -1 ) /2) )
					if( vec2ButtonPosition[a].y - mousePos.y <= (ButtonSize.y / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((ButtonSize.y * -1 ) /2) )
					{
						tempHover_ID = Hover_ID = a;
						mousePosRecord = mousePos;
						break;
					}
				Hover_ID = NONE_BUTTON;
			}
		}

		else if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == true)
		{
			for(int a = YES_BUTTON; a <= NO_BUTTON; a++)
			{
				if( vec2ButtonPosition[a].x - mousePos.x <= (ButtonSize.x / 2) && vec2ButtonPosition[a].x - mousePos.x >= ((ButtonSize.x * -1 ) /2) )
					if( vec2ButtonPosition[a].y - mousePos.y <= (ButtonSize.y / 2) && vec2ButtonPosition[a].y - mousePos.y >= ((ButtonSize.y * -1 ) /2) )
					{
						tempHover_ID = Hover_ID = a;
						mousePosRecord = mousePos;
						break;
					}
				Hover_ID = NONE_BUTTON;
			}
		}

		if(MenuScreen_ID == CHARACTER_SELECTION_SCREEN || MenuScreen_ID == CHARACTER_SELECTION_SCREEN)
			mousePosRecord = mousePos;
	}

	if(OnSoundSelectEffect == true && Hover_ID != NONE_BUTTON)
	{
		g_AEngine->PlaySegment(SelectSFX_ID, 0);
		OnSoundSelectEffect = false;
	}
	else if(Hover_ID == NONE_BUTTON)
	{
		OnSoundSelectEffect = true;
		Hover_ID = tempHover_ID;
	}
	
	if(LMouseButton)
	{
		g_AEngine->PlaySegment(SelectSFX_ID, 0);
		Button_ID = Hover_ID;
		if(g_GameStatus == MENU && g_InGameStatus == NONE)
			MainMenuProcessInput();

		else if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == false)
			PauseMenuProcessInput();

		else if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == true)
			PromptMenuProcessInput();
		LMouseButton = false;
	}
	else if(RMouseButton)
	{
		g_AEngine->PlaySegment(CancelSFX_ID, 0);
		if(g_GameStatus == MENU && g_InGameStatus == NONE)
		{
			if(MenuScreen_ID == CHARACTER_SELECTION_SCREEN || MenuScreen_ID == WEAPON_SELECTION_SCREEN || MenuScreen_ID == STAGE_SELECTION_SCREEN || MenuScreen_ID == HELP_SCREEN)
				Button_ID = Hover_ID = RETURN_BUTTON;
			MainMenuProcessInput();
		}
		else if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == false)
		{
			Button_ID = Hover_ID = CONTINUE_BUTTON;
			PauseMenuProcessInput();
		}
		else if(g_InGameStatus == PAUSE && g_GameStatus == GAME && PromptQuit == true)
		{
			Button_ID = Hover_ID = NO_BUTTON;
			PromptMenuProcessInput();
		}
		else if(g_GameStatus == HIGH_SCORE)
			g_GameStatus = MENU;
		RMouseButton = false;
	}
}

int Interface :: StringToInt(string strConvert)
{ 
  int intReturn = atoi(strConvert.c_str()); 
  return(intReturn); 
}

void Interface :: ReadHighScoreFile()
{
	int CurrentLocation = 0;
	string temp;

	ifstream HighScoreD;
	HighScoreD.open ("Data/HighScore.txt");

	if(ReadHighScoreData == true)
	{
		ReadHighScoreData = false;
		while(!HighScoreD.eof())
		{
			if(HighScoreD.eof() == true || CurrentLocation == 11)
				break;
			*TopPlayerName[CurrentLocation] = "";
			while(HighScoreD.peek() != '-')
			{
				*TopPlayerName[CurrentLocation] += char(HighScoreD.get());
			}
			HighScoreD.ignore();
			while(HighScoreD.peek() != ';')
			{
				temp += char(HighScoreD.get());
			}
			TopPlayerScore[CurrentLocation] = StringToInt(temp);
			HighScoreD.ignore();
			if(HighScoreD.peek() == '\n')
				HighScoreD.ignore(1);

			++CurrentLocation;	
			temp = "";
		}
	}
	HighScoreD.close();
}

bool Interface :: CheckHighScoreEntry()
{
	ReadHighScoreData = true;
	CurrentLocation = 0;
	PerviousLocation = 0;
	while(TopPlayerName[CurrentLocation] != NULL)
	{
		if(g_GameManager->GetScore() <= TopPlayerScore[CurrentLocation])
		{
			PerviousLocation = CurrentLocation;
			++CurrentLocation;
		}
		else
			return true;
	}
	return false;
}

void Interface ::StoreHighScoreFile()
{
	string temp;
	ofstream HighScoreD;
	HighScoreD.open("Data/HighScore.txt");
	for(int a = 0; a < 10; a++)
	{
		for(int b = 10000000; b>9; b/=10)
				if(TopPlayerScore[a] < b)
					temp += "0";
		HighScoreD << *TopPlayerName[a] << "-" << temp << TopPlayerScore[a] << ";";
		if(a != 9)
			HighScoreD << '\n';
		temp = "";
	}
	HighScoreD.close();
}

void Interface :: HighScoreEntry()
{
	
	if((GetAsyncKeyState(VK_RETURN) & 0x8000) == 0x8000 || keys[VK_ESCAPE])
	{
		if(tempStoreName != "")
		{
			glColor4f(1,1,1,1);
			g_GameStatus = HIGH_SCORE;	
			PromptEnterName = false;

			string tempTopName[10][10];
			int tempTopScore[10];
			int tempLocation = 0;
			CurrentLocation = 0;
			bool Entered = false;

			while(tempLocation < 10 && Entered == false)
			{
				if(MyScore >= TopPlayerScore[CurrentLocation] && Entered == false)
				{
					*tempTopName[tempLocation] = tempStoreName;
					tempTopScore[tempLocation] = MyScore; //g_GameManager->GetScore();
					Entered = true;
				}
				else
				{
					*tempTopName[tempLocation] = *TopPlayerName[CurrentLocation];
					tempTopScore[tempLocation] = TopPlayerScore[CurrentLocation];
					++CurrentLocation;
				}
				++tempLocation;
			}
			for(int a=tempLocation; a<10; a++)
			{
				*tempTopName[a] = *TopPlayerName[CurrentLocation];
				tempTopScore[a] = TopPlayerScore[CurrentLocation];
				++CurrentLocation;
			}
			for(int a=0; a<10; a++)
			{
				*TopPlayerName[a] = *tempTopName[a];
				TopPlayerScore[a] = tempTopScore[a];
			}

			StoreHighScoreFile();
			ReadHighScoreData = true;
			ReadHighScoreFile();
	
			return;
		}
		else
			PromptEnterName = true;
	}

	if(keys[VK_BACK])
	{
		if(tempStoreName.size() != 0)
			tempStoreName = tempStoreName.substr(0,tempStoreName.size()-1);
		keys[VK_BACK] = false;
	}

	for(char CharKeyCheck = 32; CharKeyCheck<91; CharKeyCheck++)
	{
		if(CharKeyCheck == ';' || CharKeyCheck == '-')
			keys[CharKeyCheck] = false;
		else if(keys[CharKeyCheck] && keys[VK_SHIFT])
		{
			if(int(tempStoreName.size()) < PlayerNameEntryLimit)
				tempStoreName += CharKeyCheck;
			else
				tempStoreName[PlayerNameEntryLimit-1] = CharKeyCheck;
			keys[CharKeyCheck] = false;
		}
		else if(keys[CharKeyCheck] && ((CharKeyCheck >= '0' && CharKeyCheck <= '9') || CharKeyCheck == 32))
		{
			if(int(tempStoreName.size()) < PlayerNameEntryLimit)
				tempStoreName += CharKeyCheck;
			else
				tempStoreName[PlayerNameEntryLimit-1] = CharKeyCheck;
			keys[CharKeyCheck] = false;
		}
		else if((GetAsyncKeyState(CharKeyCheck) & 0x8000) == 0x8000)
		{
			if(int(tempStoreName.size()) < PlayerNameEntryLimit)
				tempStoreName += CharKeyCheck + 32;
			else
				tempStoreName[PlayerNameEntryLimit-1] = CharKeyCheck + 32;
			keys[CharKeyCheck] = false;
			while((GetAsyncKeyState(CharKeyCheck) & 0x8000) == 0x8000);
		}
	}

	g_Font->Print("Congratulation", -200, 200, 3.0f, 0.0f,0);
	g_Font->Print("New High Score", -75, 100, 1.5f, 0.0f,0);
	g_Font->Print("Enter Name: " + tempStoreName, -200,0, 1.5f,0,0);	

	if(PromptEnterName == true)
		g_Font->Print("Please Enter A Name", -100, -275, 1.5f, 0.0f,0);
}

void Interface :: DisplayHighScore()
{
	Vector2D DisplayRankPos(-150,200);
	CurrentLocation = 0;
	int tempScore;
	string temp;

	char scoreBuffer [100];
	char rankBuffer [20];

	if(g_GameStatus == HIGH_SCORE)
	{
		BlendQuad();
		
		glColor4f(1,1,1,1);
		g_Font->Print("Hall Of Fame", -80,250, 1.5f,0,0);
		g_Font->Print("Hit 'X' or 'Esc' to quit", -160,-250, 1.5f,0,0);

		while(TopPlayerName[CurrentLocation] != NULL)
		{
			sprintf(scoreBuffer, "%i", TopPlayerScore[CurrentLocation]);
			sprintf(rankBuffer, "%i", CurrentLocation+1);

			for(int a = 10000000; a>9; a/=10)
				if(TopPlayerScore[CurrentLocation] < a)
					temp += "0";

			g_Font->Print(string(rankBuffer) + ". " + *TopPlayerName[CurrentLocation], DisplayRankPos.x-40,DisplayRankPos.y, 1.5f,0,0);
			g_Font->Print(temp + string(scoreBuffer), DisplayRankPos.x + 220,DisplayRankPos.y, 1.5f,0,0);
			DisplayRankPos += Vector2D(0,-45);
			if(++CurrentLocation > 9)
				break;
			temp = "";
		}
	}
	else if(g_GameStatus == HIGH_SCORE_ENTRY)
	{
		tempScore = MyScore;
		sprintf(scoreBuffer, "%i", tempScore);

		for(int a = 10000000; a>9; a/=10)
				if(tempScore < a)
					temp += "0";

		g_Font->Print(temp + string(scoreBuffer), 0,50, 1.5f,0,0);
	}
}

void Interface :: Flatten()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, Window_Res_W, 0, Window_Res_H, -1, 1);	// Create Ortho Resoultion View (0,0 At Top Left)
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void Interface :: DeFlatten()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Interface :: HUD()
{
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	g_TexLoader->DrawXY(m_iHudTexId);
	glPopMatrix();
}

void Interface :: BlendQuad()
{
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	glBegin(GL_QUADS);
	glVertex2f(-Window_Res_W/2, -Window_Res_H/2);
	glVertex2f(Window_Res_W/2, -Window_Res_H/2);
	glVertex2f(Window_Res_W/2, Window_Res_H/2);
	glVertex2f(-Window_Res_W/2, Window_Res_H/2);
	glEnd();
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void Interface :: RenderBackGround()
{
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	g_TexLoader->DrawXY(m_iBackGroundTexId);
	glPopMatrix();
}

void Interface :: GameFinishInPut()
{	
	if((keys['X'] || keys['Z'] || keys[VK_RETURN]) && (g_InGameStatus == LOSE || g_InGameStatus == COMPLETE))
	{
		keys['X'] = keys['Z'] = keys[VK_RETURN] = false;
		Sleep(1000);

		g_AEngine->SwitchBGMOff();
		g_AEngine->StopSegment(BGM, true);
		g_AEngine->LoadSegment(MainBGM_Path.c_str(), true, BGM);
		if(CheckHighScoreEntry())
			g_GameStatus = HIGH_SCORE_ENTRY;
		else
			g_GameStatus = HIGH_SCORE;
			//MenuScreen_ID = MAIN_MENU_SCREEN;
		g_InGameStatus = NONE;
	}
}

void Interface :: GameOverScreen()
{
	if(g_GameManager->GetLife() >= 0)
		g_TexLoader->DrawXY(m_iGoodEnd);
	else
		g_TexLoader->DrawXY(m_iBadEnd);
	CurrentLevelSelected = 0;
}
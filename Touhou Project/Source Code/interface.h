#pragma once
/* Notes
There is some problems with the print function
apparently the tex coordinates are not correct. it will be affected by the things around it
and it can only successfully print 1 character only
make changes * medium pirority
*/

#include "singleton.h"

enum ButtonStatus {START_BUTTON, SELECT_STAGE_BUTTON, HIGH_SCORE_BUTTON, HELP_BUTTON, EXIT_BUTTON, CONTINUE_BUTTON, RETURN_MAIN_BUTTON, YES_BUTTON, NO_BUTTON, RETURN_BUTTON, GENERAL_HELP_BUTTON, CONTROL_HELP_BUTTON, INTERFACE_HELP_BUTTON, SCREENSHOT_HELP_BUTTON, CREDITS_HELP_BUTTON, RIGHT_ARROW_BUTTON, LEFT_ARROW_BUTTON, CHARACTER_WEAPON_PORTRAIT_DISPLAY, NONE_BUTTON};
enum MenuStatus{MAIN_MENU_SCREEN, STAGE_SELECTION_SCREEN, CHARACTER_SELECTION_SCREEN, WEAPON_SELECTION_SCREEN, HELP_SCREEN, NONE_SCREEN};
enum DisplayStatus{HELP_DISPLAY, NONE_DISPLAY};
enum PromptTitles{ARE_YOU_SURE_PROMPT, SELECT_CHARACTER_TITLE, SELECT_STAGE_TITLE, HELP_TITLE, PRESS_Z_TO_SELECT_HELP, NONE_TITLE};
enum HUDInfoDisplay{LIFE_LEFT_HUD, BOMB_LEFT_HUD, NONE_HUD};

class Interface : public Singleton <Interface>
{
	string TopPlayerName[10][10];
	int TopPlayerScore[10];

	int	m_iBackGroundTexId;
	int m_iHudTexId;
	int m_iMainMenuButtons;
	int m_iPauseMenuButtons;
	int m_iHelpMenuButtons;
	int m_iPromptOptionsButtons;
	int m_iDirButtons;
	int m_iWeaponScreenShot[2][2];
	int * m_iCharacterPortrait;
	int * m_iHelpsDisplay;
	int m_iPromptTitles;
	int m_iLifeBombStar;

	int m_iBadEnd;
	int m_iGoodEnd;

	Vector2D ButtonSize;
	Vector2D PromptTitleSize;

	int MinCharacterSelection;
	int MaxCharacterSelection;
	int MinWeaponSelection;
	int MaxWeaponSelection;
	int WeaponChoice;
	int CharacterChoice;
	
	bool PromptEnterName;
	bool PromptQuit;
	bool DisplayHelp;
	bool MouseCenter;
	bool OnSoundSelectEffect;
	bool ReadHighScoreData;

	int SelectSFX_ID;
	int CancelSFX_ID;
	string MainBGM_Path;

	int DirArrButton;
	string tempStoreName;

	int CurrentLocation;
	int PerviousLocation;
	int PlayerNameEntryLimit;

	int MaxStage;
	int CurrentLevelSelected;
	char stageLevel[20];

	GLuint m_FontBase;
	GLuint MenuScreen_ID;
	GLuint Help_ID;
	GLuint Button_ID;
	GLuint Hover_ID;
	GLuint tempHover_ID;

	Vector2D mousePos;
	Vector2D mousePosRecord;
	Vector2D vec2MenuPosition[5];
	Vector2D vec2DisplayPosition[NONE_DISPLAY];
	Vector2D vec2ButtonPosition[static_cast<int>(NONE_BUTTON)];
public:
	Interface();
	~Interface();
	bool Init();												// just init >.<

	void MainMenu();			// Display Main Menu
	void HighScore();			// Display High Score
	void Pause();				// Display Pause Menu
	void PromptQuitSelection();	// Display Prompt Question
	void PauseMenu();			// Choose to Display Pause Menu or Prompt Question
	void HelpDisplay();			// Display Help
	void GameOverScreen();		// Display Game Over Screen
	void MainSelectionScreen();	// Choose to Display Main Menu, Stage Selection or Character Selection

	void StageSelection();		// Select Stage
	int GetSelectedStage();		// Return the Selected Stage

	void CharacterSelection();	// Select the Character
	int GetSelectedCharacter();	// Return the Selected Character
	void WeaponSelection();		// Select the Weapon
	int GetSelectedWeapon();	// Return the Selected Weapon
		
	void MainMenuProcessInput();
	void CharacterSelectionProcessInput();
	void WeaponSelectionProcessInput();
	void StageSelectionProcessInput();
	void HelpProcessInput();
	void PauseMenuProcessInput();
	void PromptMenuProcessInput();		// Yes or No
	void HighScoreEntryProcessInput();

	int StringToInt(string);
	void ReadHighScoreFile();
	void StoreHighScoreFile();
	void DisplayHighScore();
	void HighScoreEntry();
	bool CheckHighScoreEntry();	// See if Player's score meet the min requirement to enter in the highscore

	void CenterMousePosition();
	void ProcessInput();
	void KeyboardProcessInput();

	void BlendQuad();

	void Flatten();
	void DeFlatten();
	void HUD();																		// the HUD drawing function

	void RenderBackGround();

	int * GetHighScore()
	{
		return TopPlayerScore;
	}
	void GameFinishInPut();
};

extern Interface * g_Interface;	// this is to allow text printing anywhere.
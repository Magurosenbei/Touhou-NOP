#include "stdafx.h"
#include "HeadsupDisplay.h" 

HeadsupDisplay * gHUD;

HeadsupDisplay::HeadsupDisplay()
{
}
bool HeadsupDisplay::Init()
{
	highscore  = g_Interface->GetHighScore();
	counter = 9;
	status = Calm;
	Arrow = g_TexLoader->AddTexture("Textures/Others/Arrow.png");
	Noarrow = g_TexLoader->AddTexture("Textures/Others/NoArrow.png");
	Compass = g_TexLoader->AddTexture("Textures/Others/Compass.png");
	LifeStar = g_TexLoader->AddTexture("Textures/GUI/stars.png");
	Weathersprites = g_TexLoader->AddTexture("Textures/Others/WeatherSprites.png");
	volume = g_TexLoader->AddTexture("Textures/Others/Sound.png");
	isplaying = true; symbol = false; switchpic = false;
	return true;
}
void HeadsupDisplay::DisplayHighScore()
{
	int tempScore = 0; string temp;
	char scoreBuffer[100];

	if( g_GameManager->GetScore() < highscore[0])
		tempScore = highscore[0];
	else
		tempScore = g_GameManager->GetScore();
	sprintf(scoreBuffer, "%i", tempScore);
		for(int a = 10000000; a>9; a/=10)
				if(tempScore < a)
					temp += "0";

		g_Font->Print("HighScore: " + temp + string(scoreBuffer), 150,220, 1.2f,0,0);
}
void HeadsupDisplay::DisplayScore()
{
	char buffer [20];
	sprintf(buffer, "%i", g_GameManager->GetScore());

	string temp;
	for(int a = 10000000; a>9; a/=10)
		if(g_GameManager->GetScore() < a)
			temp += "0";

	
	g_Font->Print("Score:     " + temp + buffer, 150,180, 1.2f,0,0);
}
void HeadsupDisplay::ShowBombLeft()
{
	g_Font->Print("Bomb: ", 150,82.5f, 1.2f,0,0);
	int tempBomb = g_GameManager->GetBomb();
	if(tempBomb > 0)
	{
		if(tempBomb >= 7)
			tempBomb = 7;
		glColor4f(1,1,1,1);
		glPushMatrix();
		glTranslatef(240, 95, 0);
		for(int a = 0; a < tempBomb; a++)
		{
			g_TexLoader->DrawXY(LifeStar, 1, 28);
			glTranslatef(22.0f,0,0);
		}
		glPopMatrix();
	}
}
void HeadsupDisplay::ShowLifeLeft()
{
	g_Font->Print("Life: ", 150,112.5f, 1.2f,0,0);
	int tempLife = g_GameManager->GetLife();
	if(tempLife > 0)
	{
		if(tempLife >= 7)
			tempLife = 7;
		glColor4f(1,1,1,1);
		glPushMatrix();
		glTranslatef(240, 125, 0);
		for(int a = 0; a < tempLife; a++)
		{
			g_TexLoader->DrawXY(LifeStar,0 , 28);
			glTranslatef(22.0f,0,0);
		}
		glPopMatrix();
	}
}
void HeadsupDisplay::ShowPowerLevel()
{
	char powerBuffer[20];
	sprintf(powerBuffer, "%f", g_GameManager->GetPower());

	g_Font->Print("Power: " + string(powerBuffer), 150, 55, 1.2f,0,0);
}
void HeadsupDisplay::DisplayGrazeCounter()
{
		char grazeBuffer[20];
	sprintf(grazeBuffer, "%i", g_GameManager->GetGraze());

	g_Font->Print("Graze: " + string(grazeBuffer), 150, 25, 1.2f,0,0);
}
void HeadsupDisplay::RenderCompass(bool wind, float angle, int windspeed,int background)
{
	char scoreBuffer[100];
	if(!wind)
		windspeed = 0;
	ChangeWeather(windspeed);
	sprintf(scoreBuffer, "%i", windspeed);
	glPushMatrix();
		glTranslatef(230, -150, 0);
			g_TexLoader->DrawXY(Weathersprites,status,109);
			g_TexLoader->DrawXY(Compass,0,200);
		glPushMatrix();
			if(wind)
			{
				glRotatef(angle-90,0,0,1);
				g_TexLoader->DrawXY(Arrow,0,24);
			}
		glPopMatrix();
		glTranslatef(2.5,0,0);
		g_TexLoader->DrawXY(Noarrow,0,24);
	glPopMatrix();
	
	glColor3f(0,0,0);
	if(wind)
		if(windspeed > 9)
		g_Font->Print((string)scoreBuffer,219,-160,1.0f,0,0);
		else
		g_Font->Print("0"+(string)scoreBuffer,219,-160,1.0f,0,0);
	glColor3f(1,1,1);

	DisplayWeatherStatus((string)scoreBuffer);
}
void HeadsupDisplay::DisplayAll()
{
	DisplayHighScore();
	DisplayScore();
	ShowBombLeft();
	ShowLifeLeft();
	ShowPowerLevel();
	DisplayGrazeCounter();
	RenderCompass(true,(float)g_WeatherSys->GetCurrentWeather().angle,g_WeatherSys->GetCurrentWeather().wind,0);
	UpdateLifebar();
	RenderVolumeControl();
}
void HeadsupDisplay::ChangeWeather(int speed)
{
	if(speed < 10)
	{
		switch(status)
		{
			case Windy: status = WindytoCalm; break;
			case Storm: status = StormtoCalm; break;
			case WindytoCalm: if(g_TimeManager->GetFPS() %ChangeSpeed == 0)
								status = Calm; break;
			case StormtoCalm: if(g_TimeManager->GetFPS() %ChangeSpeed == 0)
								status = Calm; break;
		}
	}
	else if(speed > 9 && speed < 20 )
	{
		switch(status)
		{
			case Calm: status = CalmtoWindy; break;
			case Storm: status = StormtoWindy; break;
			case CalmtoWindy: if(g_TimeManager->GetFPS() %ChangeSpeed == 0)
								status = Windy; break;
			case StormtoWindy: if(g_TimeManager->GetFPS() %ChangeSpeed == 0)
								status = Windy; break;
		}
	}
	else
	{
		switch(status)
		{
			case Calm: status = CalmtoStorm; break;
			case Windy: status = WindytoStorm; break;
			case CalmtoStorm: if(g_TimeManager->GetFPS() %ChangeSpeed == 0 )
								status = Storm; break;
			case WindytoStorm: if(g_TimeManager->GetFPS() %ChangeSpeed == 0)
								status = Storm; break;
		}
	}

}
void HeadsupDisplay::DisplayWeatherStatus(string speed)
{
	if( status == Calm || status == Windy || status == Storm)
	{
		switch (status)
		{
			case Calm:g_Font->Print("Weather: Calm", 150, -5, 1.2f,0,0);break;
			case Windy:g_Font->Print("Weather: Windy", 150, -5, 1.2f,0,0);break;
			case Storm:g_Font->Print("Weather: Storm", 150, -5, 1.2f,0,0);break;
		}
		g_Font->Print("Speed: "+ speed,150, -35, 1.2f,0,0);
	}
	else
	{
		glColor3f(1,1,0);
		g_Font->Print("Weather: Changing", 150, -5, 1.2f,0,0);
		glColor3f(1,1,1);
		g_Font->Print("Speed: --",150, -35, 1.2f,0,0);
	}
}
void HeadsupDisplay::RenderLifeBar(string Number)
{
	glColor3f(1,1,0);
	glPushMatrix();
		glPushMatrix();
			glDisable(GL_TEXTURE_2D);
				glBegin(GL_QUADS);
				glVertex3f(-300,250,0);
				glVertex3f(-300,245,0);
				glVertex3f(C_Length,245,0);
				glVertex3f(C_Length,250,0);
				glEnd();
			glEnable(GL_TEXTURE_2D);
		glPopMatrix();
	glPopMatrix();

	g_Font->Print(Number,-330, 237.5, 1.2f,0,0);
	glColor3f(0,0,0);
}
void HeadsupDisplay::RenderVolumeControl()
{
	glPushMatrix();
	glTranslatef(375, 270,0);
	g_TexLoader->DrawXY(volume,symbol,19);
	glPopMatrix();

	if(keys['M'] == true)
		if(isplaying)
		{
			g_AEngine->SwitchBGMOff();
			DebugPrint("Switching Audio OFf");
			isplaying = false;
			symbol = true;
		}
		else
		{
			g_AEngine->SwitchBGMOn();
			DebugPrint("Switching Audio On");
			isplaying = true;
			symbol = false;
		}

}
void HeadsupDisplay::RenderBackground()
{
	glPushMatrix();
		glTranslatef(-15,0,0);
		g_TexLoader->DrawXY(mainpic);
	glPopMatrix();
}
void HeadsupDisplay::UpdateLifebar()
{
	if((g_InGameStatus != BOSS && boss == NULL) || boss->GetHp() <= 0)
		return;

	char scoreBuffer[20];
	int NoOfBars = ((boss->GetHp()-1)/Life);
		if(NoOfBars < 0)
			NoOfBars = 0;
	sprintf(scoreBuffer, "%i", NoOfBars);

	int hp =boss->GetHp()% Life;

	if(hp > 0 )
	C_Length = float((hp) * 100/ Life*4 - FullBar);
	else
	C_Length = 100;

	if(NoOfBars < 10)
		RenderLifeBar("0"+(string)scoreBuffer);
	else
		RenderLifeBar((string)scoreBuffer);
}
void HeadsupDisplay::Getboss(Enemy *Boss)
{
	this->boss = Boss;
}
void HeadsupDisplay::RecieveImage(int main, int submain)
{
	mainpic= main;
	movable = submain;
}
int HeadsupDisplay::StringToInt(string temp)
{
	int intReturn; 
	intReturn = atoi(temp.c_str()); 

	return(intReturn); 
}
#include "stdafx.h"
#include "GameMouse.h"
#include "TimeFrameManager.h"

InGameMouse * g_GameMouse;

InGameMouse :: InGameMouse()
{
	m_iMaxFrame = 4;
	m_iAnimationFrame = 0;
}
void InGameMouse :: Init()
{
	m_iTexID = g_TexLoader->AddTexture("Textures/Mouse/Mouse.png");
}

void InGameMouse :: Render()
{
	glDepthMask(GL_FALSE);
	glPushMatrix();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslatef(vec2Position.x, vec2Position.y, 0.0f);
	g_TexLoader->DrawXY(m_iTexID, m_iAnimationFrame, 32.0f);

	glPopMatrix();
	glDepthMask(GL_TRUE);
}

void InGameMouse :: SetPosition(Vector2D & temp)
{
	vec2Position = temp;
}

void InGameMouse :: Update()
{
	if(g_TimeManager->GetFPS() % 5 == 0)
	(m_iAnimationFrame < m_iMaxFrame - 1)? m_iAnimationFrame++ : m_iAnimationFrame = 0;
}

void InGameMouse :: ProcessInput()
{
	GetCursorPos(&m_MousePoint);
	if(m_MousePoint.x == WINDOW_MID_X && m_MousePoint.y == WINDOW_MID_Y)
		return;

	SetCursorPos(WINDOW_MID_X, WINDOW_MID_Y);
	
	if(vec2Position.x < MOUSE_LIMIT_MAX_X && (WINDOW_MID_X - m_MousePoint.x) < 0)
		vec2Position.x -= static_cast<float>(WINDOW_MID_X - m_MousePoint.x);
	else if(vec2Position.x > MOUSE_LIMIT_MIN_X && (WINDOW_MID_X - m_MousePoint.x) > 0)
		vec2Position.x -= static_cast<float>(WINDOW_MID_X - m_MousePoint.x);

	(vec2Position.x > 400)? vec2Position.x = 400: NULL;
	(vec2Position.x < -400)? vec2Position.x = -400: NULL;

	if(vec2Position.y <= MOUSE_LIMIT_MAX_Y && (WINDOW_MID_Y - m_MousePoint.y) < 0)
		vec2Position.y += static_cast<float>(WINDOW_MID_Y - m_MousePoint.y);
	else if(vec2Position.y >= MOUSE_LIMIT_MIN_Y && (WINDOW_MID_Y - m_MousePoint.y) > 0)
		vec2Position.y += static_cast<float>(WINDOW_MID_Y - m_MousePoint.y);

	(vec2Position.y > 300)? vec2Position.y = 300: NULL;
	(vec2Position.y < -300)? vec2Position.y = -300: NULL;
}

Vector2D InGameMouse :: GetGameMousePosition()
{
	return vec2Position;
}
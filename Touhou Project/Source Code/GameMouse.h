#pragma once

#include "singleton.h"

#define WINDOW_MID_X (Window_Res_W >> 1)
#define WINDOW_MID_Y (Window_Res_H >> 1)

#define MOUSE_LIMIT_MAX_X 400
#define MOUSE_LIMIT_MIN_X -400

#define MOUSE_LIMIT_MAX_Y 300
#define MOUSE_LIMIT_MIN_Y -300

class InGameMouse : public Singleton <InGameMouse>
{
	Vector2D	vec2Position;
	POINT		m_MousePoint;

	int m_iTexID;

	int			m_iAnimationFrame;
	int			m_iMaxFrame;
public:
	InGameMouse();
	void Init();
	void Render();
	void SetPosition(Vector2D &);
	void Update();
	void ProcessInput();
	Vector2D GetGameMousePosition();
};

extern InGameMouse * g_GameMouse;
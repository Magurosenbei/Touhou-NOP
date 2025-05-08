#include "stdafx.h"
#include "boss.h"

void Boss :: Update()
{
	if((g_TimeManager->GetFPS() & 1) == 0)	// change picture every 2 Frames
		(m_iAnimationFrame > 6)? m_iAnimationFrame = 0 : m_iAnimationFrame++;

	if(g_TimeManager->GetFPS() % 30 != 0)
		return;
		for(Degree = 0; Degree < PIE * 2.0f;)
		{
			float x = vecPosition.x + (50.0f * cos(Degree));
			float y = vecPosition.y + (50.0f * sin(Degree));
			Vector Velocity = (Vector(x, y, 0) - vecPosition).Normalize();
			//g_Engine->InsertEnemyBullet(new Bullet(vecPosition, Velocity, Vector(), Degree, 2.0f, false, g_RManager->GetBulletSprite(), 1.0f,Vector(1,0.6f,0.6f,1)));
			//g_Engine->InsertEnemyBullet(new Bullet(vecPosition + Vector(g_TimeManager->GetFPS() - 110.0f,0,0), Velocity, Vector(), Degree, 2.0f, 2.0f, false, g_RManager->GetBulletSprite(), Vector(0.6f,1,0.6f,1)));
			//g_Engine->InsertEnemyBullet(new Bullet(vecPosition + Vector(g_TimeManager->GetFPS() + 80.0f,0,0), -Velocity, Vector(), Degree, 2.0f, 2.0f, false, g_RManager->GetBulletSprite(), Vector(0.6f,0.6f,1,1)));
			
			Degree += 10.0f / 180.0f * PIE;
		}
		if(Degree > 2*PIE)
			Degree = 0;
	/*if(m_iStorePoint - m_iHitPoints == m_iInterval)
	{
		m_iStorePoint = m_iHitPoints;
		if(vecVelocity.Length() < 0.1f)
			m_dDirection = NORMAL;
		if(m_dDirection == NORMAL)
		{
			if(vecPosition.x < 0)
				m_dDirection = RIGHT;
			else
				m_dDirection = LEFT;
		}
	}
	switch(m_dDirection)
	{
	case RIGHT: if(vecAcceleration.x == -0.01f)
					break;
				vecVelocity.x = 0.5f;
				vecAcceleration.x = -0.01f;
				break;

	case LEFT:	if(vecAcceleration.x == 0.01f)
					break;
				vecVelocity.x = -0.5f;
				vecAcceleration.x = 0.01f;
				break;

	case NORMAL:if(vecAcceleration.x == 0.00f)
					break;
				vecVelocity.x = 0.0f;
				vecAcceleration.x = 0.0f;
				break;
	}*/
}
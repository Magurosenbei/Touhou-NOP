#include "stdafx.h"
#include "enemy.h"

Enemy :: ~Enemy()
{
	
}

void Enemy :: Init()
{

}
void Enemy :: AddToControl(Enemy*temp)
{
	Control.push_back(temp);
}
void Enemy :: Update()
{
	//if((g_TimeManager->GetFPS() & 1) == 0)	// change picture every 2 Frames
	//	(m_iAnimationFrame > 6)? m_iAnimationFrame = 0 : m_iAnimationFrame++;
}

void Enemy :: UpdateVectors()
{
	Onhover();
	if(g_rotate == true)
	{
		EnemyRotate();
		return;
	}
	vecPosition += vecVelocity * 0.25;
	UpdateDistance();

	if(vecVelocity.Length() > 0)
	{
		vecAcceleration = -vecVelocity.Normalize() * 0.01f;
		vecVelocity += vecAcceleration;
		if(vecVelocity.Length() < 0.1f)
		vecVelocity = Vector();
	}
}
void Enemy :: Render()
{
	glDepthMask(GL_FALSE);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPushMatrix();
		glTranslatef(vecPosition.x, vecPosition.y, vecPosition.z);
		g_TexLoader->DrawXY(m_iEnemySpriteID, m_iAnimationFrame, m_fSpriteWidth);
	glPopMatrix();
	glDepthMask(GL_TRUE);
}
void Enemy :: GoToPosition(Vector pos)
{
	pos  = HeadToPosition;
	Vector temp = HeadToPosition - vecPosition;
	distance = sqrt(temp.x * temp.x + temp.y * temp.y);
	vecVelocity = temp/50;	
}
void Enemy :: CollisionReaction()
{
	g_GameManager->AddScore();
	if(m_iHitPoints <= 0)	// if died remove from play
	{
		g_AEngine->PlaySegment(EXPLODE, 0);
		if( g_InGameStatus == BOSS )	g_ItemSys->DropBoss(vecPosition);
		else							g_ItemSys->DropEnemy(vecPosition);
		m_bRemove = true;
		m_bRelease = true;
	}
}

void Enemy :: ApplyDamage(int Dmg)
{
	m_iHitPoints -= Dmg;
	g_AEngine->PlaySegment(HIT, 0);
	g_AEngine->Set3DSoundPos(HIT, vecPosition.Normalize());
}

int Enemy ::  GetHp()
{
	int TotalHp = 0;
	if(Control.empty())
		return m_iHitPoints;
	else
		for(vector<Enemy*>::iterator g2 = Control.begin(); g2 < Control.end(); g2++)
			TotalHp +=(*g2)->GetHp();
		return TotalHp;
}
int Enemy ::GetMaxHp()
{
	return m_itotalhitpoints;
}
float Enemy ::getPercentage()
{
	int TotalHp = 0;int CurrentHP = 0;
	if(Control.empty())
		return (float)(m_iHitPoints* 100/m_itotalhitpoints);
	else
		for(vector<Enemy*>::iterator g2 = Control.begin(); g2 < Control.end(); g2++)
		{
			CurrentHP +=(*g2)->GetHp();
			TotalHp += (*g2)->GetMaxHp();
		}
		return (float)(CurrentHP* 100/TotalHp);

}
void Enemy ::Onhover()
{
	if(on_hover && vecVelocity == Vector())
	{
		if(count == true)
		{
			if(onCount > 10)
			{
				vecVelocity.y+= 0.5;
				count = false;
				onCount= 0;
			}
			onCount++;
		}
		else
		{
			if(onCount > 10)
			{
				vecVelocity.y-= 0.5;
				count = true;
				onCount = 0;
			}
			onCount++;
		}
	}
}
void Enemy ::Sethover()
{
	on_hover = true;
}
void Enemy ::OutOfBound()
{
	if(vecPosition.x > SCREEN_BORDER_MAX_X + 10 || vecPosition.x < SCREEN_BORDER_MIN_X - 10 || vecPosition.y > SCREEN_BORDER_MAX_Y + 10|| vecPosition.y < SCREEN_BORDER_MIN_Y - 10)
	{
		m_bRemove = true;
		m_bRelease = true;
	}
}

void Enemy ::EnemyRotate()
{
	if(angle == 0)
	{
		g_rotate = false;
		return;
	}
	float TempAngle = currentangle;
	vecPosition = vecPosition.RotateFromPoint(rotatepoint,PIE / 180.0f * TempAngle);
	vecPosition = (vecPosition - rotatepoint).Normalize();
	vecPosition = vecPosition * 100 + rotatepoint;

	(currentangle < 360)?currentangle+= 1:currentangle = 0;
	if(angle < -1)
		return;
	else
		angle--;
}
void Enemy ::SetRotate()
{
	g_rotate = true;
}
void Enemy ::OffRotate()
{
	g_rotate = false;
}
bool Enemy ::Checkdistance()
{
	if(distance  == 0)
		return true;
	else 
		return false;
}
void Enemy ::UpdateDistance()
{
	if(distance != 0)
	{
		Vector temp = HeadToPosition - vecPosition;
		distance = sqrt(temp.x * temp.x + temp.y * temp.y);
		if(distance > -2 && distance < 1 || distance < 2 && distance > -1)
		{
			distance = 0;
			HeadToPosition = Vector();
			vecVelocity = Vector();
		}
	}
}
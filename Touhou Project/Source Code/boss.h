#pragma once

#include "enemy.h"

/* Notes
This part may require heavy modding to make bullet patterns
*/


class Boss : public Enemy
{
	int m_iStorePoint;	//keep the amout of hp to count
	int m_iInterval;

	int m_clock;
	int m_iSetOfCards;	// number of spellcards

	Direction m_dDirection;

	float Degree;

public:
	Boss(Vector & Pos, Vector & Vel, Vector & Acc, float HitBox, int HP, int TextureID, float SpriteWidth, int NumberOfSpells)
		:	Enemy(Pos, Vel, Acc, HitBox, HP, TextureID, SpriteWidth),
			m_iSetOfCards(NumberOfSpells)
	{
		m_dDirection = NORMAL;
		m_iStorePoint = HP;
		m_iInterval = 10;
		Degree = 0;
	}

	//void Render();
	//void Init();
	void Update();
};
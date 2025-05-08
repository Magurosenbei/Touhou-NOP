#include "stdafx.h"
#include "grid.h"

void Grid :: Render()
{
	glDisable(GL_TEXTURE_2D);

	for(float i = -240; i <= 240; i += 10)
	{
		if(i == 0)
			glColor3ub(255, 0, 0);
		else
			glColor3ub(75, 95, 75);
	
		glBegin(GL_LINES);					
			glVertex3f(-240, i, -0.001f);					
			glVertex3f(240, i, -0.001f);
			glVertex3f(i, -280, -0.001f);							
			glVertex3f(i, 280, -0.001f);
		glEnd();

		if(i == 0)
			glColor3ub(0, 0, 255);
		else
			glColor3ub(125, 125, 75);
	}
	glEnable(GL_TEXTURE_2D);
}

void Grid :: Init()
{
	m_bCanTarget = false;
}

void Grid :: UpdateVectors()
{

}
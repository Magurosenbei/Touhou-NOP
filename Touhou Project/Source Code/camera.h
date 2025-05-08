#pragma once
/* Notes
This is the Camera
It possess FPS Capabilities
It may support 3rd person but you need to specify the target position every frame
You will need to know the following:

StrafeCamera()		//W A S D
Mouse_Move()		//detect mouse movements and apply changes to view
Keyboard_Input()	//name suggest
Look()				//just put this once in every frame preferbably before rendering anything
Position Camera()	//you have to put this in InitEngine at the start, only have to use once unless you want to change.

To make the camera move faster, change the defination from 0.08f to other number make sure its float-type
The rest can ignore	
*/
#include "singleton.h"
#define CAMERASPEED	0.08f				// The Camera Speed

class Camera : public Singleton <Camera>
{
protected:
	Vector mPos;	
	Vector mView;		
	Vector mUp;
public:
	Camera()
		:mPos(0,0,0), mView(0,0,0), mUp(0,0,0)
	{
	}
	void Strafe_Camera(float speed);
	void Mouse_Move(int wndWidth, int wndHeight);
	void Keyboard_Input();
	void Move_Camera(float speed);
	void Rotate_View(float speed);
	void Rotate(float angle, float x, float y, float z);
	void Look();
	void Position_Camera(float pos_x, float pos_y,	float pos_z,
		 				 float view_x, float view_y, float view_z,
						 float up_x,   float up_y,   float up_z);

};

extern Camera * g_Camera; 

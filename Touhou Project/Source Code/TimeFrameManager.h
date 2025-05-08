#pragma once

/* Notes
This Manages time Frame and the lags to fulfil 60 frames per second
use this to take record of keyframes to create your stages
*/
#include "singleton.h"

class TimeFrameManager : public Singleton <TimeFrameManager>
{
	int m_iFramePerSecond;		//	As name suggest	(int as no decimals needed)
	float m_fFrameInterval;		//	Time in between Frame, usually smaller than 0.05f 
	float m_fElapseTime;		//	How long passed sinced start of Game Engine

	float m_fLastTime;			//	Last Recorded Time
	float m_fLastFrameTime;		//	Last FrameDuration
	float m_fCurrentTime;		//	Now
	float m_fLastTimeFrame;
public:
	TimeFrameManager();
	~TimeFrameManager();
	int GetFPS();									// name suggests
	float GetInterval();							// return frame interval value
	float GetElapseTime();							// this returns actual duration
	float GetElapseTime(float Start, float End);	// this is for a custom approach incase you need to measure time from certian point to certian point
	DWORD GetThisTime();							// Get the time now, you have to store this somewhere to use the function above
	float GetLastTimeFrame();
	float GetLastTime();

	void UpdateTimeFrame();		//updates FPS and such must be activated in engine fuctions
};

extern TimeFrameManager * g_TimeManager;
#pragma warning (disable:4021)

#include "stdafx.h"
#include "TimeFrameManager.h"

TimeFrameManager * g_TimeManager;

TimeFrameManager :: TimeFrameManager()
{
	m_iFramePerSecond = 0;
	m_fLastTimeFrame = m_fFrameInterval = m_fElapseTime = m_fLastTime = m_fLastFrameTime = m_fCurrentTime = 0.0f;		
}

TimeFrameManager :: ~TimeFrameManager()
{

}

int TimeFrameManager :: GetFPS()
{
	return m_iFramePerSecond;
}

float TimeFrameManager :: GetInterval()
{
	return m_fFrameInterval;
}

float TimeFrameManager :: GetElapseTime()
{
	return m_fElapseTime;
}

float TimeFrameManager :: GetElapseTime(float Start, float End)
{
	return (End - Start);
}

DWORD TimeFrameManager :: GetThisTime()
{
	return GetTickCount();
}

float TimeFrameManager :: GetLastTimeFrame()
{
	return m_fLastTimeFrame;
}
float TimeFrameManager :: GetLastTime()
{
	return m_fLastTime;
}

void TimeFrameManager :: UpdateTimeFrame()
{
	m_fCurrentTime = timeGetTime() * 0.001f;
	m_fFrameInterval = m_fCurrentTime - m_fLastTimeFrame;
	m_iFramePerSecond++;
	if(m_fCurrentTime - m_fLastTime > 1.0f)
	{
		m_fLastTime = m_fCurrentTime;
		m_iFramePerSecond = 0;
		m_fElapseTime++;
	}
	m_fLastFrameTime = m_fFrameInterval;
	m_fLastTimeFrame = m_fCurrentTime;
}
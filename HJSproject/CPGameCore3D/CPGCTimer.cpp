#include "CPGCTimer.h"

const LARGE_INTEGER	SimpleHiResTimer::g_systemTimeFreq = getSystemFrequency();

LARGE_INTEGER getSystemFrequency()
{
	LARGE_INTEGER ret; 
	QueryPerformanceFrequency(&ret); 
	return ret;
}

CPGCMainTimer::CPGCMainTimer()
{
	reset();
}

CPGCMainTimer::~CPGCMainTimer()
{
}

SimpleHiResTimer::SimpleHiResTimer()
{
	reset();
}

SimpleHiResTimer::~SimpleHiResTimer()
{
}

void SimpleHiResTimer::start()
{
	if (!m_bActivated)
	{
		m_bActivated = true;
		m_bStopped = false;
		QueryPerformanceCounter(&m_initTick);
		m_prevTick = m_initTick;
	}
}

void SimpleHiResTimer::updateTimer()
{
	if (m_bActivated)
	{
		//프레임 시간 갱신
		QueryPerformanceCounter(&m_curTick);
		LONGLONG dt = m_curTick.QuadPart - m_prevTick.QuadPart;

		if (m_bStopped)
		{
			//정지 틱 변수 갱신
			m_pausedTicks.QuadPart += dt;
		}
		else
		{
			//프레임 틱 변수 및 타이머 작동 시간 갱신
			m_oneFrameTicks.QuadPart = dt;
			m_runningTicks.QuadPart += dt;

			m_iCnt++; //FPS 카운터

			//다음 프레임 준비
			m_prevTick = m_curTick;
		}

		//FPS 측정부
		m_elapseTicks.QuadPart += dt;

		if (m_elapseTicks.QuadPart > g_systemTimeFreq.QuadPart)
		{
			m_elapseTicks.QuadPart -= g_systemTimeFreq.QuadPart;
			m_iFPS = m_iCnt;
			m_iCnt = 0;
		}

	}
}

void SimpleHiResTimer::pause()
{
	if (!m_bStopped)
	{
		m_bStopped = true;
		m_iCnt = 0;
		m_oneFrameTicks.QuadPart = 0;
	}
}

void SimpleHiResTimer::resume()
{
	if (m_bStopped)
	{
		m_bStopped = false;
		m_prevTick.QuadPart = m_curTick.QuadPart - m_pausedTicks.QuadPart;
	}
}

void SimpleHiResTimer::end()
{
	if (m_bActivated) { reset(); }
}

void SimpleHiResTimer::reset()
{
	m_bActivated = false;

	m_initTick.QuadPart = 0;
	m_prevTick.QuadPart = 0;
	m_curTick.QuadPart = 0;

	m_bStopped = false;
	m_pausedTicks.QuadPart = 0;

	m_iCnt = 0;
	m_iFPS = 0;
	m_elapseTicks.QuadPart = 0;

	m_oneFrameTicks.QuadPart = 0;
	m_fOneFrameT = 0.0f;

	m_runningTicks.QuadPart = 0;
}

UINT SimpleHiResTimer::getFPS()
{
	return m_iFPS;
}

LARGE_INTEGER SimpleHiResTimer::getRunningTicks()
{
	return m_runningTicks;
}

LARGE_INTEGER SimpleHiResTimer::getOneFrameTicks()
{
	return m_oneFrameTicks;
}

float SimpleHiResTimer::getRunningTimeF()
{
	return (FLOAT)m_runningTicks.QuadPart / g_systemTimeFreq.QuadPart;
}

float SimpleHiResTimer::getOneFrameTimeF()
{
	return (FLOAT)m_oneFrameTicks.QuadPart / g_systemTimeFreq.QuadPart;
}

SimpleTimer::SimpleTimer()
{
	reset();
}

SimpleTimer::~SimpleTimer()
{
}

void SimpleTimer::start()
{
	if (!m_bActivated)
	{
		m_bActivated = true;
		m_bStopped = false;
		m_initTick = timeGetTime();
		m_prevTick = m_initTick;
	}
}

void SimpleTimer::updateTimer()
{
	if (m_bActivated)
	{
		//프레임 시간 갱신
		m_curTick = timeGetTime();
		DWORD dt = m_curTick - m_prevTick;

		if (m_bStopped)
		{
			//정지 틱 변수 갱신
			m_pausedTicks += dt;
		}
		else
		{
			//프레임 틱 변수 및 타이머 작동 시간 갱신
			m_oneFrameTicks = dt;
			m_runningTicks += dt;

			m_iCnt++; //FPS 카운터

			//다음 프레임 준비
			m_prevTick = m_curTick;
		}

		//FPS 측정부
		m_elapseTicks += dt;

		if (m_elapseTicks > DEFAULT_TIMER_TICK_LIMIT)
		{
			m_elapseTicks -= DEFAULT_TIMER_TICK_LIMIT;
			m_iFPS = m_iCnt;
			m_iCnt = 0;
		}
	}
}

void SimpleTimer::pause()
{
	if (!m_bStopped)
	{
		m_bStopped = true;
		m_iCnt = 0;
		m_oneFrameTicks = 0;
	}
}

void SimpleTimer::resume()
{
	if (m_bStopped)
	{
		m_bStopped = false;
		m_prevTick = m_curTick - m_pausedTicks;
	}
}

void SimpleTimer::end()
{
	if (m_bActivated) { reset(); }
}

void SimpleTimer::reset()
{
	m_bActivated = false;

	m_runningTicks = 0;
	m_oneFrameTicks = 0;

	m_initTick = 0;
	m_prevTick = 0;
	m_curTick = 0;

	m_bStopped = true;
	m_pausedTicks = 0;

	m_iCnt = 0;
	m_iFPS = 0;
	m_elapseTicks = 0;

	m_oneFrameTicks = 0;
	m_fOneFrameT = 0.0f;

	m_runningTicks = 0;
}

DWORD SimpleTimer::getFPS()
{
	return m_iFPS;
}

DWORD SimpleTimer::getRunningTicks()
{
	return m_runningTicks;
}

DWORD SimpleTimer::getOneFrameTicks()
{
	return m_oneFrameTicks;
}

FLOAT SimpleTimer::getRunningTimeF()
{
	return (FLOAT)m_runningTicks / 1000;
}

FLOAT SimpleTimer::getOneFrameTimeF()
{
	return (FLOAT)m_oneFrameTicks / 1000;
}
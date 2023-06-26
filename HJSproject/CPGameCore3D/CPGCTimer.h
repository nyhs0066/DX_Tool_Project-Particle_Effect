//////////////////////////////////////////////////
//
// CPGCTimer.h
//		게임의 시간을 담당하는 헤더.
//		1.	시계 초기화 이후 흐른 전체시간 : m_curTick - m_initTick
//		2.	1프레임 시간 : m_curTick - m_prevTick
//		3.	프레임 정지 시간 보정 : m_prevTick + m_pausedTicks
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"

#define DEFAULT_TIMER_TICK_LIMIT			1000

LARGE_INTEGER getSystemFrequency();

class SimpleHiResTimer
{
protected:
	static const LARGE_INTEGER		g_systemTimeFreq;

	//타이머 활성화 여부
	bool							m_bActivated;

	//초기틱, 이전 측정 틱, 현재 특정 틱
	LARGE_INTEGER					m_initTick;
	LARGE_INTEGER					m_prevTick;
	LARGE_INTEGER					m_curTick;

	//일시정지 기능을 위한 부분
	bool							m_bStopped;
	LARGE_INTEGER					m_pausedTicks;

	//초당 프레임 수 측정을 위한 부분
	UINT							m_iCnt;
	UINT							m_iFPS;
	LARGE_INTEGER					m_elapseTicks;

	//1프레임 시간
	LARGE_INTEGER					m_oneFrameTicks;
	FLOAT							m_fOneFrameT;

	//타이머 작동 시간
	LARGE_INTEGER					m_runningTicks;

public:
	SimpleHiResTimer();
	virtual ~SimpleHiResTimer();

	void							start();
	void							updateTimer();
	void							pause();
	void							resume();
	void							end();
	void							reset();

	UINT							getFPS();

	LARGE_INTEGER					getRunningTicks();
	LARGE_INTEGER					getOneFrameTicks();
	float							getRunningTimeF();
	float							getOneFrameTimeF();
};

//winmm.lib 필요
class SimpleTimer
{
protected:
	//타이머 활성화 여부
	bool							m_bActivated;

	//초기틱, 이전 측정 틱, 현재 특정 틱
	DWORD							m_initTick;
	DWORD							m_prevTick;
	DWORD							m_curTick;

	//일시정지 기능을 위한 부분
	bool							m_bStopped;
	DWORD							m_pausedTicks;

	//초당 프레임 수 측정을 위한 부분
	UINT							m_iCnt;
	UINT							m_iFPS;
	DWORD							m_elapseTicks;

	//1프레임 시간
	DWORD							m_oneFrameTicks;
	FLOAT							m_fOneFrameT;

	//타이머 작동 시간
	DWORD							m_runningTicks;

public:
	SimpleTimer();
	virtual ~SimpleTimer();

	void							start();
	void							updateTimer();
	void							pause();
	void							resume();
	void							end();
	void							reset();

	DWORD							getFPS();

	DWORD							getRunningTicks();
	DWORD							getOneFrameTicks();
	FLOAT							getRunningTimeF();
	FLOAT							getOneFrameTimeF();
};


//메인 타이머 클래스
class CPGCMainTimer : public SimpleHiResTimer
{
private:
	CPGCMainTimer();
	virtual ~CPGCMainTimer();

public:
	CPGCMainTimer(const CPGCMainTimer& other) = delete;
	CPGCMainTimer& operator=(const CPGCMainTimer& other) = delete;

	//전역 인스턴스 반환
	static CPGCMainTimer& getInstance()
	{
		//늦은 초기화
		static CPGCMainTimer singleInst;
		return singleInst;
	}
};

#define CPGC_MAINTIMER	CPGCMainTimer::getInstance()


//////////////////////////////////////////////////
//
// CPGCTimer.h
//		������ �ð��� ����ϴ� ���.
//		1.	�ð� �ʱ�ȭ ���� �帥 ��ü�ð� : m_curTick - m_initTick
//		2.	1������ �ð� : m_curTick - m_prevTick
//		3.	������ ���� �ð� ���� : m_prevTick + m_pausedTicks
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"

#define DEFAULT_TIMER_TICK_LIMIT			1000

LARGE_INTEGER getSystemFrequency();

class SimpleHiResTimer
{
protected:
	static const LARGE_INTEGER		g_systemTimeFreq;

	//Ÿ�̸� Ȱ��ȭ ����
	bool							m_bActivated;

	//�ʱ�ƽ, ���� ���� ƽ, ���� Ư�� ƽ
	LARGE_INTEGER					m_initTick;
	LARGE_INTEGER					m_prevTick;
	LARGE_INTEGER					m_curTick;

	//�Ͻ����� ����� ���� �κ�
	bool							m_bStopped;
	LARGE_INTEGER					m_pausedTicks;

	//�ʴ� ������ �� ������ ���� �κ�
	UINT							m_iCnt;
	UINT							m_iFPS;
	LARGE_INTEGER					m_elapseTicks;

	//1������ �ð�
	LARGE_INTEGER					m_oneFrameTicks;
	FLOAT							m_fOneFrameT;

	//Ÿ�̸� �۵� �ð�
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

//winmm.lib �ʿ�
class SimpleTimer
{
protected:
	//Ÿ�̸� Ȱ��ȭ ����
	bool							m_bActivated;

	//�ʱ�ƽ, ���� ���� ƽ, ���� Ư�� ƽ
	DWORD							m_initTick;
	DWORD							m_prevTick;
	DWORD							m_curTick;

	//�Ͻ����� ����� ���� �κ�
	bool							m_bStopped;
	DWORD							m_pausedTicks;

	//�ʴ� ������ �� ������ ���� �κ�
	UINT							m_iCnt;
	UINT							m_iFPS;
	DWORD							m_elapseTicks;

	//1������ �ð�
	DWORD							m_oneFrameTicks;
	FLOAT							m_fOneFrameT;

	//Ÿ�̸� �۵� �ð�
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


//���� Ÿ�̸� Ŭ����
class CPGCMainTimer : public SimpleHiResTimer
{
private:
	CPGCMainTimer();
	virtual ~CPGCMainTimer();

public:
	CPGCMainTimer(const CPGCMainTimer& other) = delete;
	CPGCMainTimer& operator=(const CPGCMainTimer& other) = delete;

	//���� �ν��Ͻ� ��ȯ
	static CPGCMainTimer& getInstance()
	{
		//���� �ʱ�ȭ
		static CPGCMainTimer singleInst;
		return singleInst;
	}
};

#define CPGC_MAINTIMER	CPGCMainTimer::getInstance()


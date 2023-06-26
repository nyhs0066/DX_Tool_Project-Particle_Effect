#pragma once
#include "CPGCStd.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <iostream>
#include <array>
#include <Windows.h>

#define MAX_NUMBER_OF_THREAD 3

class CPGCSyncEvent
{
private:
	std::string					m_szName;

public:
	std::condition_variable		m_hEventCV;
	std::mutex					m_hMutex;

	bool						m_bSignaled;

public:
	CPGCSyncEvent();
	CPGCSyncEvent(std::string szName);
	virtual ~CPGCSyncEvent();

	void	SetEvent();
	void	ResetEvent();

	//-1 = TIMEOUT / 0 = NO TIMEOUT / 1 = / 2 = 
	int		WaitForSingleObject(UINT timeout_ms = INFINITE);
};

class CPGCAsyncEvent
{
public:
};

class CPGCEventRunnable
{
public:
	std::thread m_hThread;
	std::atomic<bool> m_bStop;

public:
	CPGCEventRunnable(const CPGCEventRunnable&) = delete;
	CPGCEventRunnable& operator=(const CPGCEventRunnable&) = delete;

	CPGCEventRunnable();
	virtual ~CPGCEventRunnable();

	void start();
	void stop();
	void suspend();
	void resume();

	bool isStop();

	virtual void Run() = 0;
};

class CPGCEventRunableInstance : public CPGCEventRunnable
{
public:
	virtual void Run() override;
};

class CPGCEventHandler
{
public:
	std::vector<CPGCSyncEvent*> m_EventArray;
public:
	int CreateSyncEvent(std::string name);

	void	SetEvent(int ID);
	void	ResetEvent(int ID);
	int		WaitForSingleObject(int ID, UINT timeout_ms = INFINITE);
	int		WaitForMultiObject(int* pIDList, int iSize, bool bWaitAll = false, UINT timeout_ms = INFINITE);
	bool	closeEventHandle(int ID);
	bool	closeAllEventHandle(int ID);
};

//전역으로 사용해야하는 변수
std::vector<int>			g_eventList;
std::vector<std::thread>	g_threadList;
std::vector<int>			g_threadInstanceData;

CPGCEventHandler			g_handler;

void PerSceneRenderDeferredProc(int& parameter)
{
	int iInstance = parameter;

	while (true)
	{
		//update() 이벤트 통지 대기
		int iEventID = g_handler.WaitForSingleObject(iInstance);

		// ...

		g_handler.ResetEvent(iInstance);

		//render() 
		g_handler.SetEvent(MAX_NUMBER_OF_THREAD + iInstance);
	}
}


void test_Event()
{
	std::ios::sync_with_stdio(true);

	//TEST CODE1
	
	//CPGCEventRunableInstance ERI1;
	//ERI1.start();

	//while (true)
	//{
	//	ERI1.suspend();

	//	//대기 함수
	//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//	ERI1.resume();

	//	//대기 함수
	//	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	//}

	//std::cout << "END" << std::endl;

	//TEST CODE2

	for (int iThread = 0; iThread < MAX_NUMBER_OF_THREAD; iThread++)
	{
		g_threadInstanceData.push_back(iThread);
		g_eventList.push_back(g_handler.CreateSyncEvent("Event" + std::to_string(iThread)));
		g_threadList.emplace_back(std::thread(PerSceneRenderDeferredProc, std::ref(g_threadInstanceData[iThread])));

		//Background Thread(UI Thread) : 관리범위 밖으로 스레드 분리
		g_threadList[g_threadList.size() - 1].detach();
	}

	g_eventList.push_back(g_handler.CreateSyncEvent("render_0"));
	g_eventList.push_back(g_handler.CreateSyncEvent("render_1"));
	g_eventList.push_back(g_handler.CreateSyncEvent("render_2"));

	while (true)
	{
		//update
		for (int iThread = 0; iThread < MAX_NUMBER_OF_THREAD; iThread++)
		{
			g_handler.SetEvent(iThread);
		}

		//render
		int iEventArray[] = { MAX_NUMBER_OF_THREAD + 0,
		MAX_NUMBER_OF_THREAD + 1,
		MAX_NUMBER_OF_THREAD + 2 };

		g_handler.WaitForMultiObject(iEventArray, 3, true);

		for (int iThread = 0; iThread < MAX_NUMBER_OF_THREAD; iThread++)
		{
			//commendlist execute->render()

			g_handler.ResetEvent(iThread);
		}
	}
}



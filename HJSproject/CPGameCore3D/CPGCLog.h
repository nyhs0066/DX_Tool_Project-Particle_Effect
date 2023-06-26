#pragma once
#include "CPGCStd.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <iostream>

class CPGCLog
{
protected:
	std::thread m_hThread;
	std::mutex m_hMutex;
	std::mutex m_hMutexStarted;
	std::condition_variable m_hEvent;
	std::condition_variable m_hThreadStartEvent;
	std::queue<std::string> m_szQueue;

	bool m_bExit = false;
	bool m_bThreadStarted = false;

public:
	std::string m_logFilePath;

public:
	CPGCLog();
	virtual ~CPGCLog();

	void Run();
	void log(std::string data);

	static CPGCLog& Get() { static CPGCLog log; return log; }
};

int test()
{
	int iCnt = 0;

	while (iCnt < 10)
	{
		CPGCLog::Get().log("0123456");
		Sleep(1000);
		iCnt++;
	}

	std::cout << "End" << std::endl;

	return 0;
}


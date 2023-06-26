#include "CPGCEvent.h"

CPGCEventRunnable::CPGCEventRunnable() : m_bStop(false), m_hThread()
{

}

CPGCEventRunnable::~CPGCEventRunnable()
{

}

void CPGCEventRunnable::start()
{
	m_hThread = std::thread(&CPGCEventRunnable::Run, this);
}

void CPGCEventRunnable::stop()
{
	m_bStop = true;

	if (m_hThread.joinable())
	{
		m_hThread.join();
	}
}

void CPGCEventRunnable::suspend()
{
	//HANDLE == native_handle()
	SuspendThread(m_hThread.native_handle());
}

void CPGCEventRunnable::resume()
{
	ResumeThread(m_hThread.native_handle());
}

bool CPGCEventRunnable::isStop()
{
	return m_bStop;
}

void CPGCEventRunableInstance::Run()
{
	int iCnt = 0;

	while (!m_bStop)
	{
		std::cout << "Running! COUNT : " << iCnt << std::endl;
	}
}

int CPGCEventHandler::CreateSyncEvent(std::string name)
{
	m_EventArray.push_back(new CPGCSyncEvent(name));

	return m_EventArray.size();
}

void CPGCEventHandler::SetEvent(int ID)
{
	m_EventArray[ID]->SetEvent();
}

void CPGCEventHandler::ResetEvent(int ID)
{
	m_EventArray[ID]->ResetEvent();
}

int CPGCEventHandler::WaitForSingleObject(int ID, UINT timeout_ms)
{
	return m_EventArray[ID]->WaitForSingleObject(timeout_ms);
}

int CPGCEventHandler::WaitForMultiObject(int* pIDList, int iSize, bool bWaitAll, UINT timeout_ms)
{
	for (int i = 0; i < iSize; i++)
	{

	}

	return 0;
}

bool CPGCEventHandler::closeEventHandle(int ID)
{
	return true;
}

bool CPGCEventHandler::closeAllEventHandle(int ID)
{
	return true;
}

CPGCSyncEvent::CPGCSyncEvent()
{
	m_bSignaled = false;
}

CPGCSyncEvent::CPGCSyncEvent(std::string szName) : m_szName(szName)
{
	m_bSignaled = false;
}

CPGCSyncEvent::~CPGCSyncEvent()
{
}

void CPGCSyncEvent::SetEvent()
{
	m_bSignaled = true;
	m_hEventCV.notify_all();
}

void CPGCSyncEvent::ResetEvent()
{
	m_bSignaled = false;
}

int CPGCSyncEvent::WaitForSingleObject(UINT timeout_ms)
{
	if (m_bSignaled) return 0;

	int ret = -1;
	std::unique_lock<std::mutex> lock(m_hMutex);

	if (timeout_ms == INFINITE)
	{
		//무기한 대기
		m_hEventCV.wait(lock);
		return 0;
	}
	else
	{
		//지정 시간 대기
		std::cv_status wait = m_hEventCV.wait_for(lock, std::chrono::milliseconds(timeout_ms));

		if (wait == std::cv_status::timeout)
		{
			ret = -1;
		}
		else if (wait == std::cv_status::no_timeout)
		{
			ret = 0;
		}

		lock.unlock();
	}

	return ret;
}



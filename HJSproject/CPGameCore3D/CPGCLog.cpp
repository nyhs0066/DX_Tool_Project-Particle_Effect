#include "CPGCLog.h"

CPGCLog::CPGCLog()
{
	//로그 폴더가 존재하지 않으면 생성
	system("md ..\\log");

	//현재 시간을 받아서
	time_t now = time(NULL);

	//시간 구조체로 만들고
	struct tm LocalTime;
	localtime_s(&LocalTime, &now);

	//파일 저장 경로 버퍼를 생성해
	m_logFilePath.resize(100);

	//버퍼에 해당 서식에 맞는 문자열을 채우고
	strftime(&m_logFilePath.at(0), 100, "../log/%Y년%m월%d일%H시%M분&S초.txt", &LocalTime);

	//스레드 시작 플래그를 초기화 하고
	m_bThreadStarted = false;

	//스레드를 생성해서 로그 남기기 작업 시작하도록 로그 기록 함수 넘기고 스레드 시작
	m_hThread = std::thread( &CPGCLog::Run, this );

	//로그 기록 시작을 대기하기위한 뮤텍스 lock객체를 만들어
	std::unique_lock<std::mutex> lock(m_hMutexStarted);

	//시작 이벤트를 기다린다.
	m_hThreadStartEvent.wait(lock, [&]() {return m_bThreadStarted == true; });
}

CPGCLog::~CPGCLog()
{
	//싱글톤 객체가 종료되었다는 것은 프로그램이 종료 되었다는 것을 의미하므로
	//스레드 탈출 플래그를 세트한다.
	m_bExit = true;

	//스레트 탈출 플래그가 세트되었으므로 메인루프를 진행시켜 루프를 빠져나갈수 있도록 한다.
	m_hEvent.notify_all();

	//스레드가 정상적으로 끝날때 까지 대기한다.
	m_hThread.join();
}

void CPGCLog::Run()
{
	//생성 스레드에서 함수가 시작되면 설정된 경로를 따라 파일 스트림을 열고
	std::ofstream ofs(m_logFilePath);

	//열렸는지 확인 후에
	if (!ofs) { return; }

	//로그메시지를 푸시하는 함수가 호출될때 까지 대기 하기위한 뮤텍스를 생성하고
	std::unique_lock<std::mutex> lock(m_hMutex);

	//시작 플래그를 세트하고
	m_bThreadStarted = true;

	//로그 기록 시작을 알리는 통지를 기다리는 스레드에 전파한다. 
	m_hThreadStartEvent.notify_all();

	while (true)
	{
		//Log함수의 동작을 기다리고
		m_hEvent.wait(lock);	//이벤트가 발생할때까지 대기

		//이벤트를 수신했으면 락을 풀어 스레드 접근 가능하게 하고
		lock.unlock();

		while (true)
		{
			//파일에 기록할 때 동안 다른 스레드의 접근을 막고
			lock.lock();

			if (m_szQueue.empty())
			{
				//처리 메시지 큐가 비어있다면 내부 루프 빠져 나가기
				break;
			}
			else
			{
				//큐에 로그 메시지가 있다면 파일 스트림에 기록하고 큐에서 제거
				ofs << m_szQueue.front() << std::endl;
				m_szQueue.pop();
			}

			//작업이 끝났으니 다른 스레드에서 접근 가능하도록 락을 풀고
			lock.unlock();
		}

		//종료 플래그가 세트되는 경우 외부 루프를 빠져나간다.
		if (m_bExit)
		{
			break;
		}
	}
}

void CPGCLog::log(std::string data)
{
	//로그 메시지를 받는 이 함수가 호출되면 뮤텍스 락 객체를 만들고
	std::unique_lock<std::mutex> lock(m_hMutex);

	//큐에 데이터를 푸시한다.
	m_szQueue.push(data);

	//기다리고 있는 스레드에 로그 메시지를 받았다는 알림 통지
	m_hEvent.notify_all();
}

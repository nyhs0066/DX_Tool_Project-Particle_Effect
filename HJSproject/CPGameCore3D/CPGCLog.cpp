#include "CPGCLog.h"

CPGCLog::CPGCLog()
{
	//�α� ������ �������� ������ ����
	system("md ..\\log");

	//���� �ð��� �޾Ƽ�
	time_t now = time(NULL);

	//�ð� ����ü�� �����
	struct tm LocalTime;
	localtime_s(&LocalTime, &now);

	//���� ���� ��� ���۸� ������
	m_logFilePath.resize(100);

	//���ۿ� �ش� ���Ŀ� �´� ���ڿ��� ä���
	strftime(&m_logFilePath.at(0), 100, "../log/%Y��%m��%d��%H��%M��&S��.txt", &LocalTime);

	//������ ���� �÷��׸� �ʱ�ȭ �ϰ�
	m_bThreadStarted = false;

	//�����带 �����ؼ� �α� ����� �۾� �����ϵ��� �α� ��� �Լ� �ѱ�� ������ ����
	m_hThread = std::thread( &CPGCLog::Run, this );

	//�α� ��� ������ ����ϱ����� ���ؽ� lock��ü�� �����
	std::unique_lock<std::mutex> lock(m_hMutexStarted);

	//���� �̺�Ʈ�� ��ٸ���.
	m_hThreadStartEvent.wait(lock, [&]() {return m_bThreadStarted == true; });
}

CPGCLog::~CPGCLog()
{
	//�̱��� ��ü�� ����Ǿ��ٴ� ���� ���α׷��� ���� �Ǿ��ٴ� ���� �ǹ��ϹǷ�
	//������ Ż�� �÷��׸� ��Ʈ�Ѵ�.
	m_bExit = true;

	//����Ʈ Ż�� �÷��װ� ��Ʈ�Ǿ����Ƿ� ���η����� ������� ������ ���������� �ֵ��� �Ѵ�.
	m_hEvent.notify_all();

	//�����尡 ���������� ������ ���� ����Ѵ�.
	m_hThread.join();
}

void CPGCLog::Run()
{
	//���� �����忡�� �Լ��� ���۵Ǹ� ������ ��θ� ���� ���� ��Ʈ���� ����
	std::ofstream ofs(m_logFilePath);

	//���ȴ��� Ȯ�� �Ŀ�
	if (!ofs) { return; }

	//�α׸޽����� Ǫ���ϴ� �Լ��� ȣ��ɶ� ���� ��� �ϱ����� ���ؽ��� �����ϰ�
	std::unique_lock<std::mutex> lock(m_hMutex);

	//���� �÷��׸� ��Ʈ�ϰ�
	m_bThreadStarted = true;

	//�α� ��� ������ �˸��� ������ ��ٸ��� �����忡 �����Ѵ�. 
	m_hThreadStartEvent.notify_all();

	while (true)
	{
		//Log�Լ��� ������ ��ٸ���
		m_hEvent.wait(lock);	//�̺�Ʈ�� �߻��Ҷ����� ���

		//�̺�Ʈ�� ���������� ���� Ǯ�� ������ ���� �����ϰ� �ϰ�
		lock.unlock();

		while (true)
		{
			//���Ͽ� ����� �� ���� �ٸ� �������� ������ ����
			lock.lock();

			if (m_szQueue.empty())
			{
				//ó�� �޽��� ť�� ����ִٸ� ���� ���� ���� ������
				break;
			}
			else
			{
				//ť�� �α� �޽����� �ִٸ� ���� ��Ʈ���� ����ϰ� ť���� ����
				ofs << m_szQueue.front() << std::endl;
				m_szQueue.pop();
			}

			//�۾��� �������� �ٸ� �����忡�� ���� �����ϵ��� ���� Ǯ��
			lock.unlock();
		}

		//���� �÷��װ� ��Ʈ�Ǵ� ��� �ܺ� ������ ����������.
		if (m_bExit)
		{
			break;
		}
	}
}

void CPGCLog::log(std::string data)
{
	//�α� �޽����� �޴� �� �Լ��� ȣ��Ǹ� ���ؽ� �� ��ü�� �����
	std::unique_lock<std::mutex> lock(m_hMutex);

	//ť�� �����͸� Ǫ���Ѵ�.
	m_szQueue.push(data);

	//��ٸ��� �ִ� �����忡 �α� �޽����� �޾Ҵٴ� �˸� ����
	m_hEvent.notify_all();
}

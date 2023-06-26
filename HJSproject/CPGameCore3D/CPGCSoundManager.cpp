#include "CPGCSoundManager.h"

CPRS_Sound::CPRS_Sound()
{
	m_pSystem = nullptr;
	m_pSound = nullptr;
	m_pChannel = nullptr;
}

CPRS_Sound::~CPRS_Sound()
{
	release();
}

FMOD_RESULT CPRS_Sound::play(UINT iVolume, bool loopFlag)
{
	FMOD_RESULT fr = FMOD_OK;

	//�÷��̰� ���� ä���� �ڵ����� ��ȿȭ �ȴ�.
	//�÷������� ����� isPlaying()�Լ��� Ȯ���Ѵ�.
	//���� �÷��״� ä���� ������ ���Ŀ� ����ȴ�.

	if (m_pChannel)
	{
		bool playFlag;
		m_pChannel->isPlaying(&playFlag);

		setLoop(loopFlag);
		setVolume(iVolume);

		if (playFlag) { return fr; }
	}

	fr = m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	if (fr != FMOD_OK) { return fr; }

	setLoop(loopFlag);
	setVolume(iVolume);

	return fr;
}

FMOD_RESULT CPRS_Sound::playEffect(UINT iVolume, bool loopFlag)
{
	FMOD_RESULT fr = FMOD_OK;

	setLoop(loopFlag);

	fr = m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	if (fr != FMOD_OK) { return fr; }

	return fr;
}

void CPRS_Sound::setLoop(bool loopFlag)
{
	if (m_pChannel)
	{
		if (loopFlag) { m_pChannel->setMode(FMOD_LOOP_NORMAL); }
		else { m_pChannel->setMode(FMOD_LOOP_OFF); }
	}
}

void CPRS_Sound::stop()
{
	if (m_pChannel) { m_pChannel->stop(); }
}

int CPRS_Sound::togglePause()
{
	if (m_pChannel)
	{
		bool pauseFlag;
		m_pChannel->getPaused(&pauseFlag);

		m_pChannel->setPaused(!pauseFlag);

		return !pauseFlag;
	}

	return -1;
}

void CPRS_Sound::volumeUp(UINT delta)
{
	m_iVolume = min(100, m_iVolume + delta);

	if (m_pChannel) { m_pChannel->setVolume(m_iVolume / 100.0f); }
}

void CPRS_Sound::volumeDown(UINT delta)
{
	m_iVolume = max(0, m_iVolume - delta);

	if (m_pChannel) { m_pChannel->setVolume(m_iVolume / 100.0f); }
}

void CPRS_Sound::setVolume(UINT iVolume)
{
	m_iVolume = max(0, iVolume);
	m_iVolume = min(100, iVolume);

	if (m_pChannel) { m_pChannel->setVolume(m_iVolume / 100.0f); }
}

UINT CPRS_Sound::getSoundLengthMs()
{
	UINT len = -1;
	if (m_pSound) { m_pSound->getLength(&len, FMOD_TIMEUNIT_MS); }

	return len;
}

UINT CPRS_Sound::getCurSoundPosMs()
{
	UINT cPos = -1;
	if (m_pChannel) { m_pChannel->getPosition(&cPos, FMOD_TIMEUNIT_MS); }

	return cPos;
}

bool CPRS_Sound::init()
{
	return true;
}

bool CPRS_Sound::release()
{
	if (m_pSound) { m_pSound->release(); m_pSound = nullptr; }

	return true;
}

CPGCSoundManager::CPGCSoundManager()
{
	m_pSystem = nullptr;
}

CPGCSoundManager::~CPGCSoundManager()
{

}

bool CPGCSoundManager::createSound(std::wstring wszSoundName, std::wstring wszFilename)
{
	//���� �̸��� ������ ����
	if (m_soundMap.find(wszSoundName) == m_soundMap.end() &&
		m_filenameMap.find(wszFilename) == m_filenameMap.end())
	{
		CPRS_Sound* newE = new CPRS_Sound;

		FMOD_RESULT fr = FMOD_OK;
		std::string filename = atl_W2M(wszFilename);

		fr = m_pSystem->createSound(filename.c_str(), FMOD_DEFAULT,
			nullptr, &newE->m_pSound);

		if(fr == FMOD_OK)
		{
			newE->m_pSystem = m_pSystem;
			m_filenameMap.insert(std::make_pair(wszFilename, wszSoundName));
			m_soundMap.insert(std::make_pair(wszSoundName, newE));
		}
		else 
		{ 
			delete newE;
			newE = nullptr;
			return false; 
		}
	}

	return true;
}

//bool CPGCSoundManager::loadScript(std::wstring wszInfoFileName)
//{
//	//��ũ��Ʈ ���ϸ� �ߺ� Ȯ��
//	if (m_fileNameSet.end() != m_fileNameSet.find(wszInfoFileName)) { return false; }
//
//	//�ʿ� ���� ����
//	std::wifstream wiFp;
//	wchar_t	scriptInfo[256] = { 0 };
//	std::wstring wszbuf;
//	std::wstring prefix, postfix;
//	std::wstring IDname;
//
//	//ī��Ʈ�� ID�ε���
//	UINT cnt = 0;		//�ҷ����⸦ ������ ���ҽ� ���� ��ȯ��
//	UINT idx = m_curIDX;
//
//	//���� ����
//	wiFp.open(wszInfoFileName, std::ios::in);
//	if (!wiFp) { return cnt; }
//
//	//��ũ��Ʈ ���� ó��
//	wiFp.getline(scriptInfo, 256);
//	wiFp >> wszbuf >> prefix;		//����
//	wiFp >> wszbuf >> postfix;		//����
//
//	if (prefix == L"#NULL") { prefix.clear(); }
//	else { prefix += L"_"; }
//	if (postfix == L"#NULL") { postfix.clear(); }
//	else { postfix = L"_" + postfix; }
//
//	//���ҽ� �б� ����
//	while (true)
//	{
//		//�̸����� ������ Ȯ��
//		wiFp >> wszbuf;
//		if (wszbuf == L"#END") { break; }
//
//		//�̸��� Ű���� ����
//		IDname = wszbuf;
//		std::wstring key = prefix + IDname + postfix;
//
//		//����� �б�
//		wiFp >> wszbuf;
//
//		//�ߺ� Ȯ���� �ӽ� ����Ʈ�� ���� ���� 
//		if (m_soundMap.end() == m_soundMap.find(key))
//		{
//			CPRS_Sound* newE = new CPRS_Sound;
//
//			if (newE->create(m_pSystem, MYNSP::api_W2M(wszbuf.c_str())) == FMOD_ERR_INVALID_PARAM)
//			{
//				delete newE;
//				newE = nullptr;
//			}
//			else
//			{
//				wcscpy_s(newE->m_RInfo.resourceType, L"SOUND");
//				newE->m_RInfo.idx = idx++;
//				wcscpy_s(newE->m_RInfo.IDname, IDname.c_str());
//				wcscpy_s(newE->m_RInfo.RPath, wszbuf.c_str());
//
//				m_soundMap.insert(std::make_pair(key, newE));
//				cnt++;
//			}
//		}
//	}
//
//	//������ ������ ���� �Ŵ����� ���
//	m_fileNameSet.insert(wszInfoFileName);
//	m_curIDX = idx;
//
//	return cnt;
//}
//
//bool CPGCSoundManager::loadDir(std::wstring wszPath)
//{
//	//wszPath�� ���������� /�� �ٴ´ٰ� �����Ѵ�. ������ ���̴� �͵� ���� ����
//	std::wstring dirPath = wszPath + L"*.*";
//
//	intptr_t handle;
//	_wfinddata_t fd;	//���� �Ǵ� ���丮�� �Ӽ��� ��Ÿ���� ����ü
//
//	handle = _wfindfirst(dirPath.c_str(), &fd); //���н� -1�� ��ȯ�Ѵ�.
//
//	if (handle != -1)
//	{
//		do
//		{
//			if ((fd.attrib & _A_SUBDIR) && (fd.name[0] != L'.'))
//			{
//				loadDir(wszPath + fd.name + L"/");
//			}
//			else if (fd.name[0] != L'.')
//			{
//				m_fileNameSet.insert(wszPath + fd.name);
//			}
//		} while (!_wfindnext(handle, &fd));
//	}
//
//	_findclose(handle);
//
//	return true;
//}
//
//std::wstring CPGCSoundManager::getSplitName(std::wstring wszFullPath)
//{
//	std::wstring ret;
//
//	wchar_t drive[MAX_PATH] = { 0 };
//	wchar_t dir[MAX_PATH] = { 0 };
//	wchar_t fileName[MAX_PATH] = { 0 };
//	wchar_t extension[MAX_PATH] = { 0 };
//
//	//���� ��� ����
//	_wsplitpath_s(wszFullPath.c_str(), drive, dir, fileName, extension);
//
//	ret = fileName;
//	ret += extension;
//
//	return ret;
//}

CPRS_Sound* CPGCSoundManager::getPtr(std::wstring wszSoundName)
{
	auto it = m_soundMap.find(wszSoundName);
	if (it != m_soundMap.end()) { return it->second; }

	return nullptr;
}

bool CPGCSoundManager::init()
{
	FMOD_RESULT fr = FMOD_OK;

	//FMOD ��ü���� �����ϱ� ���� System �������̽��� �����Ѵ�.
	fr = FMOD::System_Create(&m_pSystem);

	fr = m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);
	if (fr != FMOD_OK) { return false; }
	return true;
}

bool CPGCSoundManager::update()
{
	//�ý��� �������̽��� �����ϰ� �ִ� ��ü���� �� �����Ӹ��� ������ �־�� �� 
	m_pSystem->update();
	return true;
}

bool CPGCSoundManager::release()
{
	m_filenameMap.clear();

	for (auto it : m_soundMap) 
	{
		delete it.second;
		it.second = nullptr;
	}

	m_soundMap.clear();

	//�ý����� �����Ǳ� ������ �ݵ�� �ٸ� ���ҽ��� ���� �����Ǿ�� �Ѵ�.
	if (m_pSystem) { m_pSystem->close(); m_pSystem->release(); m_pSystem = nullptr; }

	return true;
}
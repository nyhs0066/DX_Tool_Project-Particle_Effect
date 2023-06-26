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

	//플레이가 끝난 채널은 자동으로 무효화 된다.
	//플레이중인 사운드는 isPlaying()함수로 확인한다.
	//루프 플래그는 채널이 생성된 이후에 적용된다.

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
	//같은 이름이 없으면 생성
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
//	//스크립트 파일명 중복 확인
//	if (m_fileNameSet.end() != m_fileNameSet.find(wszInfoFileName)) { return false; }
//
//	//필요 변수 선언
//	std::wifstream wiFp;
//	wchar_t	scriptInfo[256] = { 0 };
//	std::wstring wszbuf;
//	std::wstring prefix, postfix;
//	std::wstring IDname;
//
//	//카운트와 ID인덱스
//	UINT cnt = 0;		//불러오기를 성공한 리소스 개수 반환용
//	UINT idx = m_curIDX;
//
//	//파일 열기
//	wiFp.open(wszInfoFileName, std::ios::in);
//	if (!wiFp) { return cnt; }
//
//	//스크립트 선단 처리
//	wiFp.getline(scriptInfo, 256);
//	wiFp >> wszbuf >> prefix;		//접미
//	wiFp >> wszbuf >> postfix;		//접두
//
//	if (prefix == L"#NULL") { prefix.clear(); }
//	else { prefix += L"_"; }
//	if (postfix == L"#NULL") { postfix.clear(); }
//	else { postfix = L"_" + postfix; }
//
//	//리소스 읽기 루프
//	while (true)
//	{
//		//이름인지 끝인지 확인
//		wiFp >> wszbuf;
//		if (wszbuf == L"#END") { break; }
//
//		//이름과 키값을 저장
//		IDname = wszbuf;
//		std::wstring key = prefix + IDname + postfix;
//
//		//상대경로 읽기
//		wiFp >> wszbuf;
//
//		//중복 확인후 임시 리스트에 내용 저장 
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
//	//루프가 끝나면 파일 매니저에 등록
//	m_fileNameSet.insert(wszInfoFileName);
//	m_curIDX = idx;
//
//	return cnt;
//}
//
//bool CPGCSoundManager::loadDir(std::wstring wszPath)
//{
//	//wszPath의 마지막에는 /이 붙는다고 가정한다. 없으면 붙이는 것도 좋을 지도
//	std::wstring dirPath = wszPath + L"*.*";
//
//	intptr_t handle;
//	_wfinddata_t fd;	//파일 또는 디렉토리의 속성을 나타내는 구조체
//
//	handle = _wfindfirst(dirPath.c_str(), &fd); //실패시 -1을 반환한다.
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
//	//파일 경로 분해
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

	//FMOD 객체들을 생성하기 위한 System 인터페이스를 생성한다.
	fr = FMOD::System_Create(&m_pSystem);

	fr = m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);
	if (fr != FMOD_OK) { return false; }
	return true;
}

bool CPGCSoundManager::update()
{
	//시스템 인터페이스를 소유하고 있는 객체에서 매 프레임마다 갱신해 주어야 함 
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

	//시스템이 해제되기 이전에 반드시 다른 리소스가 먼저 해제되어야 한다.
	if (m_pSystem) { m_pSystem->close(); m_pSystem->release(); m_pSystem = nullptr; }

	return true;
}
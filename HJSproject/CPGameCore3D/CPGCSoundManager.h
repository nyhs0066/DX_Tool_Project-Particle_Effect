#pragma once
#include "CPGCStd.h"

#include "fmod.h"			//C�� ���
#include "fmod.hpp"			//C++�� ���
#include "fmod_errors.h"	//����׿� ó��Ȯ���� ���� ���

class CPRS_Sound
{
	friend class CPGCSoundManager;
private:
	std::wstring			m_wszName;

	FMOD::System*			m_pSystem;
	FMOD::Sound*			m_pSound;
	FMOD::Channel*			m_pChannel;

	UINT					m_iVolume;

public:
	CPRS_Sound();
	~CPRS_Sound();

	FMOD_RESULT				play(UINT iVolume, bool loopFlag = false);
	FMOD_RESULT				playEffect(UINT iVolume, bool loopFlag = false);
	void					setLoop(bool loopFlag);
	void					stop();

	int						togglePause();

	//0 : silent / 1 : full / 1 < : amplify
	void					volumeUp(UINT delta = 2);
	void					volumeDown(UINT delta = 2);
	void					setVolume(UINT iVolume);

	UINT					getSoundLengthMs();
	UINT					getCurSoundPosMs();

	bool					init();
	bool					release();
};

//���� �Ŵ����� Ű���� ��ũ��Ʈ ������ ������ �̸����� ���Ѵ�.
class CPGCSoundManager
{
private:
	FMOD::System*								m_pSystem;

	std::set<std::wstring>						m_scriptFilenameSet;

	std::map<std::wstring, std::wstring>		m_filenameMap;
	std::map<std::wstring, CPRS_Sound*>			m_soundMap;
	//UINT										m_curIDX;

	CPGCSoundManager();
	~CPGCSoundManager();

public:
	CPGCSoundManager(const CPGCSoundManager& other) = delete;
	CPGCSoundManager& operator=(const CPGCSoundManager& other) = delete;

public:
	bool					createSound(std::wstring wszSoundName, std::wstring wszFilename);

	//bool					loadScript(std::wstring wszInfoFileName);
	//bool					loadDir(std::wstring wszPath);
	//std::wstring			getSplitName(std::wstring wszFullPath);

	CPRS_Sound*				getPtr(std::wstring wszSoundName);

	bool					init();
	bool					update();
	bool					release();

	static CPGCSoundManager& getInstance()
	{
		static CPGCSoundManager singleInst;
		return singleInst;
	}
};

#define CPGC_SOUND_MGR CPGCSoundManager::getInstance()
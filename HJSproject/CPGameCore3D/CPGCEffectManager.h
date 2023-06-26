#pragma once
#include "CPGCStd.h"
#include "CPGCFileIO.h"
#include "CPGCEffect.h"

#define DEFAULT_PSYSTEM_DIRECTORY_WSZPATH	L"../../data/save/particlesystem/new/"
#define DEFAULT_PSYSTEM_DIRECTORY_SZPATH	"../../data/save/particlesystem/new/"

#define DEFAULT_CLEANUP_TIME_LIMIT			60.0f

class CPGCEffectManager
{
private:
	std::map<std::wstring, CPRS_PSYSTEM_FILESTRUCT> m_PSystemMap;

	std::vector<CPGCEffect*>						m_pendingDeleteEffectQueue;

	float											m_timer;

	CPGCEffectManager();
	~CPGCEffectManager();
public:
	CPGCEffectManager(const CPGCEffectManager& other) = delete;
	CPGCEffectManager& operator=(const CPGCEffectManager& other) = delete;

public:

	bool	init();
	bool	update();
	bool	release();

	bool	loadPSystem(std::wstring wszPSystemName, std::wstring wszPSystemFilePath);
	bool	loadPSystemDir(std::wstring wszDirPath);

	void	addToDeleteList(CPGCEffect* pEffect);
	int		cleanUp();

	CPRS_PSYSTEM_FILESTRUCT* getPSystem(std::wstring wszPSystemName);

	static CPGCEffectManager& getInstance()
	{
		static CPGCEffectManager singleInst;
		return singleInst;
	}
};

#define CPGC_EFFECT_MGR	CPGCEffectManager::getInstance()

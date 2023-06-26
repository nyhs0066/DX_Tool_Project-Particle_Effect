#include "CPGCEffectManager.h"

CPGCEffectManager::CPGCEffectManager()
{
	m_timer = 0.0f;
}

CPGCEffectManager::~CPGCEffectManager()
{
}

bool CPGCEffectManager::init()
{
	bool bRet = loadPSystemDir(DEFAULT_PSYSTEM_DIRECTORY_WSZPATH);

	return bRet;
}

bool CPGCEffectManager::update()
{
	m_timer += CPGC_MAINTIMER.getOneFrameTimeF();

	if (m_timer > DEFAULT_CLEANUP_TIME_LIMIT && m_pendingDeleteEffectQueue.size())
	{
		m_timer = 0.0f;
		int cnt = cleanUp();

		OutputDebugString((L"NUMBER OF DELETED EFFECT : " + std::to_wstring(cnt)).c_str());
	}

	return true;
}

bool CPGCEffectManager::release()
{
	m_PSystemMap.clear();

	cleanUp();

	return true;
}

bool CPGCEffectManager::loadPSystem(std::wstring wszPSystemName, std::wstring wszPSystemFilePath)
{
	if (m_PSystemMap.find(wszPSystemName) == m_PSystemMap.end())
	{
		CPRS_PSYSTEM_FILESTRUCT loadData;

		bool bRet = CPGC_FILEIO_MGR.loadFile(wszPSystemFilePath, &loadData);

		std::wstring nameToken;

		size_t offset = wszPSystemName.find(L'.', 0);

		nameToken = wszPSystemName.substr(0, offset);

		if (bRet)
		{
			for (int i = 0; i < loadData.iEmiiterCnt; i++)
			{
				if (loadData.emitters[i].wszTextureName[0] != NULL)
				{
					CPGC_TEXTURE_MGR.createTexture(loadData.emitters[i].wszTextureName, loadData.emitters[i].wszTexturePath);
				}
			}

			m_PSystemMap.insert(std::make_pair(nameToken, loadData));
		}
		else { return false; }
	}

	return true;
}

bool CPGCEffectManager::loadPSystemDir(std::wstring wszDirPath)
{
	std::vector<std::wstring> exts;
	exts.push_back(L".PSystem");
	std::vector<std::wstring> names;
	std::vector<std::wstring> paths;

	bool bRet = loadDir(wszDirPath, &exts, &paths, &names);

	if (bRet)
	{
		for (int i = 0; i < paths.size(); i++)
		{
			loadPSystem(names[i], paths[i]);
		}
	}
	else { return false; }

	return true;
}

void CPGCEffectManager::addToDeleteList(CPGCEffect* pEffect)
{
	m_pendingDeleteEffectQueue.push_back(pEffect);
}

int CPGCEffectManager::cleanUp()
{
	for (auto it : m_pendingDeleteEffectQueue)
	{
		it->release();
		delete it;
		it = nullptr;
	}

	int ret = m_pendingDeleteEffectQueue.size();
	m_pendingDeleteEffectQueue.clear();

	return ret;
}

CPRS_PSYSTEM_FILESTRUCT* CPGCEffectManager::getPSystem(std::wstring wszPSystemName)
{
	auto it = m_PSystemMap.find(wszPSystemName);

	if (it != m_PSystemMap.end())
	{
		return &it->second;
	}

	return nullptr;
}

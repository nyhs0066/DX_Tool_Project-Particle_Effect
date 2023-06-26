#include "CPGCSpriteManager.h"

CPGCSpriteManager::CPGCSpriteManager()
{
}

CPGCSpriteManager::~CPGCSpriteManager()
{
}

bool CPGCSpriteManager::init()
{
	m_wszDefaultUVScriptFilePath = L"../../data/save/script/spritescript/UVSpriteScript.UVSTXT";
	m_wszDefaultMTScriptFilePath = L"../../data/save/script/spritescript/MTSpriteScript.MTSTXT";

	loadUVScript(m_wszDefaultUVScriptFilePath);
	loadMTScript(m_wszDefaultMTScriptFilePath);

	return true;
}

bool CPGCSpriteManager::release()
{
	saveUVScript(m_wszDefaultUVScriptFilePath);
	saveMTScript(m_wszDefaultMTScriptFilePath);

	m_scriptFilenameSet.clear();

	for (auto it : m_UVSpriteMap)
	{
		delete it.second;
		it.second = nullptr;
	}

	for (auto it : m_MTspriteMap)
	{
		delete it.second;
		it.second = nullptr;
	}

	m_UVSpriteMap.clear();
	m_MTspriteMap.clear();

	return true;
}

bool CPGCSpriteManager::createUVSprite(std::wstring wszSpriteName, std::wstring wszTexName, std::wstring wszTexFilepath, UINT iRows, UINT iCols)
{
	if (m_UVSpriteMap.find(wszSpriteName) == m_UVSpriteMap.end())
	{
		if (FAILED(CPGC_TEXTURE_MGR.createTexture(wszTexName, wszTexFilepath)))
		{
			if (!CPGC_TEXTURE_MGR.getPtr(wszTexName)) { return false; }
		}

		CPRS_Sprite* newE = new CPRS_Sprite;
		newE->m_wszName = wszSpriteName;
		newE->m_wszUVTexName = wszTexName;
		newE->m_wszTexFilePath = wszTexFilepath;
		newE->m_pTexArr.push_back(CPGC_TEXTURE_MGR.getPtr(wszTexName));

		D3D11_TEXTURE2D_DESC td = newE->m_pTexArr[0]->getTexInfo();
		float singleCol = td.Width / (float)iCols;
		float singleRow = td.Height / (float)iRows;


		newE->m_spriteInfo.m_spriteType = SPRITE_UV;
		newE->m_spriteInfo.m_iRows = iRows;
		newE->m_spriteInfo.m_iCols = iCols;
		newE->m_spriteInfo.m_fSingleRowLen = singleCol;
		newE->m_spriteInfo.m_fSingleColLen = singleRow;
		newE->m_spriteInfo.m_texRECT = { {0.0f, 0.0f}, {(float)td.Width, (float)td.Height} };

		newE->m_spriteInfo.m_iStFrame = 0;
		newE->m_spriteInfo.m_iEdFrame = iRows * iCols - 1;
		newE->m_spriteInfo.m_iTotalFrame = iRows * iCols;

		for (int i = 0; i < iRows; i++)
		{
			for (int j = 0; j < iCols; j++)
			{
				newE->m_UVSpriteArr.push_back({ {(singleCol / td.Width) * j, (singleRow / td.Height) * i}, 
					{(singleCol / td.Width) * (j + 1), (singleRow / td.Height) * (i + 1)} });
			}
		}

		m_UVSpriteMap.insert(std::make_pair(wszSpriteName, newE));
	}
	else { return false; }

	return true;
}

bool CPGCSpriteManager::createMTSprite(std::wstring wszSpriteName,
	std::vector<std::wstring>* pWszTexNames,
	std::vector<std::wstring>* pWszTexFilepaths)
{
	if (m_MTspriteMap.find(wszSpriteName) == m_MTspriteMap.end())
	{
		if (pWszTexNames && pWszTexFilepaths)
		{
			CPRS_Sprite* newE = new CPRS_Sprite;

			for (int i = 0; i < pWszTexFilepaths->size(); i++)
			{
				if (FAILED(CPGC_TEXTURE_MGR.createTexture((*pWszTexNames)[i], (*pWszTexFilepaths)[i])))
				{
					if (!CPGC_TEXTURE_MGR.getPtr((*pWszTexNames)[i])) 
					{ 
						newE->m_pTexArr.clear();
						delete newE;
						return false; 
					}
				}

				newE->m_pTexArr.push_back(CPGC_TEXTURE_MGR.getPtr((*pWszTexNames)[i]));
			}

			newE->m_wszName = wszSpriteName;

			newE->m_spriteInfo.m_spriteType = SPRITE_MULTI_TEX;

			newE->m_spriteInfo.m_iStFrame = 0;
			newE->m_spriteInfo.m_iEdFrame = newE->m_pTexArr.size() - 1;
			newE->m_spriteInfo.m_iTotalFrame = newE->m_pTexArr.size();

			m_MTspriteMap.insert(std::make_pair(wszSpriteName, newE));
		}
		else { return false; }
	}
	else { return false; }
	return true;
}

CPRS_Sprite* CPGCSpriteManager::getUVPtr(std::wstring wszSpriteName)
{
	auto it = m_UVSpriteMap.find(wszSpriteName);
	if (it != m_UVSpriteMap.end()) { return it->second; }

	return nullptr;
}

CPRS_Sprite* CPGCSpriteManager::getMTPtr(std::wstring wszSpriteName)
{
	auto it = m_MTspriteMap.find(wszSpriteName);
	if (it != m_MTspriteMap.end()) { return it->second; }

	return nullptr;
}

void CPGCSpriteManager::getUVSpriteNames(std::vector<std::wstring>& wszNameList)
{
	wszNameList.clear();

	for (auto it : m_UVSpriteMap)
	{
		wszNameList.push_back(it.first);
	}
}

void CPGCSpriteManager::getMTSpriteNames(std::vector<std::wstring>& wszNameList)
{
	wszNameList.clear();

	for (auto it : m_MTspriteMap)
	{
		wszNameList.push_back(it.first);
	}
}

void CPGCSpriteManager::getUVSpriteNames(std::vector<std::string>& szNameList)
{
	szNameList.clear();

	for (auto it : m_UVSpriteMap)
	{
		szNameList.push_back(atl_W2M(it.first));
	}
}

void CPGCSpriteManager::getMTSpriteNames(std::vector<std::string>& szNameList)
{
	szNameList.clear();

	for (auto it : m_MTspriteMap)
	{
		szNameList.push_back(atl_W2M(it.first));
	}
}

int CPGCSpriteManager::loadUVScript(std::wstring wszUVScriptFilepath)
{
	//��ũ��Ʈ ���ϸ� �ߺ� Ȯ��
	if (m_scriptFilenameSet.end() != m_scriptFilenameSet.find(wszUVScriptFilepath)) { return false; }

	//�ʿ� ���� ����
	std::wifstream wiFp;
	wchar_t	scriptInfo[256] = { 0 };

	std::wstring wszCheckHeadStr;

	std::wstring wszSpriteName;
	std::wstring wszTexName;
	std::wstring wszTexFilepath;
	UINT iRows;
	UINT iCols;

	//ī��Ʈ�� ID�ε���
	UINT cnt = 0;		//�ҷ����⸦ ������ ���ҽ� ���� ��ȯ��

	//���� ����
	wiFp.open(wszUVScriptFilepath, std::ios::in);
	if (!wiFp) { return cnt; }

	wiFp >> wszCheckHeadStr;

	if (wszCheckHeadStr != L"#UVScript")
	{
		wiFp.close();
		return cnt;
	}

	//���ҽ� �б� ����
	while (true)
	{
		bool bRet = true;

		//�̸����� ������ Ȯ��
		wiFp >> wszSpriteName;
		if (wszSpriteName == L"#END") { break; }
		else
		{
			wiFp >> wszTexName;
			wiFp >> wszTexFilepath;
			wiFp >> iRows;
			wiFp >> iCols;

			bRet = createUVSprite(wszSpriteName,
				wszTexName,
				wszTexFilepath,
				iRows,
				iCols);

			CPGC_TEXTURE_MGR.createTexture(wszTexName, wszTexFilepath);

			if (bRet) { cnt++; }
		}
	}

	wiFp.close();

	//������ ������ ���� �Ŵ����� ���
	m_scriptFilenameSet.insert(wszUVScriptFilepath);

	return cnt;
}

int CPGCSpriteManager::saveUVScript(std::wstring wszUVScriptFilepath)
{
	//�ʿ� ���� ����
	std::wofstream woFp;
	wchar_t	scriptInfo[256] = { 0 };

	//ī��Ʈ
	UINT cnt = 0;		//�ҷ����⸦ ������ ���ҽ� ���� ��ȯ��

	//���� ����
	woFp.open(wszUVScriptFilepath, std::ios::out);
	if (!woFp) { return cnt; }

	woFp << L"#UVScript";
	woFp << L"\n";

	//���ҽ� ���� ����
	for(auto it : m_UVSpriteMap)
	{
		woFp << it.second->m_wszName;
		woFp << L"\n";

		woFp << it.second->m_wszUVTexName;
		woFp << L"\n";

		woFp << it.second->m_wszTexFilePath;
		woFp << L"\n";

		woFp << it.second->m_spriteInfo.m_iRows;
		woFp << L"\n";

		woFp << it.second->m_spriteInfo.m_iCols;
		woFp << L"\n";

		cnt++;
	}

	woFp << L"#END";

	woFp.close();

	return cnt;
}

int CPGCSpriteManager::loadMTScript(std::wstring wszMTScriptFilepath)
{
	//��ũ��Ʈ ���ϸ� �ߺ� Ȯ��
	if (m_scriptFilenameSet.end() != m_scriptFilenameSet.find(wszMTScriptFilepath)) { return false; }

	//�ʿ� ���� ����
	std::wifstream wiFp;
	wchar_t	scriptInfo[256] = { 0 };

	std::wstring wszCheckHeadStr;

	std::wstring wszSpriteName;
	std::wstring wszTexName;
	std::wstring wszTexFilepath;
	std::vector<std::wstring> wszTexNameList;
	std::vector<std::wstring> wszTexFilepathList;

	//ī��Ʈ�� ID�ε���
	UINT cnt = 0;		//�ҷ����⸦ ������ ���ҽ� ���� ��ȯ��

	//���� ����
	wiFp.open(wszMTScriptFilepath, std::ios::in);
	if (!wiFp) { return cnt; }

	wiFp >> wszCheckHeadStr;

	if (wszCheckHeadStr != L"#MTScript")
	{
		wiFp.close();
		return cnt;
	}

	//���ҽ� �б� ����
	while (true)
	{
		bool bRet = true;

		//�̸����� ������ Ȯ��
		wiFp >> wszSpriteName;
		if (wszSpriteName == L"#END") { break; }
		else
		{
			wiFp >> wszTexName;
			while (wszTexName != L"#TEXLISTEND")
			{
				wszTexNameList.push_back(wszTexName);
				wiFp >> wszTexName;
			}

			int N = 0;
			wiFp >> wszTexFilepath;
			while (wszTexFilepath != L"#TEXPATHLISTEND")
			{
				wszTexFilepathList.push_back(wszTexFilepath);
				CPGC_TEXTURE_MGR.createTexture(wszTexNameList[N++], wszTexFilepath);

				wiFp >> wszTexFilepath;
			}

			bRet = createMTSprite(wszSpriteName,
				&wszTexNameList,
				&wszTexFilepathList);

			if (bRet) { cnt++; }

			wszTexNameList.clear();
			wszTexFilepathList.clear();
		}
	}

	wiFp.close();

	//������ ������ ���� �Ŵ����� ���
	m_scriptFilenameSet.insert(wszMTScriptFilepath);

	return cnt;
}

int CPGCSpriteManager::saveMTScript(std::wstring wszMTScriptFilepath)
{
	//�ʿ� ���� ����
	std::wofstream woFp;
	wchar_t	scriptInfo[256] = { 0 };

	//ī��Ʈ
	UINT cnt = 0;		//�ҷ����⸦ ������ ���ҽ� ���� ��ȯ��

	//���� ����
	woFp.open(wszMTScriptFilepath, std::ios::out);
	if (!woFp) { return cnt; }

	woFp << L"#MTScript";
	woFp << L"\n";

	//���ҽ� ���� ����
	for (auto it : m_MTspriteMap)
	{
		woFp << it.second->m_wszName;
		woFp << L"\n";

		for (auto Texit : it.second->m_pTexArr)
		{
			woFp << Texit->getName();
			woFp << L"\n";
		}

		woFp << L"#TEXLISTEND\n";

		for (auto Texit : it.second->m_pTexArr)
		{
			woFp << Texit->getFilePath();
			woFp << L"\n";
		}

		woFp << L"#TEXPATHLISTEND\n";
	}

	woFp << L"#END";

	woFp.close();

	return cnt;
}

//////////////////////////////////////////////////
//
// CPGCSpriteManager.h
//		2D스프라이트 통합 관리 매니저
//		UV, Texture교체 애니메이션 정보를 가진다.
// 
//////////////////////////////////////////////////

#pragma once
#include "CPRSShape.h"
#include "CPGCTextureManager.h"

using UV_ARRAY = std::vector<CPRS_RECT>;
using TEX_ARRAY = std::vector<CPRS_Texture*>;

enum CPRS_SPRITE_TYPE
{
	SPRITE_UV = 0,
	SPRITE_MULTI_TEX,
	NUMBER_OF_SPRITES
};

struct CPRS_SPRITE_INFO
{
	CPRS_SPRITE_TYPE		m_spriteType;

	//UV
	UINT					m_iRows;
	UINT					m_iCols;
	float					m_fSingleRowLen;
	float					m_fSingleColLen;
	CPRS_RECT				m_texRECT;

	//COMMON
	UINT					m_iStFrame;
	UINT					m_iEdFrame;
	UINT					m_iTotalFrame;
};

struct CPRS_Sprite
{
	std::wstring			m_wszName;
	std::wstring			m_wszUVTexName;
	std::wstring			m_wszTexFilePath;
	CPRS_SPRITE_INFO		m_spriteInfo;

	UV_ARRAY				m_UVSpriteArr;
	TEX_ARRAY				m_pTexArr;		//멀티 텍스처 스프라이트 구조로도 사용하고 단일 텍스처 UV스프라이트의 텍스처를 보관할때도 사용한다.
};

class CPGCSpriteManager
{
#pragma region region1
private:
	std::set<std::wstring>					m_scriptFilenameSet;
	std::map<std::wstring, CPRS_Sprite*>	m_UVSpriteMap;
	std::map<std::wstring, CPRS_Sprite*>	m_MTspriteMap;

	CPGCSpriteManager();
	~CPGCSpriteManager();

public:
	std::wstring							m_wszDefaultUVScriptFilePath;
	std::wstring							m_wszDefaultMTScriptFilePath;

	CPGCSpriteManager(const CPGCSpriteManager& other) = delete;
	CPGCSpriteManager& operator=(const CPGCSpriteManager& other) = delete;
#pragma endregion region1
public:
	bool init();
	bool release();

	bool createUVSprite(std::wstring wszSpriteName,
		std::wstring wszTexName,
		std::wstring wszTexFilepath,
		UINT iRows, UINT iCols);

	bool createMTSprite(std::wstring wszSpriteName,
		std::vector<std::wstring>* pWszTexNames,
		std::vector<std::wstring>* pWszTexFilepaths);

	int loadUVScript(std::wstring wszUVScriptFilepath);
	int saveUVScript(std::wstring wszUVScriptFilepath);

	int loadMTScript(std::wstring wszMTScriptFilepath);
	int saveMTScript(std::wstring wszMTScriptFilepath);

	CPRS_Sprite* getUVPtr(std::wstring wszSpriteName);
	CPRS_Sprite* getMTPtr(std::wstring wszSpriteName);

	void getUVSpriteNames(std::vector<std::wstring>& wszNameList);
	void getUVSpriteNames(std::vector<std::string>& szNameList);

	void getMTSpriteNames(std::vector<std::wstring>& wszNameList);
	void getMTSpriteNames(std::vector<std::string>& szNameList);

	static CPGCSpriteManager& getInstance()
	{
		static CPGCSpriteManager singleInst;
		return singleInst;
	}
};

#define CPGC_SPRITE_MGR CPGCSpriteManager::getInstance()
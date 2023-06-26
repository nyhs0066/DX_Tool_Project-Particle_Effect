#pragma once
#include "CPGCStd.h"
#include "CPGCIMGUIModule.h"
#include "CPGCTextureManager.h"

class CPIMGUIControl
{
public:
	virtual void showUIControl(const char* ID) = 0;
};

class CPIMGUITexSelector : public CPIMGUIControl
{
public:
	//텍스처 검색
	std::vector<std::wstring>						m_wszTargetExt;
	std::string										m_szTexSearchPath;

	//텍스처 선택 콤보 박스 정보
	std::vector<std::wstring>						m_wszTexFilePaths;
	std::vector<std::string>						m_szTexNames;
	std::string										m_szSelectedTexName;
	std::wstring									m_wszSelectedTexName;
	std::wstring									m_wszSelectedTexPath;
	CPRS_Texture*									m_pSelectedTex;
	int												m_iTexIdx;

	CPIMGUITexSelector();

	bool init(std::string szTexSearchPath);
	void selectClear();
	virtual void showUIControl(const char* ID) override;
};

class CPIMGUIMultiTexSelector : public CPIMGUIControl
{
public:
	//텍스처 검색
	std::vector<std::wstring>						m_wszTargetExt;
	std::string										m_szTexSearchPath;

	//텍스처 선택 콤보 박스 정보
	std::vector<std::wstring>						m_wszTexFilePaths;
	std::vector<std::string>						m_szTexNames;
	std::string										m_szSelectedTexName;
	std::wstring									m_wszSelectedTexName;
	std::wstring									m_wszSelectedTexPath;
	CPRS_Texture*									m_pSelectedTex;
	int												m_iTexIdx;
	std::vector<char>								m_bSelection;

	std::vector<std::wstring>						m_wszSelectedTexFilePaths;
	std::vector<std::string>						m_szSelectedTexNames;
	std::vector<std::wstring>						m_wszSelectedTexNames;
	int												m_iTexIdx2;

	CPIMGUIMultiTexSelector();

	bool init(std::string szTexSearchPath);
	void selectClear();
	virtual void showUIControl(const char* ID) override;
};

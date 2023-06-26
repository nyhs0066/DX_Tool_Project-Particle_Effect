//////////////////////////////////////////////////
//
// CPTextureManager.h
//		텍스처 통합 관리 매니저
//		텍스처마다 자신이 샘플링될 스테이트를 지정한다.
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"

//DX Tool Kit 텍스처 로더를 위한 헤더
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

class CPRS_Texture
{
	friend class CPGCTextureManager;
private:
	ID3D11DeviceContext*								m_pDContext;

	std::wstring										m_wszName;
	std::wstring										m_wszFilePath;

	D3D11_TEXTURE2D_DESC								m_texDesc;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_texBuf;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRView;

public:
	CPRS_Texture();
	~CPRS_Texture();

	D3D11_TEXTURE2D_DESC								getTexInfo();
	ID3D11Texture2D*									getTexBuf();
	ID3D11ShaderResourceView*							getSRV();
	std::wstring										getName();
	std::wstring										getFilePath();
};

class CPGCTextureManager
{
#pragma region region_1
private:
	ID3D11Device*							m_pDevice;
	ID3D11DeviceContext*					m_pDContext;

	std::set<std::wstring>					m_scriptFilenameSet;

	std::map<std::wstring, std::wstring>	m_filenameMap;
	std::map<std::wstring, CPRS_Texture*>	m_textureMap;

	CPGCTextureManager();
	~CPGCTextureManager();

public:
	CPGCTextureManager(const CPGCTextureManager& other) = delete;
	CPGCTextureManager& operator=(const CPGCTextureManager& other) = delete;
#pragma endregion region_1
public:
	bool init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);
	bool release();

	HRESULT createTexture(std::wstring wszTexureName, std::wstring wszTexFilename);
	HRESULT createTextureEx(std::wstring wszTexureName,
		std::wstring wszTexFilename,
		D3D11_USAGE resourceUsage,
		D3D11_CPU_ACCESS_FLAG CPUFlag);

	HRESULT createAlphaMaskTexture(UINT iWidth, UINT iHeight);

	CPRS_Texture* getPtr(std::wstring wszTexureName);

	static CPGCTextureManager& getInstance()
	{
		static CPGCTextureManager singleInst;
		return singleInst;
	}
};

#define CPGC_TEXTURE_MGR CPGCTextureManager::getInstance()
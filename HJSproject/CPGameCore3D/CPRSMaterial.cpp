#include "CPRSMaterial.h"

CPRS_Material::CPRS_Material()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;
}

CPRS_Material::~CPRS_Material()
{
}

bool CPRS_Material::init()
{
	return true;
}

bool CPRS_Material::release()
{
	return true;
}

HRESULT CPRS_Material::create(	ID3D11Device* pDevice,
								ID3D11DeviceContext* pDContext,
								std::wstring wszShaderName,
								LPCWSTR* wszTextureFilenameArr,
								UINT iTexNum)
{
	HRESULT hr = S_OK;

	if (!pDevice || !pDContext) { hr = E_POINTER; printCreateErr(hr); return hr; }
	else
	{
		m_pDevice = pDevice;
		m_pDContext = pDContext;
	}

	loadShaderGroup(wszShaderName);

	for (UINT i = 0; i < iTexNum; i++)
	{
		if (wszTextureFilenameArr[i]) { loadTexture(wszTextureFilenameArr[i]); }
	}

	return hr;
}

void CPRS_Material::loadShaderGroup(std::wstring wszShaderName)
{
	CPGC_SHADER_MGR.getShaderPreset(wszShaderName, m_shaderGroup);
}

bool CPRS_Material::loadTexture(std::wstring wszTextureName)
{
	CPRS_Texture* pTex = CPGC_TEXTURE_MGR.getPtr(wszTextureName);

	if (!pTex) { return false; }

	m_pTextureArr.push_back(pTex);
	m_pTexSRVArr.push_back(pTex->getSRV());
	return true;
}

void CPRS_Material::bindToPipeline()
{
	m_shaderGroup.bindToPipeline(m_pDContext);

	if (m_pTextureArr.size()) 
	{
		UINT count = m_pTextureArr.size();

		m_pDContext->PSSetShaderResources(0, count, &m_pTexSRVArr.at(0));
	}
}
//////////////////////////////////////////////////
//
// CPGCMaterial.h
//		Mesh의 렌더 특성 결정 헤더
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCShaderManager.h"
#include "CPGCTextureManager.h"
#include "CPGCDxStateManager.h"

class CPRS_Material
{
protected:
	ID3D11Device*								m_pDevice;
	ID3D11DeviceContext*						m_pDContext;

	CPRS_ShaderGroup							m_shaderGroup;
	std::vector<CPRS_Texture*>					m_pTextureArr;
	std::vector<ID3D11ShaderResourceView*>		m_pTexSRVArr;

public:
	CPRS_Material();
	virtual ~CPRS_Material();

	virtual bool					init();
	virtual bool					release();

	virtual HRESULT					create(	ID3D11Device* pDevice, 
											ID3D11DeviceContext* pDContext, 
											std::wstring wszShaderName,
											LPCWSTR* wszTextureFilenameArr,
											UINT iTexNum);

	virtual void loadShaderGroup(std::wstring wszShaderName);
	virtual bool loadTexture(std::wstring wszTextureName);
	virtual void bindToPipeline();
};
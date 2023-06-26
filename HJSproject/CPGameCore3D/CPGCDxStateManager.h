//////////////////////////////////////////////////
//
// CPGCDxStateManager.h
//		���������� ó�� Ư���� �����ϴ� state ���� �Ŵ���
//		state ����
//			SamplerState
//				�ؽ�ó UV�� �� �ܰ��� ��� ������ ��Ī �� ������ ���� ���ø� ����� �����Ѵ�.
//				���α׷��� ������ Shader�� �� �ܰ躰�� SamplerState�� ���ε� �� ���ִ�.
//				���÷��� �ټ��� �ѹ��� ���ε� �����ϴ�. 
//				Dx 11�� shader�ܰ�� 16�������� ��밡���ϴ�.
// 
//			RasterizerState
//				RS �ܰ迡�� ���� ��������� ����/�ĸ� ���� ��� �� �ø�, ��Ƽ ���ø��� ���� 
//				���� -> ȭ�� ��ȯ������ ���� ����� �����Ѵ�.
// 
//			BlendState
//				OM�ܰ迡�� ��� ��ҵ��� ��� �ռ�, ����, ����, ���� �Ͽ� ���� ����� �������� ���� ����� �����Ѵ�.
// 
//			DepthStencilState
//				���Ͽ���� ���� - ���ٽ� ���� ���� ��� ��Ҹ� �����ϴ� ����� �����Ѵ�.
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"

enum CPRS_SSType
{
	SS_LINEAR_CLAMP = 0,
	SS_LINEAR_WRAP,
	SS_POINT_WRAP,
	SS_SHADOWMAP_COMP_BORDER,
	SS_NUMBER_OF_TYPE
};

enum CPRS_RSType
{
	RS_SOLID = 0,
	RS_SOLID_NOCULL,
	RS_SOLID_FRONTCULL,
	RS_WIREFRAME,
	RS_WIREFRAME_NOCULL,
	RS_WIREFRAME_FRONTCULL,
	RS_SHADOWMAP_APPLYBIAS,
	RS_NUMBER_OF_TYPE
};

enum CPRS_BSType
{
	BS_DEFAULT = 0,
	BS_MSALPHATEST,
	BS_ALPHABLEND,
	BS_DUALSOURCEBLEND,
	BS_ADDITIVEBLEND,
	//BS_DUALSOURCEADDITIVEBLEND,
	BS_NOSOURCEBLEND,
	BS_NUMBER_OF_TYPE
};

//DEPTH [COMPARISON]
//STENCIL [COMPARISON] [TARGET FACE] [PASS OP] [FAIL OP] [DEPTH FAIL OP]
enum CPRS_DSSType
{
	DSS_DISABLE = 0,
	DSS_D_COMPLESS,
	DSS_D_COMPGREATER,
	DSS_D_COMPLESS_NOWRITE,

	DSS_S_EQUAL_FRONT_KEEP,

	DSS_DS_ALWAYS_FRONT_INC,
	DSS_DS_ALWAYS_FRONT_REPLACE,
	DSS_DS_GREATER_FRONT_INC,
	DSS_DS_GREATER_FRONT_REPLACE,
	DSS_DS_NOTEQUAL_FRONT_KEEP,
	DSS_NUMBER_OF_TYPE
};

class CPGCDXStateManager
{
#pragma region region_1
private:
	ID3D11Device*										m_pDevice;
	ID3D11DeviceContext*								m_pDContext;

	std::map<std::wstring, ID3D11SamplerState*>			m_SStateMap;
	std::map<std::wstring, ID3D11RasterizerState*>		m_RStateMap;
	std::map<std::wstring, ID3D11BlendState*>			m_BStateMap;
	std::map<std::wstring, ID3D11DepthStencilState*>	m_DSStateMap;

	CPGCDXStateManager();
	~CPGCDXStateManager();

	HRESULT												createDefaultStates();

public:
	CPGCDXStateManager(const CPGCDXStateManager& other) = delete;
	CPGCDXStateManager& operator=(const CPGCDXStateManager& other) = delete;
#pragma endregion region_1
public:
	bool init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);
	bool release();
	
	HRESULT						createSState(std::wstring wszSStateName, D3D11_SAMPLER_DESC samplerDesc);
	HRESULT						createRState(std::wstring wszRStateName, D3D11_RASTERIZER_DESC rasterizerDesc);
	HRESULT						createBState(std::wstring wszBStateName, D3D11_BLEND_DESC blendDesc);
	HRESULT						createDSState(std::wstring wszDSStateName, D3D11_DEPTH_STENCIL_DESC depthStencilDesc);

	ID3D11SamplerState*			getSState(std::wstring wszSStateName);
	ID3D11RasterizerState*		getRState(std::wstring wszRStateName);
	ID3D11BlendState*			getBState(std::wstring wszBStateName);
	ID3D11DepthStencilState*	getDSState(std::wstring wszDSStateName);

	static CPGCDXStateManager& getInstance()
	{
		static CPGCDXStateManager singleInst;
		return singleInst;
	}
};

#define CPGC_DXSTATE_MGR CPGCDXStateManager::getInstance()

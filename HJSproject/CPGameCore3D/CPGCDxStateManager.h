//////////////////////////////////////////////////
//
// CPGCDxStateManager.h
//		파이프라인 처리 특성을 결정하는 state 관리 매니저
//		state 종류
//			SamplerState
//				텍스처 UV와 각 단계의 요소 색상의 매칭 및 보간을 위한 샘플링 방법을 지정한다.
//				프로그래밍 가능한 Shader는 각 단계별로 SamplerState를 바인딩 할 수있다.
//				샘플러는 다수를 한번에 바인딩 가능하다. 
//				Dx 11은 shader단계당 16개까지만 사용가능하다.
// 
//			RasterizerState
//				RS 단계에서 기하 구성요소의 전면/후면 구성 방식 및 컬링, 멀티 샘플링과 같은 
//				공간 -> 화면 변환절차에 대한 방법을 지정한다.
// 
//			BlendState
//				OM단계에서 출력 요소들을 어떻게 합성, 보간, 병합, 배제 하여 최종 출력할 것인지에 대한 방법을 지정한다.
// 
//			DepthStencilState
//				기하요소의 깊이 - 스텐실 값에 따른 출력 요소를 결정하는 방식을 지정한다.
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

//////////////////////////////////////////////////
//
// CPGCRenderTarget.h
//		텍스처기반 버퍼를 이용한 렌더 타겟 클래스
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"
#include "CPGCDxStateManager.h"

#define RTV_DEFAULT_CLEAR_GRAYSCALE		0.15f
#define DEPTH_CLEAR_VALUE				1.0f
#define STENCIL_CLEAR_VALUE				0

#define MAX_NUMBER_OF_RENDER_TARGET		4

//OM스테이지에 8개까지만 바인딩 할 수 있으므로 유의
class CPGCRenderTarget
{
public:
	ID3D11Device*										m_pDevice;
	ID3D11DeviceContext*								m_pDContext;

	//텍스처 렌더타겟이 사용할 부분
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_pDSView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pRTSRView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pDSSRView;

	//뷰포트
	D3D11_VIEWPORT										m_viewPort;

	//초기화 색상
	FLOAT												clearColor[4];

	//변경하면서 이전 렌더타겟 내용을 복구하기 위해 임시 저장하는 부분
	//뷰 포트와 렌더 타겟 뷰, 깊이 스텐실 뷰와 이전에 사용한 뷰포트 개수까지 필요하다.
	ID3D11RenderTargetView*								m_pPrevRTV[8];
	ID3D11DepthStencilView*								m_pPrevDSV;
	D3D11_VIEWPORT										m_prevVPorts[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT												m_iVPnum;	//불러올 뷰포트의 개수

public:
	CPGCRenderTarget();
	virtual ~CPGCRenderTarget();

	bool init();
	bool release();

	HRESULT create(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext,
		UINT iWidth = 1024, UINT iHeight = 1024);
	void clearViews();
	bool bindToPipeline();
	void getPipelineRTResource();

	void drawStart();
	void drawEnd();

	ID3D11ShaderResourceView*					getRTSRV();
	ID3D11ShaderResourceView*					getDSSRV();

	HRESULT resize(UINT iWidth, UINT iHeight);

	const D3D11_VIEWPORT& getViewport();
};


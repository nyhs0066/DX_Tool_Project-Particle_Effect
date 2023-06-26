//////////////////////////////////////////////////
//
// CPGCRenderTarget.h
//		�ؽ�ó��� ���۸� �̿��� ���� Ÿ�� Ŭ����
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"
#include "CPGCDxStateManager.h"

#define RTV_DEFAULT_CLEAR_GRAYSCALE		0.15f
#define DEPTH_CLEAR_VALUE				1.0f
#define STENCIL_CLEAR_VALUE				0

#define MAX_NUMBER_OF_RENDER_TARGET		4

//OM���������� 8�������� ���ε� �� �� �����Ƿ� ����
class CPGCRenderTarget
{
public:
	ID3D11Device*										m_pDevice;
	ID3D11DeviceContext*								m_pDContext;

	//�ؽ�ó ����Ÿ���� ����� �κ�
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_pDSView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pRTSRView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pDSSRView;

	//����Ʈ
	D3D11_VIEWPORT										m_viewPort;

	//�ʱ�ȭ ����
	FLOAT												clearColor[4];

	//�����ϸ鼭 ���� ����Ÿ�� ������ �����ϱ� ���� �ӽ� �����ϴ� �κ�
	//�� ��Ʈ�� ���� Ÿ�� ��, ���� ���ٽ� ��� ������ ����� ����Ʈ �������� �ʿ��ϴ�.
	ID3D11RenderTargetView*								m_pPrevRTV[8];
	ID3D11DepthStencilView*								m_pPrevDSV;
	D3D11_VIEWPORT										m_prevVPorts[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT												m_iVPnum;	//�ҷ��� ����Ʈ�� ����

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


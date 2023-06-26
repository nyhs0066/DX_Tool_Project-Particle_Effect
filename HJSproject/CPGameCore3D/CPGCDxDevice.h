//////////////////////////////////////////////////
//
// CPGCDxDevice.h
//		�׷��� ����� ���� DX ����̽� �������̽� ���� ���
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCRenderTarget.h"

//���� ī�� ��� ��ġ ����
struct DxOutput
{
	Microsoft::WRL::ComPtr<IDXGIOutput> m_pOutput;
	DXGI_OUTPUT_DESC m_desc;
};

//���� ī�� ����
struct DXAdapter
{
	Microsoft::WRL::ComPtr<IDXGIAdapter> m_pAdapter;
	DXGI_ADAPTER_DESC m_desc;
	std::vector<DxOutput> m_outputList;
};

class CPGCDxDevice
{
private:
	//���� ī�� �� ī�忡 ���ε��� ��� ��ġ ����Ʈ
	std::vector<DXAdapter>								m_adapterList;

	//Device / Factory �������̽�
	Microsoft::WRL::ComPtr<ID3D11Device>				m_pDevice;
	Microsoft::WRL::ComPtr<IDXGIFactory>				m_pDXGIFactory;

	//Device context �������̽�
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_pIMDContext;		//���� DC
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_pDEDContext;		//���� DC

	//Swapchain �������̽�
	Microsoft::WRL::ComPtr<IDXGISwapChain>				m_pSwapchain;

	// RTV / DSV
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_pDSView;

	// RTV SRV / DSV SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pRTSRView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pDSSRView;

	//Viewports
	D3D11_VIEWPORT									m_VPort;

	//���� Ÿ�� Ŭ���� ���� ����
	FLOAT											m_clearColor[4];

	CPGCDxDevice();
	~CPGCDxDevice();

	//create�Լ��� ���� �Լ���
	HRESULT								createDXGIFactoryInterface();
	HRESULT								createDeviceInterface();
	HRESULT								createSwapchain(HWND hwnd, UINT iWidth, UINT iHeight);
	HRESULT								createRTView();
	HRESULT								createDSView(UINT iWidth, UINT iHeight);

	//����� / ��� ��ġ ����Ʈ ����
	void								createAdapterList();

public:
	CPGCDxDevice(const CPGCDxDevice& other) = delete;
	CPGCDxDevice& operator=(const CPGCDxDevice& other) = delete;

	//����Ÿ�� Ŭ���� �÷���
	bool											m_bNoClear;

	//��üȭ�� �÷���
	BOOL											m_bFullscreen;

public:
	bool								init();
	bool								release();

	//DX Resource ����
	HRESULT								create(HWND hwnd, UINT iWidth, UINT iHeight);

	//���������ο� ���� ���� ���ε�
	void								clearRenderTarget();
	void								clearShaderResource();
	void								bindToPipeline();

	//ȭ�� ���������̼�
	void								presentation();

	//Viewport ����
	void								setViewport(UINT iWidth, UINT iHeight);

	//ũ�� ���� ������� ������ �۾�
	bool								resizeComponent(UINT iWidth, UINT iHeight);

	//���� �� ������ �ʿ��� ���ҽ� ��ȯ
	ID3D11Device*						getDevice();
	ID3D11DeviceContext*				getImDContext();
	ID3D11DeviceContext*				getDeDContext();
	IDXGISwapChain*						getSwapChain();
	ID3D11RenderTargetView*				getRTV();
	ID3D11ShaderResourceView*			getRTSRV();
	ID3D11ShaderResourceView*			getDSSRV();

	//���� �ν��Ͻ� ��ȯ
	static CPGCDxDevice& getInstance()
	{
		static CPGCDxDevice singleInst;
		return singleInst;
	}
};

#define CPGC_DXMODULE		CPGCDxDevice::getInstance()
#define I_DEVICE			CPGCDxDevice::getDevice()
#define I_IM_DCONTEXT		CPGCDxDevice::getImDContext()
#define I_DE_DCONTEXT		CPGCDxDevice::getDeDContext()

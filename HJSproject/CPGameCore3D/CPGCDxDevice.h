//////////////////////////////////////////////////
//
// CPGCDxDevice.h
//		그래픽 출력을 위한 DX 디바이스 인터페이스 모음 헤더
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCRenderTarget.h"

//비디오 카드 출력 장치 정보
struct DxOutput
{
	Microsoft::WRL::ComPtr<IDXGIOutput> m_pOutput;
	DXGI_OUTPUT_DESC m_desc;
};

//비디오 카드 정보
struct DXAdapter
{
	Microsoft::WRL::ComPtr<IDXGIAdapter> m_pAdapter;
	DXGI_ADAPTER_DESC m_desc;
	std::vector<DxOutput> m_outputList;
};

class CPGCDxDevice
{
private:
	//비디오 카드 및 카드에 바인딩된 출력 장치 리스트
	std::vector<DXAdapter>								m_adapterList;

	//Device / Factory 인터페이스
	Microsoft::WRL::ComPtr<ID3D11Device>				m_pDevice;
	Microsoft::WRL::ComPtr<IDXGIFactory>				m_pDXGIFactory;

	//Device context 인터페이스
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_pIMDContext;		//직접 DC
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_pDEDContext;		//지연 DC

	//Swapchain 인터페이스
	Microsoft::WRL::ComPtr<IDXGISwapChain>				m_pSwapchain;

	// RTV / DSV
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_pDSView;

	// RTV SRV / DSV SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pRTSRView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pDSSRView;

	//Viewports
	D3D11_VIEWPORT									m_VPort;

	//렌더 타겟 클리어 색상 변수
	FLOAT											m_clearColor[4];

	CPGCDxDevice();
	~CPGCDxDevice();

	//create함수의 보조 함수들
	HRESULT								createDXGIFactoryInterface();
	HRESULT								createDeviceInterface();
	HRESULT								createSwapchain(HWND hwnd, UINT iWidth, UINT iHeight);
	HRESULT								createRTView();
	HRESULT								createDSView(UINT iWidth, UINT iHeight);

	//어댑터 / 출력 장치 리스트 생성
	void								createAdapterList();

public:
	CPGCDxDevice(const CPGCDxDevice& other) = delete;
	CPGCDxDevice& operator=(const CPGCDxDevice& other) = delete;

	//렌더타겟 클리어 플래그
	bool											m_bNoClear;

	//전체화면 플래그
	BOOL											m_bFullscreen;

public:
	bool								init();
	bool								release();

	//DX Resource 생성
	HRESULT								create(HWND hwnd, UINT iWidth, UINT iHeight);

	//파이프라인에 렌더 설정 바인딩
	void								clearRenderTarget();
	void								clearShaderResource();
	void								bindToPipeline();

	//화면 프레젠테이션
	void								presentation();

	//Viewport 설정
	void								setViewport(UINT iWidth, UINT iHeight);

	//크기 의존 구성요소 재조정 작업
	bool								resizeComponent(UINT iWidth, UINT iHeight);

	//생성 및 렌더에 필요한 리소스 반환
	ID3D11Device*						getDevice();
	ID3D11DeviceContext*				getImDContext();
	ID3D11DeviceContext*				getDeDContext();
	IDXGISwapChain*						getSwapChain();
	ID3D11RenderTargetView*				getRTV();
	ID3D11ShaderResourceView*			getRTSRV();
	ID3D11ShaderResourceView*			getDSSRV();

	//전역 인스턴스 반환
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

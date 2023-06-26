#include "CPGCDxDevice.h"

CPGCDxDevice::CPGCDxDevice()
{
	ZeroMemory(m_clearColor, sizeof(m_clearColor));

	for (int i = 0; i < 3; i++) { m_clearColor[i] = RTV_DEFAULT_CLEAR_GRAYSCALE; }
	m_clearColor[3] = 1.0f;

	m_bFullscreen = FALSE;

	m_bNoClear = false;
}

CPGCDxDevice::~CPGCDxDevice()
{
}

bool CPGCDxDevice::init()
{
	return true;
}

bool CPGCDxDevice::release()
{
	return true;
}

HRESULT CPGCDxDevice::create(HWND hwnd, UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	hr = createDXGIFactoryInterface();
	if (FAILED(hr)) { printCreateErr(hr); return hr; }

	createAdapterList();
	if (FAILED(hr)) { printCreateErr(hr); return hr; }

	hr = createDeviceInterface();
	if (FAILED(hr)) { printCreateErr(hr); return hr; }

	hr = createSwapchain(hwnd, iWidth, iHeight);
	if (FAILED(hr)) { printCreateErr(hr); return hr; }

	hr = createRTView();
	if (FAILED(hr)) { printCreateErr(hr); return hr; }

	hr = createDSView(iWidth, iHeight);
	if (FAILED(hr)) { printCreateErr(hr); return hr; }

	setViewport(iWidth, iHeight);

	return hr;
}

HRESULT CPGCDxDevice::createDXGIFactoryInterface()
{
	HRESULT hr = S_OK;

	hr = CreateDXGIFactory(_uuidof(IDXGIFactory), (void**)m_pDXGIFactory.GetAddressOf());

	return hr;
}

HRESULT CPGCDxDevice::createDeviceInterface()
{
	HRESULT hr = S_OK;

	//디바이스 생성 플래그 : 사용할 멀티 / 싱글 스레드 지정 가능
	UINT createflag = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	//DX 특성 레벨 : 사용할 DX버전을 우선순위대로 열거한 리스트
	D3D_FEATURE_LEVEL flArray[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_12_0
	};

	//사용될 DX 특성 레벨을 반환 받는 변수 : 넘겨준 리스트에서 어떤 DX 수준이 사용될 것인지를 나타낸다.
	D3D_FEATURE_LEVEL selectedFl;

	//Adapter를 지정하는 경우에는 반드시 D3D_DRIVER_TYPE_UNKNOWN이어야 한다.
	//Adapter를 미지정 하는 경우에는 기본값으로 적용될 D3D_DRIVER_TYPE을 별도로 지정해 주어야 한다.
	hr = D3D11CreateDevice(m_adapterList[0].m_pAdapter.Get(),		//사용 어댑터
							D3D_DRIVER_TYPE_UNKNOWN,				//드라이버 타입
							NULL,									//드라이버 타입이 소프트웨어 렌더인경우 해당 소프트웨어의 핸들
							createflag,								//드라이버 생성 플래그
							flArray,								//DX 특성 레벨 리스트
							CPGC_ARRAY_ELEM_NUM(flArray),						//리스트 원소 개수
							D3D11_SDK_VERSION,						//DX SDK 버전
							m_pDevice.GetAddressOf(),				//반환받을 디바이스 인터페이스 변수
							&selectedFl,							//선택된 DX 특성 수준
							m_pIMDContext.GetAddressOf()			//반환받을 DC 인터페이스
							);

	if (FAILED(hr)) { return hr; }

	hr = m_pDevice->CreateDeferredContext(0, m_pDEDContext.GetAddressOf());		//지연 DC 생성

	return hr;
}

HRESULT CPGCDxDevice::createSwapchain(HWND hwnd, UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC dscd;
	ZeroMemory(&dscd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//화면 영역 크기
	dscd.BufferDesc.Width = iWidth;
	dscd.BufferDesc.Height = iHeight;

	//출력 장치 컬러 포맷
	dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//화면 재생률
	dscd.BufferDesc.RefreshRate.Denominator = 1;
	dscd.BufferDesc.RefreshRate.Numerator = 60;

	//샘플링 심도
	dscd.SampleDesc.Count = 1;
	dscd.SampleDesc.Quality = 0;

	//버퍼 사용 용도 및 개수
	dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	dscd.BufferCount = 2;									//백 버퍼의 개수

	//출력 어플리케이션의 창 핸들과 윈도우 여부
	dscd.OutputWindow = hwnd;
	dscd.Windowed = true;

	//버퍼 교체 옵션 및 각종 플래그
	//스왑체인의 프론트 버퍼에는 접근할 수 없으며 
	//DXGI_SWAP_EFFECT_DISCARD : Present이후 백 버퍼의 내용을 버린다. 0번 스왑체인 백 버퍼에 접근 할 수 있다.
	dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;				
	dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	hr = m_pDXGIFactory->CreateSwapChain(m_pDevice.Get(), &dscd, m_pSwapchain.GetAddressOf());
	//if (FAILED(hr)) { return hr; }

	return hr;
}

HRESULT CPGCDxDevice::createRTView()
{
	HRESULT hr = S_OK;

	//렌더 타겟용 백 버퍼를 스왑체인에서 불러오기
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuf;

	//Swap Effect가 DXGI_SWAP_EFFECT_DISCARD 또는 DXGI_SWAP_EFFECT_FLIP_DISCARD인 경우 0번 버퍼만 읽고 쓸 수 있다.
	hr = m_pSwapchain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)pBackBuf.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { return hr; }

	//RTV 생성하기
	hr = m_pDevice->CreateRenderTargetView(pBackBuf.Get(), NULL, m_pRTView.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	//RT Shader Resource View 생성하기
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.Texture2D.MipLevels = 1;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	hr = m_pDevice->CreateShaderResourceView(pBackBuf.Get(), &srvd, m_pRTSRView.GetAddressOf());
	//if (FAILED(hr)) { return hr; }

	return hr;
}

HRESULT CPGCDxDevice::createDSView(UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	//깊이 스텐실 뷰를 위한 버퍼 생성하기 : Texture2D형식을 이용한다.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDSBuf;

	//버퍼 속성 세팅부
	D3D11_TEXTURE2D_DESC t2d;
	ZeroMemory(&t2d, sizeof(D3D11_TEXTURE2D_DESC));

	//깊이 스텐실 버퍼 크기 - 화면 버퍼의 깊이 스텐실 버퍼로 쓸 것이므로 동일한 사이즈로 만든다.
	t2d.Width = iWidth;
	t2d.Height = iHeight;

	//Mipmap 단계와 텍스처 배열의 텍스처 개수를 지정한다.
	t2d.MipLevels = 1;
	t2d.ArraySize = 1;

	//깊이 스텐실 버퍼의 포맷
	t2d.Format = DXGI_FORMAT_R24G8_TYPELESS;	//깊이 3바이트, 스텐실 1바이트 쉐이더 리소스로 사용하는 경우 반드시 이 옵션이어야 한다.

	//샘플링 심도
	t2d.SampleDesc.Count = 1;
	t2d.SampleDesc.Quality = 0;

	//버퍼 접근 / 사용 방식
	t2d.Usage = D3D11_USAGE_DEFAULT;

	//버퍼 타입 바인딩 플래그
	t2d.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = m_pDevice->CreateTexture2D(&t2d, NULL, pDSBuf.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	//깊이 스텐실 뷰 속성부 세팅
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	//버퍼의 포맷과 사용한 버퍼의 차원을 설정
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = m_pDevice->CreateDepthStencilView(pDSBuf.Get(), &dsvd, m_pDSView.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	//DS Shader Resource View 생성하기
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.Texture2D.MipLevels = 1;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	hr = m_pDevice->CreateShaderResourceView(pDSBuf.Get(), &srvd, m_pDSSRView.GetAddressOf());
	//if (FAILED(hr)) { return hr; }

	return hr;
}

void CPGCDxDevice::clearRenderTarget()
{
	//렌더 타겟 뷰, 깊이 스텐실 뷰 클리어
	if (!m_bNoClear)
	{
		m_pIMDContext->ClearRenderTargetView(m_pRTView.Get(), m_clearColor);
	}

	m_pIMDContext->ClearDepthStencilView(m_pDSView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		DEPTH_CLEAR_VALUE,
		STENCIL_CLEAR_VALUE);
}

void CPGCDxDevice::clearShaderResource()
{
	ID3D11ShaderResourceView* nullRes[16] = {0};

	m_pIMDContext->VSSetShaderResources(0, 16, nullRes);
	m_pIMDContext->HSSetShaderResources(0, 16, nullRes);
	m_pIMDContext->DSSetShaderResources(0, 16, nullRes);
	m_pIMDContext->GSSetShaderResources(0, 16, nullRes);
	m_pIMDContext->PSSetShaderResources(0, 16, nullRes);
	m_pIMDContext->CSSetShaderResources(0, 16, nullRes);
}

void CPGCDxDevice::bindToPipeline()
{
	//뷰포트 세팅 : Rasterizer Stage
	m_pIMDContext->RSSetViewports(1, &m_VPort);

	//렌더 타겟 뷰, 깊이 스텐실 뷰 세팅 : Output-Merger Stage
	m_pIMDContext->OMSetRenderTargets(1, m_pRTView.GetAddressOf(), m_pDSView.Get());
}

void CPGCDxDevice::presentation()
{
	//syncinterval : Vertical blanking interval(수직 귀선 간격 : 화면 출력 종료후 다음 화면의 처음 출력 위치에서 출력이 일어날 때 까지 걸리는 시간)의 횟수와 동기화
	m_pSwapchain->Present(0, 0);
}

void CPGCDxDevice::setViewport(UINT iWidth, UINT iHeight)
{
	ZeroMemory(&m_VPort, sizeof(D3D11_VIEWPORT));

	m_VPort.TopLeftX = m_VPort.TopLeftY = 0.0f;

	m_VPort.Width = (FLOAT)iWidth;
	m_VPort.Height = (FLOAT)iHeight;

	m_VPort.MinDepth = 0.0f;
	m_VPort.MaxDepth = 1.0f;
}

void CPGCDxDevice::createAdapterList()
{
	if (!m_adapterList.size())
	{
		UINT ADPidx = 0;
		UINT OUTIdx = 0;

		while (true)
		{
			DXAdapter newADPE;

			//해당 인덱스의 어댑터 인터페이스가 있는지 확인
			if (m_pDXGIFactory->EnumAdapters(ADPidx, &newADPE.m_pAdapter) == DXGI_ERROR_NOT_FOUND) { break; }
			else
			{
				//어댑터 정보 채우고 출력 장치 정보 불러오기를 위한 인덱스 조정
				newADPE.m_pAdapter->GetDesc(&newADPE.m_desc);
				OUTIdx = 0;

				while (true)
				{
					DxOutput newOUTE;

					//해당 인덱스의 출력 장치 인터페이스가 있는지 확인
					if (newADPE.m_pAdapter->EnumOutputs(OUTIdx, &newOUTE.m_pOutput) == DXGI_ERROR_NOT_FOUND) { break; }
					else
					{
						newOUTE.m_pOutput->GetDesc(&newOUTE.m_desc);
						OUTIdx++;
						newADPE.m_outputList.push_back(newOUTE);
					}
				}

				ADPidx++;
				m_adapterList.push_back(newADPE);
			}
		}
	}
}

bool CPGCDxDevice::resizeComponent(UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	//1. 바인딩된 파이프 라인 리소스 해제
	m_pIMDContext->OMSetRenderTargets(0, NULL, NULL);

	//2. 렌더 타겟 뷰 / 깊이 스텐실 뷰 리소스 해제
	m_pRTView.ReleaseAndGetAddressOf();
	m_pDSView.ReleaseAndGetAddressOf();
	m_pRTSRView.ReleaseAndGetAddressOf();
	m_pDSSRView.ReleaseAndGetAddressOf();

	//3. 스왑체인 인터페이스 속성 받아온 후 재생성
	DXGI_SWAP_CHAIN_DESC dscd;
	m_pSwapchain->GetDesc(&dscd);
	dscd.BufferDesc.Width = iWidth;
	dscd.BufferDesc.Height = iHeight;

	m_pSwapchain.ReleaseAndGetAddressOf();

	hr = m_pDXGIFactory->CreateSwapChain(m_pDevice.Get(), &dscd, m_pSwapchain.GetAddressOf());
	if (FAILED(hr)) { printCreateErr(hr); return false; }
	
	//4. 렌더 타겟 뷰 / 깊이 스텐실 뷰 재생성
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuf;

	hr = m_pSwapchain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)pBackBuf.GetAddressOf());
	if (FAILED(hr)) { printCreateErr(hr); return false; }

	hr = createRTView();
	if (FAILED(hr)) { printCreateErr(hr); return false; }

	hr = createDSView(iWidth, iHeight);
	if (FAILED(hr)) { printCreateErr(hr); return false; }

	//5. Viewport 재조정
	setViewport(iWidth, iHeight);

	//6.파이프 라인에 다시 바인딩
	bindToPipeline();

	return true;
}

ID3D11Device* CPGCDxDevice::getDevice()
{
	return m_pDevice.Get();
}

ID3D11DeviceContext* CPGCDxDevice::getImDContext()
{
	return m_pIMDContext.Get();
}

ID3D11DeviceContext* CPGCDxDevice::getDeDContext()
{
	return m_pDEDContext.Get();
}

IDXGISwapChain* CPGCDxDevice::getSwapChain()
{
	return m_pSwapchain.Get();
}

ID3D11RenderTargetView* CPGCDxDevice::getRTV()
{
	return m_pRTView.Get();
}

ID3D11ShaderResourceView* CPGCDxDevice::getRTSRV()
{
	return m_pRTSRView.Get();
}

ID3D11ShaderResourceView* CPGCDxDevice::getDSSRV()
{
	return m_pDSSRView.Get();
}

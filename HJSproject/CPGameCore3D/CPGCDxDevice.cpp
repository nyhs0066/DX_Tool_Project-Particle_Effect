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

	//����̽� ���� �÷��� : ����� ��Ƽ / �̱� ������ ���� ����
	UINT createflag = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	//DX Ư�� ���� : ����� DX������ �켱������� ������ ����Ʈ
	D3D_FEATURE_LEVEL flArray[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_12_0
	};

	//���� DX Ư�� ������ ��ȯ �޴� ���� : �Ѱ��� ����Ʈ���� � DX ������ ���� �������� ��Ÿ����.
	D3D_FEATURE_LEVEL selectedFl;

	//Adapter�� �����ϴ� ��쿡�� �ݵ�� D3D_DRIVER_TYPE_UNKNOWN�̾�� �Ѵ�.
	//Adapter�� ������ �ϴ� ��쿡�� �⺻������ ����� D3D_DRIVER_TYPE�� ������ ������ �־�� �Ѵ�.
	hr = D3D11CreateDevice(m_adapterList[0].m_pAdapter.Get(),		//��� �����
							D3D_DRIVER_TYPE_UNKNOWN,				//����̹� Ÿ��
							NULL,									//����̹� Ÿ���� ����Ʈ���� �����ΰ�� �ش� ����Ʈ������ �ڵ�
							createflag,								//����̹� ���� �÷���
							flArray,								//DX Ư�� ���� ����Ʈ
							CPGC_ARRAY_ELEM_NUM(flArray),						//����Ʈ ���� ����
							D3D11_SDK_VERSION,						//DX SDK ����
							m_pDevice.GetAddressOf(),				//��ȯ���� ����̽� �������̽� ����
							&selectedFl,							//���õ� DX Ư�� ����
							m_pIMDContext.GetAddressOf()			//��ȯ���� DC �������̽�
							);

	if (FAILED(hr)) { return hr; }

	hr = m_pDevice->CreateDeferredContext(0, m_pDEDContext.GetAddressOf());		//���� DC ����

	return hr;
}

HRESULT CPGCDxDevice::createSwapchain(HWND hwnd, UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC dscd;
	ZeroMemory(&dscd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//ȭ�� ���� ũ��
	dscd.BufferDesc.Width = iWidth;
	dscd.BufferDesc.Height = iHeight;

	//��� ��ġ �÷� ����
	dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//ȭ�� �����
	dscd.BufferDesc.RefreshRate.Denominator = 1;
	dscd.BufferDesc.RefreshRate.Numerator = 60;

	//���ø� �ɵ�
	dscd.SampleDesc.Count = 1;
	dscd.SampleDesc.Quality = 0;

	//���� ��� �뵵 �� ����
	dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	dscd.BufferCount = 2;									//�� ������ ����

	//��� ���ø����̼��� â �ڵ�� ������ ����
	dscd.OutputWindow = hwnd;
	dscd.Windowed = true;

	//���� ��ü �ɼ� �� ���� �÷���
	//����ü���� ����Ʈ ���ۿ��� ������ �� ������ 
	//DXGI_SWAP_EFFECT_DISCARD : Present���� �� ������ ������ ������. 0�� ����ü�� �� ���ۿ� ���� �� �� �ִ�.
	dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;				
	dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	hr = m_pDXGIFactory->CreateSwapChain(m_pDevice.Get(), &dscd, m_pSwapchain.GetAddressOf());
	//if (FAILED(hr)) { return hr; }

	return hr;
}

HRESULT CPGCDxDevice::createRTView()
{
	HRESULT hr = S_OK;

	//���� Ÿ�ٿ� �� ���۸� ����ü�ο��� �ҷ�����
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuf;

	//Swap Effect�� DXGI_SWAP_EFFECT_DISCARD �Ǵ� DXGI_SWAP_EFFECT_FLIP_DISCARD�� ��� 0�� ���۸� �а� �� �� �ִ�.
	hr = m_pSwapchain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)pBackBuf.ReleaseAndGetAddressOf());
	if (FAILED(hr)) { return hr; }

	//RTV �����ϱ�
	hr = m_pDevice->CreateRenderTargetView(pBackBuf.Get(), NULL, m_pRTView.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	//RT Shader Resource View �����ϱ�
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

	//���� ���ٽ� �並 ���� ���� �����ϱ� : Texture2D������ �̿��Ѵ�.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDSBuf;

	//���� �Ӽ� ���ú�
	D3D11_TEXTURE2D_DESC t2d;
	ZeroMemory(&t2d, sizeof(D3D11_TEXTURE2D_DESC));

	//���� ���ٽ� ���� ũ�� - ȭ�� ������ ���� ���ٽ� ���۷� �� ���̹Ƿ� ������ ������� �����.
	t2d.Width = iWidth;
	t2d.Height = iHeight;

	//Mipmap �ܰ�� �ؽ�ó �迭�� �ؽ�ó ������ �����Ѵ�.
	t2d.MipLevels = 1;
	t2d.ArraySize = 1;

	//���� ���ٽ� ������ ����
	t2d.Format = DXGI_FORMAT_R24G8_TYPELESS;	//���� 3����Ʈ, ���ٽ� 1����Ʈ ���̴� ���ҽ��� ����ϴ� ��� �ݵ�� �� �ɼ��̾�� �Ѵ�.

	//���ø� �ɵ�
	t2d.SampleDesc.Count = 1;
	t2d.SampleDesc.Quality = 0;

	//���� ���� / ��� ���
	t2d.Usage = D3D11_USAGE_DEFAULT;

	//���� Ÿ�� ���ε� �÷���
	t2d.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = m_pDevice->CreateTexture2D(&t2d, NULL, pDSBuf.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	//���� ���ٽ� �� �Ӽ��� ����
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	//������ ���˰� ����� ������ ������ ����
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = m_pDevice->CreateDepthStencilView(pDSBuf.Get(), &dsvd, m_pDSView.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	//DS Shader Resource View �����ϱ�
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
	//���� Ÿ�� ��, ���� ���ٽ� �� Ŭ����
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
	//����Ʈ ���� : Rasterizer Stage
	m_pIMDContext->RSSetViewports(1, &m_VPort);

	//���� Ÿ�� ��, ���� ���ٽ� �� ���� : Output-Merger Stage
	m_pIMDContext->OMSetRenderTargets(1, m_pRTView.GetAddressOf(), m_pDSView.Get());
}

void CPGCDxDevice::presentation()
{
	//syncinterval : Vertical blanking interval(���� �ͼ� ���� : ȭ�� ��� ������ ���� ȭ���� ó�� ��� ��ġ���� ����� �Ͼ �� ���� �ɸ��� �ð�)�� Ƚ���� ����ȭ
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

			//�ش� �ε����� ����� �������̽��� �ִ��� Ȯ��
			if (m_pDXGIFactory->EnumAdapters(ADPidx, &newADPE.m_pAdapter) == DXGI_ERROR_NOT_FOUND) { break; }
			else
			{
				//����� ���� ä��� ��� ��ġ ���� �ҷ����⸦ ���� �ε��� ����
				newADPE.m_pAdapter->GetDesc(&newADPE.m_desc);
				OUTIdx = 0;

				while (true)
				{
					DxOutput newOUTE;

					//�ش� �ε����� ��� ��ġ �������̽��� �ִ��� Ȯ��
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

	//1. ���ε��� ������ ���� ���ҽ� ����
	m_pIMDContext->OMSetRenderTargets(0, NULL, NULL);

	//2. ���� Ÿ�� �� / ���� ���ٽ� �� ���ҽ� ����
	m_pRTView.ReleaseAndGetAddressOf();
	m_pDSView.ReleaseAndGetAddressOf();
	m_pRTSRView.ReleaseAndGetAddressOf();
	m_pDSSRView.ReleaseAndGetAddressOf();

	//3. ����ü�� �������̽� �Ӽ� �޾ƿ� �� �����
	DXGI_SWAP_CHAIN_DESC dscd;
	m_pSwapchain->GetDesc(&dscd);
	dscd.BufferDesc.Width = iWidth;
	dscd.BufferDesc.Height = iHeight;

	m_pSwapchain.ReleaseAndGetAddressOf();

	hr = m_pDXGIFactory->CreateSwapChain(m_pDevice.Get(), &dscd, m_pSwapchain.GetAddressOf());
	if (FAILED(hr)) { printCreateErr(hr); return false; }
	
	//4. ���� Ÿ�� �� / ���� ���ٽ� �� �����
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuf;

	hr = m_pSwapchain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)pBackBuf.GetAddressOf());
	if (FAILED(hr)) { printCreateErr(hr); return false; }

	hr = createRTView();
	if (FAILED(hr)) { printCreateErr(hr); return false; }

	hr = createDSView(iWidth, iHeight);
	if (FAILED(hr)) { printCreateErr(hr); return false; }

	//5. Viewport ������
	setViewport(iWidth, iHeight);

	//6.������ ���ο� �ٽ� ���ε�
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

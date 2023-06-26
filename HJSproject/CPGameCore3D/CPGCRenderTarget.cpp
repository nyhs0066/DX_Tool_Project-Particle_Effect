#include "CPGCRenderTarget.h"

CPGCRenderTarget::CPGCRenderTarget()
{
    m_pDContext = nullptr;

    ZeroMemory(&m_viewPort, sizeof(D3D11_VIEWPORT));

    for (int i = 0; i < 3; i++) { clearColor[i] = RTV_DEFAULT_CLEAR_GRAYSCALE; }
    clearColor[3] = 1.0f;

    m_iVPnum = 1;
}

CPGCRenderTarget::~CPGCRenderTarget()
{
}

bool CPGCRenderTarget::init()
{
    return true;
}

bool CPGCRenderTarget::release()
{
    for (int i = 0; i < MAX_NUMBER_OF_RENDER_TARGET; i++)
    {
        if (m_pPrevRTV[i])
        {
            m_pPrevRTV[i]->Release();
            m_pPrevRTV[i] = nullptr;
        }
    }

    if (m_pPrevDSV) { m_pPrevDSV->Release(); }

    return true;
}

HRESULT CPGCRenderTarget::create(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext,
    UINT iWidth, UINT iHeight)
{
    //0. HRESULT ��ȯ ������ ����� �Ӽ� ����ü ����
    HRESULT hr = S_OK;

    if (!pDevice || !pDContext) { hr = E_POINTER; return hr; }
    else
    {
        m_pDevice = pDevice;
        m_pDContext = pDContext;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D>				pRTBuf;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>				pDSBuf;

    D3D11_TEXTURE2D_DESC m_RTtexDesc;
    D3D11_TEXTURE2D_DESC m_DStexDesc;
    D3D11_RENDER_TARGET_VIEW_DESC m_RTVDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC m_DSVDesc;
    //D3D11_SHADER_RESOURCE_VIEW_DESC m_RTSRVDesc;      //unused
    D3D11_SHADER_RESOURCE_VIEW_DESC m_DSSRVDesc;

    ZeroMemory(&m_RTtexDesc, sizeof(D3D11_TEXTURE2D_DESC));
    ZeroMemory(&m_DStexDesc, sizeof(D3D11_TEXTURE2D_DESC));
    ZeroMemory(&m_RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
    ZeroMemory(&m_DSVDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    //ZeroMemory(&m_RTSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));        //unused
    ZeroMemory(&m_DSSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

    //1. ���ο� ���� Ÿ�� / ���� ���ٽ� �䰡 �� ���۷� ����� �ؽ�ó ���� �����
    m_RTtexDesc.Width = iWidth;
    m_RTtexDesc.Height = iHeight;
    m_RTtexDesc.MipLevels = 1;
    m_RTtexDesc.ArraySize = 1;
    m_RTtexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //���÷��� ��ĵ
    m_RTtexDesc.SampleDesc.Count = 1;
    m_RTtexDesc.SampleDesc.Quality = 0;
    m_RTtexDesc.Usage = D3D11_USAGE_DEFAULT;
    m_RTtexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    m_RTtexDesc.CPUAccessFlags = 0;
    m_RTtexDesc.MiscFlags = 0;

    m_DStexDesc.Width = iWidth;
    m_DStexDesc.Height = iHeight;
    m_DStexDesc.MipLevels = 1;
    m_DStexDesc.ArraySize = 1;
    m_DStexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;  //���� ���ٽ� �� - ����ȭ X
    m_DStexDesc.SampleDesc.Count = 1;
    m_DStexDesc.SampleDesc.Quality = 0;
    m_DStexDesc.Usage = D3D11_USAGE_DEFAULT;
    m_DStexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    m_DStexDesc.CPUAccessFlags = 0;
    m_DStexDesc.MiscFlags = 0;

    hr = pDevice->CreateTexture2D(&m_RTtexDesc, nullptr, pRTBuf.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    hr = pDevice->CreateTexture2D(&m_DStexDesc, nullptr, pDSBuf.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    //2. ���� Ÿ�� �� / ���� ���ٽ� �� �����ϱ�
    m_RTVDesc.Format = m_RTtexDesc.Format;
    m_RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = pDevice->CreateRenderTargetView(pRTBuf.Get(), &m_RTVDesc, m_pRTView.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    m_DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    m_DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    hr = pDevice->CreateDepthStencilView(pDSBuf.Get(), &m_DSVDesc, m_pDSView.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    //3.���� Ÿ�� / ���� ���ٽ� ���̴� ���ҽ� �� �����ϱ� - �ؽ�ó�� �׷��� ������ �ش�
    hr = pDevice->CreateShaderResourceView(pRTBuf.Get(), nullptr, m_pRTSRView.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    //���� ���ٽ� ���̴� ���ҽ� ��� �����θ� nullptr�� �� �� ����. ���� �Ѱ��־�� ��
    m_DSSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    m_DSSRVDesc.Texture2D.MipLevels = 1;
    m_DSSRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; //���̴������� ���������θ� ���̸� ��Ÿ����.

    hr = pDevice->CreateShaderResourceView(pDSBuf.Get(), &m_DSSRVDesc, m_pDSSRView.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    //4. ����Ʈ ����
    m_viewPort.TopLeftX = 0.0f;
    m_viewPort.TopLeftY = 0.0f;
    m_viewPort.Width = (FLOAT)iWidth;
    m_viewPort.Height = (FLOAT)iHeight;
    m_viewPort.MinDepth = 0.0f;
    m_viewPort.MaxDepth = 1.0f;

    return hr;
}

void CPGCRenderTarget::clearViews()
{
    m_pDContext->ClearRenderTargetView(m_pRTView.Get(), clearColor);
    m_pDContext->ClearDepthStencilView(m_pDSView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        DEPTH_CLEAR_VALUE,
        STENCIL_CLEAR_VALUE);
}

bool CPGCRenderTarget::bindToPipeline()
{
    //0. ���� ������ ���õ� ���� Ÿ���� PS���� ���ҽ��� ����ϰ� �־����Ƿ�
    //�׸����� �ϴ� ����Ÿ������ ���̴��� ���� ����Ÿ�� ���ҽ��� ������� �۾��ϴ�
    //input/output �Ѵ� ����ϴ� ������ ���ϰ� �ִ�.
    //����ġ ���� �۾������ �߻���Ű�Ƿ� PS���������� ���ε��� ����Ÿ���� 
    //�ڵ����� NULL������ ����鼭 ��� �ִ� ���̴�.
    //���ҽ� ��뿡 ��������

    clearViews();

    m_pDContext->RSSetViewports(1, &m_viewPort);
    m_pDContext->OMSetRenderTargets(1, m_pRTView.GetAddressOf(), m_pDSView.Get());

    //�⺻ ������Ʈ ������
    m_pDContext->OMSetBlendState(CPGC_DXSTATE_MGR.getBState(L"BS_ALPHABLEND"), NULL, 0xffffffff);
    m_pDContext->OMSetDepthStencilState(CPGC_DXSTATE_MGR.getDSState(L"DSS_D_COMPLESS"), 0xff);

    return true;
}

void CPGCRenderTarget::getPipelineRTResource()
{
    for (int i = 0; i < MAX_NUMBER_OF_RENDER_TARGET; i++)
    {
        if (m_pPrevRTV[i]) 
        { 
            m_pPrevRTV[i]->Release(); 
            m_pPrevRTV[i] = nullptr;
        }
    }

    if (m_pPrevDSV) { m_pPrevDSV->Release(); }

    //�׻� ���� ����� ����. ����� ��������
    m_pDContext->OMGetRenderTargets(MAX_NUMBER_OF_RENDER_TARGET, m_pPrevRTV, &m_pPrevDSV);
    m_pDContext->RSGetViewports(&m_iVPnum, m_prevVPorts);
}

void CPGCRenderTarget::drawStart()
{
    getPipelineRTResource();
    bindToPipeline();
}

void CPGCRenderTarget::drawEnd()
{
    m_pDContext->RSSetViewports(1, m_prevVPorts);
    m_pDContext->OMSetRenderTargets(MAX_NUMBER_OF_RENDER_TARGET, m_pPrevRTV, m_pPrevDSV);
}

ID3D11ShaderResourceView* CPGCRenderTarget::getRTSRV()
{
    return m_pRTSRView.Get();
}

ID3D11ShaderResourceView* CPGCRenderTarget::getDSSRV()
{
    return m_pDSSRView.Get();
}

HRESULT CPGCRenderTarget::resize(UINT iWidth, UINT iHeight)
{
    if (m_pDSSRView.Get()) { m_pDSSRView.Get()->Release(); }
    if (m_pRTSRView.Get()) { m_pRTSRView.Get()->Release(); }
    if (m_pDSView.Get()) { m_pDSView.Get()->Release(); }
    if (m_pRTView.Get()) { m_pRTView.Get()->Release(); }

    return create(m_pDevice, m_pDContext, iWidth, iHeight);
}

const D3D11_VIEWPORT& CPGCRenderTarget::getViewport()
{
    return m_viewPort;
}

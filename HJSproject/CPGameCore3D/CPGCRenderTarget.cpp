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
    //0. HRESULT 반환 변수와 사용할 속성 구조체 선언
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

    //1. 새로운 렌더 타겟 / 깊이 스텐실 뷰가 백 버퍼로 사용할 텍스처 버퍼 만들기
    m_RTtexDesc.Width = iWidth;
    m_RTtexDesc.Height = iHeight;
    m_RTtexDesc.MipLevels = 1;
    m_RTtexDesc.ArraySize = 1;
    m_RTtexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //디스플레이 스캔
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
    m_DStexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;  //깊이 스텐실 용 - 정규화 X
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

    //2. 렌더 타겟 뷰 / 깊이 스텐실 뷰 생성하기
    m_RTVDesc.Format = m_RTtexDesc.Format;
    m_RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = pDevice->CreateRenderTargetView(pRTBuf.Get(), &m_RTVDesc, m_pRTView.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    m_DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    m_DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    hr = pDevice->CreateDepthStencilView(pDSBuf.Get(), &m_DSVDesc, m_pDSView.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    //3.렌더 타겟 / 깊이 스텐실 쉐이더 리소스 뷰 생성하기 - 텍스처를 그려낼 도구에 해당
    hr = pDevice->CreateShaderResourceView(pRTBuf.Get(), nullptr, m_pRTSRView.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    //깊이 스텐실 쉐이더 리소스 뷰는 설정부를 nullptr로 둘 수 없음. 값을 넘겨주어야 함
    m_DSSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    m_DSSRVDesc.Texture2D.MipLevels = 1;
    m_DSSRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; //쉐이더에서는 빨간색으로만 깊이를 나타낸다.

    hr = pDevice->CreateShaderResourceView(pDSBuf.Get(), &m_DSSRVDesc, m_pDSSRView.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    //4. 뷰포트 세팅
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
    //0. 이전 값으로 세팅된 렌더 타겟을 PS에서 리소스로 사용하고 있었으므로
    //그리고자 하는 렌더타겟으로 쉐이더가 같은 렌더타겟 리소스를 대상으로 작업하는
    //input/output 둘다 사용하는 오류를 범하고 있다.
    //예기치 않은 작업결과를 발생시키므로 PS스테이지에 바인딩된 렌더타겟을 
    //자동으로 NULL값으로 만들면서 경고를 주는 것이다.
    //리소스 사용에 유의하자

    clearViews();

    m_pDContext->RSSetViewports(1, &m_viewPort);
    m_pDContext->OMSetRenderTargets(1, m_pRTView.GetAddressOf(), m_pDSView.Get());

    //기본 스테이트 설정부
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

    //항상 새로 만들어 낸다. 릴리즈에 유의하자
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

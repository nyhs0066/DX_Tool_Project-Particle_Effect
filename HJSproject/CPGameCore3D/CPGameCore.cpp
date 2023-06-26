#include "CPGameCore.h"

CPGameCore::CPGameCore(const WCHAR* wszWndName, const WCHAR* wszWcName) : CPGCWindow(wszWndName, wszWcName)
{
	m_iRenderTargetNum = 0;

	m_iPrevWndWidth = 0;
	m_iPrevWndHeight = 0;
}

CPGameCore::~CPGameCore()
{
}

bool CPGameCore::coreInit()
{
	HRESULT hr = S_OK;

	//DX 렌더링 모듈 생성
	hr = CPGC_DXMODULE.create(CPGCWindow::m_hwnd, CPGCWindow::m_wndWidth, CPGCWindow::m_wndHeight);
	if (FAILED(hr)) { printCreateErr(hr); return false; }

	m_pDevice = CPGC_DXMODULE.getDevice();
	m_pImDContext = CPGC_DXMODULE.getImDContext();

	//주 타이머 시작
	CPGC_MAINTIMER.start();

	//주 입력 클래스 초기화
	CPGC_MAININPUT.init();

	//DX State 매니저 초기화
	CPGC_DXSTATE_MGR.init(m_pDevice, m_pImDContext);

	//ImGui 모듈 초기화
	CPGC_IMGUI_MODULE.init(CPGCWindow::m_hwnd, m_pDevice, m_pImDContext);

	//텍스트 렌더 모듈 생성
	CPGC_TEXTRENDERER.create(CPGC_DXMODULE.getSwapChain());
	if (FAILED(hr)) { printCreateErr(hr); return false; }

	//Shader 매니저 초기화
	CPGC_SHADER_MGR.init(m_pDevice, m_pImDContext);

	//VLayout 매니저 초기화
	CPGC_VLAYOUT_MGR.init(m_pDevice);

	//Texture 매니저 초기화
	CPGC_TEXTURE_MGR.init(m_pDevice, m_pImDContext);

	//스프라이트 매니저 초기화
	CPGC_SPRITE_MGR.init();

	//FMOD Sound 매니저 초기화
	CPGC_SOUND_MGR.init();

	//Mesh 매니저 초기화
	CPGC_MESH_MGR.init(m_pDevice, m_pImDContext);

	//FBX 매니저 초기화
	CPGC_FBX_MGR.init(m_pDevice, m_pImDContext);

	//Effect 리소스 파일 매니저 초기화
	//CPGC_EFFECT_MGR.init();

	//DX 코어 생성 플래그 세트
	CPGCWindow::bDxCreateFlag = true;

	//MRT 생성
	hr = createMRTs(CPGCWindow::m_wndWidth, CPGCWindow::m_wndHeight);

	//디퍼드 렌더를 위한 쿼드 생성
	m_quad.setDevice(m_pDevice, m_pImDContext);
	hr = m_quad.create(L"MainQuad", L"CPRS_SM_RECT");
	m_quad.m_shaderGroup.pVS = CPGC_SHADER_MGR.getVS(L"VS_DefaultMRTQuad");
	m_quad.m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_DefaultMRTQuad");

	//0 : 선형 래핑 / 1 : 선형 클램프
	m_quad.addSampler(CPGC_VNAME2WSTR(SS_LINEAR_WRAP));
	m_quad.addSampler(CPGC_VNAME2WSTR(SS_LINEAR_CLAMP));

	return init();
}

bool CPGameCore::coreUpdate()
{
	corePreUpdate();

	//TO DO
	preUpdate();
	update();
	postUpdate();

	//TO DO END
	corePostUpdate();

	return true;
}

bool CPGameCore::corePreUpdate()
{
	//업데이트 작업을 수행하기 위해서 현재 프레임 시간을 얻기 위한 주 타이머 갱신
	CPGC_MAINTIMER.updateTimer();

	//입력 상태 업데이트
	CPGC_MAININPUT.update();

	////사운드 시스템 갱신
	CPGC_SOUND_MGR.update();

	//IMGUI 모듈 갱신
	CPGC_IMGUI_MODULE.update();
	return true;
}

bool CPGameCore::corePostUpdate()
{
	return true;
}

bool CPGameCore::coreRender()
{
	corePreRender();

	//TO DO
	preRender();
	render();
	postRender();
	//TO DO END

	corePostRender();

	return true;
}

bool CPGameCore::corePreRender()
{
	//MRT 클리어 및 바인딩
	clearMRTs();
	bindMRTs();

	//기본 스테이트 설정부
	m_pImDContext->OMSetBlendState(CPGC_DXSTATE_MGR.getBState(L"BS_ALPHABLEND"), NULL, 0xffffffff);
	m_pImDContext->OMSetDepthStencilState(CPGC_DXSTATE_MGR.getDSState(L"DSS_D_COMPLESS"), 0xff);

	return true;
}

bool CPGameCore::corePostRender()
{
	//GUI 렌더
	CPGC_IMGUI_MODULE.render();

	////////////////////////////////////////////////
	// 백 버퍼 출력 준비
	////////////////////////////////////////////////

	//렌더 타겟 클리어
	CPGC_DXMODULE.clearRenderTarget();
	CPGC_DXMODULE.clearShaderResource();

	//기본 렌더링을 위한 작업 파이프라인에 바인딩
	CPGC_DXMODULE.bindToPipeline();

	//기본 스테이트 설정부
	m_pImDContext->OMSetBlendState(CPGC_DXSTATE_MGR.getBState(L"BS_ALPHABLEND"), NULL, 0xffffffff);
	m_pImDContext->OMSetDepthStencilState(CPGC_DXSTATE_MGR.getDSState(L"DSS_D_COMPLESS"), 0);

	//MRT텍스처 바인딩
	m_pImDContext->PSSetShaderResources(0, m_iSRVNum, m_pSRVArray.data());

	//쿼드 렌더
	m_quad.render();

	//출력 준비된 버퍼의 내용과 프론트 버퍼를 Swap
	CPGC_DXMODULE.presentation();

	//휠 상태 정보를 초기화 한다.
	ZeroMemory(&CPGCWindow::m_wheelState, sizeof(CPGC_MOUSE_WHEEL_STATE));

	//shader리소스 정리
	CPGC_DXMODULE.clearShaderResource();

	return true;
}

bool CPGameCore::coreRelease()
{
	//주 타이머 종료
	CPGC_MAINTIMER.end();

	//ImGui 모듈 초기화
	CPGC_IMGUI_MODULE.release();

	//텍스트 렌더러 정리
	CPGC_TEXTRENDERER.release();

	//DX 렌더 모듈 정리
	CPGC_DXMODULE.release();

	//주 입력 클래스 정리
	CPGC_MAININPUT.release();

	//매니저 리소스 정리
	CPGC_DXSTATE_MGR.release();

	CPGC_SPRITE_MGR.release();
	CPGC_SHADER_MGR.release();
	CPGC_VLAYOUT_MGR.release();
	CPGC_TEXTURE_MGR.release();
	CPGC_SOUND_MGR.release();
	CPGC_MESH_MGR.release();
	CPGC_FBX_MGR.release();
	CPGC_EFFECT_MGR.release();

	release();

	releaseMRTs();
	m_quad.release();

	return true;
}

void CPGameCore::clearMRTs()
{
	for (int i = 0; i < m_iRenderTargetNum; i++)
	{
		if (m_pMRT[i])
			m_pMRT[i]->clearViews();
	}
}

void CPGameCore::bindMRTs()
{
	m_pImDContext->RSSetViewports(1, &m_mainViewPort);
	m_pImDContext->OMSetRenderTargets(m_iRenderTargetNum, m_pRTVArray.data(), m_pMRT[0]->m_pDSView.Get());
}

HRESULT CPGameCore::createMRTs(UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	m_iRenderTargetNum = MAX_NUMBER_OF_RENDER_TARGET;
	m_iSRVNum = m_iRenderTargetNum + 1;

	m_pRTVArray.clear();
	m_pSRVArray.clear();

	m_pMRT.resize(m_iRenderTargetNum);

	for (int i = 0; i < m_iRenderTargetNum; i++)
	{
		m_pMRT[i] = new CPGCRenderTarget;
		hr = m_pMRT[i]->create(m_pDevice, m_pImDContext, iWidth, iHeight);

		if (FAILED(hr))
		{
			m_pRTVArray.clear();
			releaseMRTs();
			return hr;
		}

		m_pRTVArray.push_back(m_pMRT[i]->m_pRTView.Get());
		m_pSRVArray.push_back(m_pMRT[i]->m_pRTSRView.Get());
	}

	m_pSRVArray.push_back(m_pMRT[0]->getDSSRV());

	ZeroMemory(&m_mainViewPort, sizeof(D3D11_VIEWPORT));
	m_mainViewPort.Width = iWidth;
	m_mainViewPort.Height = iHeight;
	m_mainViewPort.MaxDepth = 1.0f;

	return hr;
}

void CPGameCore::releaseMRTs()
{
	for (int i = 0; i < m_iRenderTargetNum; i++)
	{
		if (m_pMRT[i])
		{
			m_pMRT[i]->release();
			delete m_pMRT[i];
			m_pMRT[i] = nullptr;
		}
	}
}

bool CPGameCore::resizeComponent(UINT iWidth, UINT iHeight)
{
	HRESULT hr = createMRTs(iWidth, iHeight);

	ZeroMemory(&m_mainViewPort, sizeof(D3D11_VIEWPORT));
	m_mainViewPort.Width = iWidth;
	m_mainViewPort.Height = iHeight;
	m_mainViewPort.MaxDepth = 1.0f;

	//의존성의 루트부터 새로 조정한다.
	CPGC_DXMODULE.resizeComponent(iWidth, iHeight);
	CPGC_TEXTRENDERER.resizeComponent(iWidth, iHeight, CPGC_DXMODULE.getSwapChain());
	
	CPGCWindow::bDxCreateFlag = true;

	return true;
}

void CPGameCore::toggleFullscreen()
{
	if (!CPGC_DXMODULE.m_bFullscreen)
	{
		m_iPrevWndWidth = CPGCWindow::m_wndWidth;
		m_iPrevWndHeight = CPGCWindow::m_wndHeight;

		/*DXGI_MODE_DESC dmd;
		dmd.Width = 2560;
		dmd.Height = 1440;
		dmd.RefreshRate.Denominator = 1;
		dmd.RefreshRate.Numerator = 60;
		dmd.Format = DXGI_FORMAT_R8G8B8A8_UINT;
		dmd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		dmd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		CPGC_DXMODULE.getSwapChain()->ResizeTarget(&dmd);*/

		CPGC_DXMODULE.getSwapChain()->SetFullscreenState(TRUE, NULL);
	}
	else
	{
		/*DXGI_MODE_DESC dmd;
		dmd.Width = m_iPrevWndWidth;
		dmd.Height = m_iPrevWndHeight;
		dmd.RefreshRate.Denominator = 1;
		dmd.RefreshRate.Numerator = 60;
		dmd.Format = DXGI_FORMAT_R8G8B8A8_UINT;
		dmd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		dmd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		CPGC_DXMODULE.getSwapChain()->ResizeTarget(&dmd);*/

		CPGC_DXMODULE.getSwapChain()->SetFullscreenState(FALSE, NULL);
	}

	CPGC_DXMODULE.m_bFullscreen = !CPGC_DXMODULE.m_bFullscreen;
}

bool CPGameCore::init()
{
	//TO DO

	return true;
}

bool CPGameCore::update()
{
	preUpdate();

	//TO DO

	update();

	//TO DO END

	postUpdate();

	return true;
}

bool CPGameCore::preUpdate()
{
	return true;
}

bool CPGameCore::postUpdate()
{
	return true;
}

bool CPGameCore::render()
{
	preRender();

	//TO DO

	render();

	//TO DO END

	postRender();

	return true;
}

bool CPGameCore::preRender()
{
	return true;
}

bool CPGameCore::postRender()
{
	return true;
}

bool CPGameCore::release()
{
	//TO DO

	return true;
}

int CPGameCore::run()
{
	int iRet = 1;

	if (!coreInit()) { iRet = -1; return iRet; }

	while (iRet == 1)
	{
		if (msgProcess())
		{
			if (!coreUpdate()) { iRet = -1; }
			if (!coreRender()) { iRet = -1; }
		}
		else { iRet = 0; }
	}

	if (!coreRelease()) { iRet = -1; }

	return iRet;
}

LRESULT CPGameCore::msgHandler(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(m_hwnd, uMsg, wParam, lParam))
	{
		return true;
	}

	return CPGCWindow::msgHandler(uMsg, wParam, lParam);
}

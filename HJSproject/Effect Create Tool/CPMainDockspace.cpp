#include "CPMainDockspace.h"

CPMainDockspace::CPMainDockspace()
{
    //창 플래그
	m_bMainShow                 = false;
	m_bFullscreen               = false;
    m_bOpenDemoWindow           = false;
    m_bResizeFlag               = false;
    m_bOpenDXRTWindow           = false;
    m_bOpenEffectSystemWindow   = false;
    m_bOpenMapWindow      = false;

    //전역 DX State 플래그
    m_bEnableWireFrame          = false;

    //변수 포인터
    m_pDevice                   = nullptr;
    m_pDContext                 = nullptr;
    m_pMainRT                   = nullptr;

    m_pMainCam                  = nullptr;

    //워크 스페이스
    m_pEffectWSpace             = nullptr;
    m_pMapWSpace                = nullptr;

    m_iShowEffectPopup = 0;
    m_iShowMapPopup = 0;

    m_isMapPicked = false;
}

CPMainDockspace::~CPMainDockspace()
{

}

bool CPMainDockspace::init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
    //창 플래그
    m_bMainShow                 = true;
    m_bFullscreen               = true;
    m_bOpenDXRTWindow           = true;
    
    //변수 포인터
    m_pDevice                   = pDevice;
    m_pDContext                 = pDContext;

    //렌더 타겟 생성
    m_pMainRT = new CPGCRenderTarget;
    m_pMainCam = new CPGCDebugCamera;

    m_pMainRT->create(CPGC_DXMODULE.getDevice(), CPGC_DXMODULE.getImDContext(),
        800, 600);

    //카메라 세팅
    m_pMainCam->setScreenWH(CPGCWindow::m_wndWidth, CPGCWindow::m_wndHeight);
    m_pMainCam->init();

    m_pMainCam->setViewMat({ 0.0f, 10.0f, -25.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });

    //위크 스페이스 구성
    EFFECT_SPACE.init(pDevice, pDContext, m_pMainRT, m_pMainCam);
    MAP_SPACE.init(pDevice, pDContext, m_pMainRT, m_pMainCam);

    m_pEffectWSpace = &EFFECT_SPACE;
    m_pMapWSpace = &MAP_SPACE;
	return true;
}

bool CPMainDockspace::preUpdate()
{
    if (m_bEnableWireFrame)
    {
        m_pDContext->RSSetState(CPGC_DXSTATE_MGR.getRState(L"RS_WIREFRAME"));
    }
    else
    {
        m_pDContext->RSSetState(CPGC_DXSTATE_MGR.getRState(L"RS_SOLID"));
    }

    if (m_bResizeFlag) { resizeComponent(m_mainRTVPort.Width, m_mainRTVPort.Height); }

    m_pMainCam->update();

    return true;
}

bool CPMainDockspace::update()
{
    if (m_pMapWSpace)
    {
        m_pMapWSpace->update();
    }

    if (m_pEffectWSpace)
    {
        m_pEffectWSpace->update();
    }

    if (CPGC_MAININPUT.getKeyState(VK_LBUTTON) == KEY_DOWN)
    {
        m_picker.setMatrix(nullptr, m_pMainCam->getViewMatPtr(), m_pMainCam->getProjMatPtr());

        bool m_bHit = false;

        if (m_pMapWSpace && m_pMapWSpace->m_pQuadTree)
        {
            if (getIntersectionInMap()) { m_isMapPicked = true; }
            else { m_isMapPicked = false; }

            for (auto it : m_pMapWSpace->m_pQuadTree->m_drawLeafNodeList)
            {
                if (CPGCCollision::rayToOBB(m_picker.m_ray, it->m_boundingBox) == CPGC_CO_HIT)
                {
                    m_bHit = true;
                    m_testNodeIdx = it->m_NodeIdx;
                    break;
                }
            }
        }

        if (!m_bHit) { m_testNodeIdx = -1; }
    }

	return true;
}

bool CPMainDockspace::postUpdate()
{
    if (m_pMapWSpace)
    {
        m_pMapWSpace->postUpdate();
    }

    if (m_pEffectWSpace)
    {
        m_pEffectWSpace->postUpdate();
    }

    return true;
}

bool CPMainDockspace::preRender()
{
    m_pMainRT->drawStart();

    if (m_pMapWSpace)
    {
        m_pMapWSpace->preRender();
    }

    return true;
}

bool CPMainDockspace::render()
{
    if (m_pMapWSpace)
    {
        m_pMapWSpace->render();
    }

    if (m_pEffectWSpace)
    {
        m_pEffectWSpace->render();
    }

	return true;
}

bool CPMainDockspace::postRender()
{
    m_pMainRT->drawEnd();

    if (m_bMainShow)
    {
        showMainDockSpace();

        showStatusWindow();

        if (m_bOpenEffectSystemWindow)
        {
            showEffectCreateWindow();
        }

        if (m_bOpenMapWindow)
        {
            showMapCreateWindow();
        }

        if (m_bOpenDemoWindow)
        {
            ImGui::ShowDemoWindow(&m_bOpenDemoWindow);
        }

        if (m_bOpenDXRTWindow)
        {
            showDXRTWindow();
        }
    }

    //기본 설정 값으로 변경
    if (m_bEnableWireFrame)
    {
        m_pDContext->RSSetState(CPGC_DXSTATE_MGR.getRState(L"RS_WIREFRAME"));
    }
    else
    {
        m_pDContext->RSSetState(CPGC_DXSTATE_MGR.getRState(L"RS_SOLID"));
    }

    return true;
}

bool CPMainDockspace::release()
{
    if (m_pEffectWSpace)
    {
        m_pEffectWSpace->release();
    }

    if (m_pMapWSpace)
    {
        m_pMapWSpace->release();
    }

    if (m_pMainRT) 
    { 
        m_pMainRT->release(); 
        delete m_pMainRT; 
        m_pMainRT = nullptr;
    }

    if (m_pMainCam)
    {
        m_pMainCam->release();
        delete m_pMainCam;
        m_pMainCam = nullptr;
    }

	return true;
}

bool CPMainDockspace::resizeComponent(UINT iWidth, UINT iHeight)
{
    m_pMainRT->resize(iWidth, iWidth);
    m_pMainCam->resizeComponent(iWidth, iHeight);
    m_picker.setTargetWindow(m_mainRTVPort);

	return true;
}

bool CPMainDockspace::setMainCam(CPGCCamera* pMainCam)
{
    return m_pMainCam = pMainCam;
}

bool CPMainDockspace::getResizeFlag()
{
    return m_bResizeFlag;
}

void CPMainDockspace::showMainDockSpace()
{
    ImGui::PushID("MAIN1");

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(u8"파일"))
        {
            //TO DO
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(u8"편집"))
        {
            //TO DO

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(u8"보기"))
        {
            //TO DO
            if (ImGui::MenuItem(u8"DX 렌더 뷰 포트", "", (m_bOpenDXRTWindow != 0)))
                m_bOpenDXRTWindow = !m_bOpenDXRTWindow;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(u8"테스트"))
        {
            //TO DO
            if (ImGui::MenuItem("Demo Window", "", (m_bOpenDemoWindow != 0)))
                m_bOpenDemoWindow = !m_bOpenDemoWindow;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(u8"이펙트 도구"))
        {
            if (ImGui::MenuItem(u8"새로운 파티클 시스템 생성", ""))
                m_iShowEffectPopup = 1;

            if (ImGui::MenuItem(u8"불러오기", ""))
                m_iShowEffectPopup = 2;

            if (ImGui::MenuItem(u8"이전 버전 파일 불러오기", ""))
                m_iShowEffectPopup = 3;

            if (ImGui::MenuItem(u8"저장하기", ""))
                m_iShowEffectPopup = 4;

            if (ImGui::MenuItem(u8"이전 버전 파일 변환하기", ""))
                m_iShowEffectPopup = 5;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(u8"지형 생성 도구"))
        {
            if (ImGui::MenuItem(u8"새로운 지형 생성", ""))
                m_iShowMapPopup = 1;

            if (ImGui::MenuItem(u8"불러오기", ""))
                m_iShowMapPopup = 2;

            if (ImGui::MenuItem(u8"저장하기", ""))
                m_iShowMapPopup = 3;

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    switch (m_iShowEffectPopup)
    {
        case 1: EFFECT_SPACE.createNewParticleSystemPopup(m_iShowEffectPopup, m_bOpenEffectSystemWindow); break;
        case 2: EFFECT_SPACE.loadParticleSystemPopup(m_iShowEffectPopup, m_bOpenEffectSystemWindow, LOPT_CURRENT_VER); break;
        case 3: EFFECT_SPACE.loadParticleSystemPopup(m_iShowEffectPopup, m_bOpenEffectSystemWindow, LOPT_PREV_VER); break;
        case 4: EFFECT_SPACE.saveParticleSystemPopup(m_iShowEffectPopup); break;
        case 5: EFFECT_SPACE.convertDirectory(DEFAULT_OLD_EFFECT_DIRECTORY_WSZPATH, DEFAULT_NEW_EFFECT_DIRECTORY_WSZPATH); m_iShowEffectPopup = 0; break;
    }

    switch (m_iShowMapPopup)
    {
        case 1: MAP_SPACE.createNewMapPopup(m_iShowMapPopup, m_bOpenMapWindow); break;
        case 2: MAP_SPACE.loadMapPopup(m_iShowMapPopup, m_bOpenMapWindow); break;
        case 3: MAP_SPACE.saveMapPopup(m_iShowMapPopup); break;
    }

    ImGui::PopID();
}

void CPMainDockspace::showStatusWindow()
{
    ImGui::PushID("MAIN2");

    ImGui::Begin(u8"상태 창");
    ImGui::Text(u8"평균 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text(u8"실행 시간 : %.3f초", CPGC_MAINTIMER.getRunningTimeF());
    ImGui::Text(u8"뷰 포트 시작 위치 | X : %d, Y : %d", (int)m_mainRTVPort.TopLeftX, (int)m_mainRTVPort.TopLeftY - m_RTWindowOffsetY);
    ImGui::Text(u8"DX 뷰포트 마우스 정보 | X : %d, Y : %d", CPGC_MAININPUT.getMousePos().x - (int)m_mainRTVPort.TopLeftX, CPGC_MAININPUT.getMousePos().y - (int)m_mainRTVPort.TopLeftY);
    ImGui::Text(u8"피킹 레이 정보 | Origin : (%.3f, %.3f, %.3f)\n Direction : (%.3f, %.3f, %.3f)",
        m_picker.m_ray.stPos.x, m_picker.m_ray.stPos.y, m_picker.m_ray.stPos.z,
        m_picker.m_ray.direction.x, m_picker.m_ray.direction.y, m_picker.m_ray.direction.z);

    if (m_isMapPicked)
    {
        ImGui::Text(u8"맵 피킹 지점 정보 | X : %.3f, Y : %.3f, Z : %.3f",
            m_picker.m_vISPoint.x,
            m_picker.m_vISPoint.y,
            m_picker.m_vISPoint.z);
    }
    else
    {
        ImGui::Text(u8"맵 피킹 지점 정보 | X : NDEF, Y : NDEF, Z : NDEF");
    }

    ImGui::Text(u8"현재 선택된 리프노드 번호 : %d", m_testNodeIdx);

    if (m_bOpenMapWindow)
    {
        ImGui::Text(u8"렌더된 리프노드 수 : %d개", MAP_SPACE.getRenderedLeafNodeCount());
    }

    ImGui::SeparatorText(u8"전역 설정");

    ImGui::Checkbox(u8"와이어 프레임 렌더", &m_bEnableWireFrame);

    ImGui::End();

    ImGui::PopID();
}

void CPMainDockspace::showDXRTWindow()
{
    ImGui::PushID("MAIN3");

    if (!CPGCWindow::m_bCollapseFlag)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);


        ImGui::Begin(u8"DX 렌더 뷰 포트", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::PopStyleVar(3);


        ImVec2 sizeMin = ImGui::GetWindowContentRegionMin();
        ImVec2 sizeMax = ImGui::GetWindowContentRegionMax();

        m_RTWindowOffsetY = sizeMin.y;

        m_mainRTVPort.TopLeftX = ImGui::GetWindowPos().x;
        m_mainRTVPort.TopLeftY = ImGui::GetWindowPos().y + m_RTWindowOffsetY;
        m_mainRTVPort.Width = sizeMax.x - sizeMin.x;
        m_mainRTVPort.Height = sizeMax.y - sizeMin.y;

        ImVec2 m_RTSize = ImVec2(m_mainRTVPort.Width, m_mainRTVPort.Height);
        if (m_pMainRT && !m_bResizeFlag)
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddCallback([](const ImDrawList*, const ImDrawCmd*)
                {
                    CPGC_DXMODULE.getImDContext()->OMSetBlendState(
                        CPGC_DXSTATE_MGR.getBState(L"BS_DEFAULT"), NULL, -1);
                }, nullptr);

            
            ImGui::Image((void*)m_pMainRT->getRTSRV(), m_RTSize);

            draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
        }

        ImGui::End();

        if (m_prevMainRTVPort.Width != m_mainRTVPort.Width ||
            m_prevMainRTVPort.Height != m_mainRTVPort.Height)
        {
            ImGui::SetNextWindowSize(m_RTSize);
            m_bResizeFlag = true;
        }
        else { m_bResizeFlag = false; }

        if (m_prevMainRTVPort.TopLeftX != m_mainRTVPort.TopLeftX ||
            m_prevMainRTVPort.TopLeftY != m_mainRTVPort.TopLeftY)
        {
            m_picker.setTargetWindow(m_mainRTVPort);
        }

        m_prevMainRTVPort = m_mainRTVPort;
    }

    ImGui::PopID();
}


void CPMainDockspace::showEffectCreateWindow()
{
    if (m_pEffectWSpace)
    {
        m_pEffectWSpace->openWorkspaceWindow();
    }
}

void CPMainDockspace::showMapCreateWindow()
{
    if (m_pMapWSpace)
    {
        m_pMapWSpace->openWorkspaceWindow();
    }
}

bool CPMainDockspace::getIntersectionInMap()
{
    for (auto it : m_pMapWSpace->m_pQuadTree->m_drawLeafNodeList)
    {
        UINT nFace = it->m_idxList.size() / 3;
        UINT idx = 0;
        for (int face = 0; face < nFace; face++)
        {
            UINT i0 = it->m_idxList[idx + 0];
            UINT i1 = it->m_idxList[idx + 1];
            UINT i2 = it->m_idxList[idx + 2];

            CPM_Vector3 v0 = m_pMapWSpace->m_pMap->m_vertices[i0].p;
            CPM_Vector3 v1 = m_pMapWSpace->m_pMap->m_vertices[i1].p;
            CPM_Vector3 v2 = m_pMapWSpace->m_pMap->m_vertices[i2].p;

            if (m_picker.getTriangleIntersection(v0, v1, v2))
            {
                return true;
            }

            idx += 3;
        }
    }

    return false;
}
#include "CPMapWorkspace.h"

CPMapWorkspace::CPMapWorkspace()
{
    m_pDevice = nullptr;
    m_pDContext = nullptr;

    m_pMainCam = nullptr;
    m_pRT = nullptr;

    m_iMCRow = CPIMGUI_MIN_NUMBER_OF_CELL;
    m_iMCCol = CPIMGUI_MIN_NUMBER_OF_CELL;
    m_iMCDepthLimit = 3;
    m_fMCCellsize = 1.0f;

    m_bUseHeightMap = false;
}

CPMapWorkspace::~CPMapWorkspace()
{
}

bool CPMapWorkspace::init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext, CPGCRenderTarget* pMainRT, CPGCCamera* pMainCam)
{
    //변수 포인터
    m_pDevice = pDevice;
    m_pDContext = pDContext;
    m_pRT = pMainRT;

    setMainCam(pMainCam);

    m_pMapBaseTexSelector = new CPIMGUITexSelector;
    m_pHeightMapTexSelector = new CPIMGUITexSelector;

    m_pMapBaseTexSelector->init("../../data/texture/map/");
    m_pHeightMapTexSelector->init("../../data/texture/map/");

    return true;
}

bool CPMapWorkspace::preUpdate()
{
    return true;
}

bool CPMapWorkspace::update()
{
    if (m_pQuadTree)
    {
        m_pQuadTree->update();
    }
    return true;
}

bool CPMapWorkspace::postUpdate()
{
    return true;
}

bool CPMapWorkspace::preRender()
{
    if (m_pQuadTree)
    {
        m_pQuadTree->preRender();
    }

    return true;
}

bool CPMapWorkspace::render()
{
    if (m_pQuadTree)
    {
        m_pQuadTree->render();
    }
    return true;
}

bool CPMapWorkspace::postRender()
{
    return true;
}

bool CPMapWorkspace::release()
{
    if (m_pMap)
    {
        m_pMap->release();
        delete m_pMap;
        m_pMap = nullptr;
    }

    if (m_pQuadTree)
    {
        m_pQuadTree->clear();
        delete m_pQuadTree;
        m_pQuadTree = nullptr;
    }

    if (m_pMapBaseTexSelector)
    {
        delete m_pMapBaseTexSelector;
    }

    if (m_pHeightMapTexSelector)
    {
        delete m_pHeightMapTexSelector;
    }

    return true;
}

void CPMapWorkspace::setMainCam(CPGCCamera* pCam)
{
    m_pMainCam = pCam;
}

void CPMapWorkspace::openWorkspaceWindow()
{
    ImGui::PushID("MAP1");

    if (m_pMap)
    {
        showCreateMapWindow();
    }

    ImGui::PopID();
}

void CPMapWorkspace::showCreateMapWindow()
{
    ImGui::PushID("Map2");

    std::string temp = m_pMap->getSzName();
    temp += "##Map2";

    ImGui::Begin(temp.c_str());

    ImGui::Separator();
    ImGui::SeparatorText(u8"지형 생성");
    ImGui::Separator();

    

    ImGui::Text(u8"행 개수 / 열 개수");
    ImGui::SetNextItemWidth(80.0f);
    ImGui::DragInt(u8"행 개수##Row", &m_iMCRow, 1.0f, CPIMGUI_MIN_NUMBER_OF_CELL, CPIMGUI_MAX_NUMBER_OF_CELL); ImGui::SameLine();
    ImGui::SetNextItemWidth(80.0f);
    ImGui::DragInt(u8"열 개수##Col", &m_iMCCol, 1.0f, CPIMGUI_MIN_NUMBER_OF_CELL, CPIMGUI_MAX_NUMBER_OF_CELL);

    ImGui::SetNextItemWidth(80.0f);
    ImGui::DragFloat(u8"셀 크기##CellSize", &m_fMCCellsize, 0.5f, CPIMGUI_MIN_CELL_SIZE, CPIMGUI_MAX_CELL_SIZE, "%.1f");

    ImGui::SetNextItemWidth(80.0f);
    ImGui::DragInt(u8"공간분할 깊이 제한##Depth", &m_iMCDepthLimit, 1.0f, CPIMGUI_MIN_PARTITION_DEPTH, CPIMGUI_MAX_PARTITION_DEPTH);

    m_pMapBaseTexSelector->showUIControl("MapTexSelector0");

    ImGui::Checkbox(u8"높이맵 사용", &m_bUseHeightMap);

    if (m_bUseHeightMap)
    {
        m_pHeightMapTexSelector->showUIControl("MapTexSelector1");
    }

    ImGui::Separator();
    ImGui::Separator();

    if (ImGui::Button(u8"생성", ImVec2(40, 40)))
    {
        HRESULT hr = S_OK;

        if (m_pMap) { m_pMap->release(); }
        if (m_pQuadTree) { m_pQuadTree->clear(); }

        std::wstring wszTexName = atl_M2W(m_pMapBaseTexSelector->m_szSelectedTexName);

        if (m_bUseHeightMap && m_pHeightMapTexSelector->m_pSelectedTex)
        {
            m_pHeightMapTexSelector->m_wszSelectedTexName = L"HMAP_" + m_pHeightMapTexSelector->m_wszSelectedTexName;

            hr = m_pMap->create(m_pDevice,
                m_pDContext,
                wszTexName,
                { 0.0f, 0.0f, 0.0f },
                m_iMCCol,
                m_iMCRow,
                m_fMCCellsize,
                m_pHeightMapTexSelector->m_wszSelectedTexName,
                m_pHeightMapTexSelector->m_wszSelectedTexPath);
        }
        else
        {
            hr = m_pMap->create(m_pDevice,
                m_pDContext,
                wszTexName,
                { 0.0f, 0.0f, 0.0f },
                m_iMCCol,
                m_iMCRow,
                m_fMCCellsize);
        }

        if (SUCCEEDED(hr))
        {
            m_pQuadTree = new CPGCQuadTree;
            m_pQuadTree->create(m_pDevice, m_pDContext, m_pMap, m_iMCDepthLimit);
            m_pQuadTree->m_pCam = m_pMainCam;
        }
    }

    ImGui::End();

    ImGui::PopID();
}

void CPMapWorkspace::createNewMapPopup(int& popOpt, bool& bOpenWorkspace)
{
    ImGui::PushID("MAP_CREATE");

    ImGui::OpenPopup(u8"새로 생성");

    //항상 가운데에서 생성하게 한다.
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(u8"새로 생성", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SeparatorText(u8"맵 이름");
        static std::string tempName;
        CPImGui::InputString("##Name0", &tempName);

        if (ImGui::Button(u8"생성") && tempName.size())
        {
            if (m_pMap)
            {
                m_pQuadTree->clear();
                m_pMap->release();

                delete m_pMap;
                delete m_pQuadTree;
            }

            m_pMap = new CPGCMap;
            m_pMap->setName(tempName);

            tempName.clear();
            popOpt = 0;
            bOpenWorkspace = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button(u8"닫기"))
        {
            popOpt = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void CPMapWorkspace::loadMapPopup(int& popOpt, bool& bOpenWorkspace)
{
    ImGui::PushID("MAP_LOAD");

    ImGui::OpenPopup(u8"파일 불러오기");

    //항상 가운데에서 생성하게 한다.
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(u8"파일 불러오기", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SeparatorText(u8"불러올 맵 파일 경로");
        static std::string tempNameLoad;
        CPImGui::InputString("##LoadFileName", &tempNameLoad);

        if (ImGui::Button(u8"불러오기##LoadFile") && tempNameLoad.size())
        {
            tempNameLoad += ".map";

            bool bRet = loadMap(atl_M2W(tempNameLoad));

            if (bRet) 
            { 
                popOpt = 0;
                bOpenWorkspace = true;
                ImGui::CloseCurrentPopup(); 
            }

            tempNameLoad.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button(u8"닫기"))
        {
            popOpt = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void CPMapWorkspace::saveMapPopup(int& popOpt)
{
    ImGui::PushID("MAP_SAVE");

    ImGui::OpenPopup(u8"파일 저장");

    //항상 가운데에서 생성하게 한다.
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(u8"파일 저장", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SeparatorText(u8"저장할 맵 파일 이름");
        static std::string tempNameSave;
        CPImGui::InputString("##SaveFileName", &tempNameSave);

        if (ImGui::Button(u8"저장##SaveFile") && tempNameSave.size())
        {
            bool bRet = saveMap(atl_M2W(tempNameSave));

            if (bRet) { ImGui::CloseCurrentPopup(); }

            tempNameSave.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button(u8"닫기"))
        {
            popOpt = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

bool CPMapWorkspace::loadMap(std::wstring wszMapFileName)
{
    return false;
}

bool CPMapWorkspace::saveMap(std::wstring wszMapFileName)
{
    return false;
}

int CPMapWorkspace::getRenderedLeafNodeCount()
{
    if (m_pQuadTree) { return m_pQuadTree->m_drawLeafNodeList.size(); }
    return 0;
}

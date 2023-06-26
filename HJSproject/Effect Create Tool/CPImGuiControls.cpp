#include "CPImGuiControls.h"

CPIMGUITexSelector::CPIMGUITexSelector()
{
    m_wszTargetExt.push_back(L".tga");
    m_wszTargetExt.push_back(L".dds");
    m_wszTargetExt.push_back(L".png");
    m_wszTargetExt.push_back(L".jpg");
    m_wszTargetExt.push_back(L".jpeg");
    m_wszTargetExt.push_back(L".bmp");
    m_wszTargetExt.push_back(L".gif");
    m_wszTargetExt.push_back(L".TGA");
    m_wszTargetExt.push_back(L".DDS");
    m_wszTargetExt.push_back(L".PNG");
    m_wszTargetExt.push_back(L".JPG");
    m_wszTargetExt.push_back(L".JPEG");
    m_wszTargetExt.push_back(L".BMP");
    m_wszTargetExt.push_back(L".GIF");
}

bool CPIMGUITexSelector::init(std::string szTexSearchPath)
{
    m_szTexSearchPath = szTexSearchPath.c_str();

    return true;
}

void CPIMGUITexSelector::selectClear()
{
    m_szSelectedTexName.clear();
    m_wszSelectedTexName.clear();
    m_wszSelectedTexPath.clear();
    m_pSelectedTex = nullptr;
    m_iTexIdx = -1;
}

void CPIMGUITexSelector::showUIControl(const char* ID)
{
    ImGui::PushID(ID);

    ImGui::Text(u8"텍스처 검색 경로");
    CPImGui::InputString("##Name0", &m_szTexSearchPath); ImGui::SameLine();

    if (ImGui::Button(u8"검색"))
    {
        if (m_szTexSearchPath.size())
        {
            std::string temp = m_szTexSearchPath;
            char chLast = temp[temp.size() - 1];
            if (chLast != '\\' && chLast != '/')
            {
                temp += '\\';
            }

            m_wszSelectedTexPath.clear();
            m_szTexNames.clear();

            loadDir(atl_M2W(temp), &m_wszTargetExt, &m_wszTexFilePaths, &m_szTexNames);
        }
    }

    ImGui::Text(u8"텍스처");
    if (ImGui::BeginCombo(u8"##Combo0", m_szSelectedTexName.c_str()))
    {
        for (int i = 0; i < m_szTexNames.size(); i++)
        {
            const bool is_selected = (m_iTexIdx == i);
            if (ImGui::Selectable(m_szTexNames[i].c_str(), is_selected))
            {
                m_iTexIdx = i;
                m_szSelectedTexName = m_szTexNames[m_iTexIdx];
                m_wszSelectedTexPath = m_wszTexFilePaths[m_iTexIdx];
                m_wszSelectedTexName = atl_M2W(m_szSelectedTexName);

                if (!CPGC_TEXTURE_MGR.getPtr(atl_M2W(m_szSelectedTexName)))
                {
                    if (SUCCEEDED(CPGC_TEXTURE_MGR.createTexture(atl_M2W(m_szSelectedTexName), m_wszSelectedTexPath)))
                    {
                        m_pSelectedTex = CPGC_TEXTURE_MGR.getPtr(atl_M2W(m_szSelectedTexName));
                    }
                }
                else
                {
                    m_pSelectedTex = CPGC_TEXTURE_MGR.getPtr(atl_M2W(m_szSelectedTexName));
                }
            }

            //콤보 박스가 보여질 때 포커싱될 요소를 설정하는 부분
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (m_szSelectedTexName == "None")
    {
        ImGui::SameLine();
        ImGui::Image(nullptr, ImVec2(100, 100));
    }
    else if (m_pSelectedTex)
    {
        ImGui::SameLine();
        ImGui::Image((void*)m_pSelectedTex->getSRV(), ImVec2(100, 100));
    }

    ImGui::PopID();
}

CPIMGUIMultiTexSelector::CPIMGUIMultiTexSelector()
{
    m_wszTargetExt.push_back(L".tga");
    m_wszTargetExt.push_back(L".dds");
    m_wszTargetExt.push_back(L".png");
    m_wszTargetExt.push_back(L".jpg");
    m_wszTargetExt.push_back(L".jpeg");
    m_wszTargetExt.push_back(L".bmp");
    m_wszTargetExt.push_back(L".gif");
    m_wszTargetExt.push_back(L".TGA");
    m_wszTargetExt.push_back(L".DDS");
    m_wszTargetExt.push_back(L".PNG");
    m_wszTargetExt.push_back(L".JPG");
    m_wszTargetExt.push_back(L".JPEG");
    m_wszTargetExt.push_back(L".BMP");
    m_wszTargetExt.push_back(L".GIF");
}

bool CPIMGUIMultiTexSelector::init(std::string szTexSearchPath)
{
    m_szTexSearchPath = szTexSearchPath.c_str();

    return true;
}

void CPIMGUIMultiTexSelector::selectClear()
{
    m_szSelectedTexName.clear();
    m_wszSelectedTexName.clear();
    m_wszSelectedTexPath.clear();
    m_pSelectedTex = nullptr;
    m_iTexIdx = -1;
    m_bSelection.clear();

    m_wszSelectedTexFilePaths.clear();
    m_szSelectedTexNames.clear();
    m_wszSelectedTexNames.clear();
    m_iTexIdx2 = -1;
}

void CPIMGUIMultiTexSelector::showUIControl(const char* ID)
{
    ImGui::PushID(ID);

    ImGui::Text(u8"텍스처 검색 경로");
    CPImGui::InputString("##Name0", &m_szTexSearchPath); ImGui::SameLine();

    if (ImGui::Button(u8"검색"))
    {
        if (m_szTexSearchPath.size())
        {
            std::string temp = m_szTexSearchPath;
            char chLast = temp[temp.size() - 1];
            if (chLast != '\\' && chLast != '/')
            {
                temp += '\\';
            }

            m_wszSelectedTexPath.clear();
            m_szTexNames.clear();

            loadDir(atl_M2W(temp), &m_wszTargetExt, &m_wszTexFilePaths, &m_szTexNames);

            m_bSelection.clear();
            m_bSelection.resize(m_szTexNames.size());
        }
    }

    ImGui::Text(u8"텍스처");
    if (ImGui::BeginListBox(u8"##List0"))
    {
        for (int i = 0; i < m_szTexNames.size(); i++)
        {
            if (ImGui::Selectable(m_szTexNames[i].c_str(), m_bSelection[i]))
            {
                if (!ImGui::GetIO().KeyCtrl) { memset(&m_bSelection.at(0), 0, m_bSelection.size()); }

                m_bSelection[i] ^= 1;

                m_iTexIdx = i;
                m_szSelectedTexName = m_szTexNames[m_iTexIdx];
                m_wszSelectedTexPath = m_wszTexFilePaths[m_iTexIdx];
                m_wszSelectedTexName = atl_M2W(m_szSelectedTexName);

                if (!CPGC_TEXTURE_MGR.getPtr(atl_M2W(m_szSelectedTexName)))
                {
                    if (SUCCEEDED(CPGC_TEXTURE_MGR.createTexture(atl_M2W(m_szSelectedTexName), m_wszSelectedTexPath)))
                    {
                        m_pSelectedTex = CPGC_TEXTURE_MGR.getPtr(atl_M2W(m_szSelectedTexName));
                    }
                }
                else
                {
                    m_pSelectedTex = CPGC_TEXTURE_MGR.getPtr(atl_M2W(m_szSelectedTexName));
                }
            }
        }
        ImGui::EndListBox();
    }

    if (m_szSelectedTexName == "None")
    {
        ImGui::SameLine();
        ImGui::Image(nullptr, ImVec2(100, 100));
    }
    else if (m_pSelectedTex)
    {
        ImGui::SameLine();
        ImGui::Image((void*)m_pSelectedTex->getSRV(), ImVec2(100, 100));
    }

    if (ImGui::Button(u8"선택 리스트에 추가"))
    {
        for (int i = 0; i < m_szTexNames.size(); i++)
        {
            if (m_bSelection[i]) 
            { 
                m_szSelectedTexNames.push_back(m_szTexNames[i]);
                m_wszSelectedTexNames.push_back(atl_M2W(m_szTexNames[i])); 
                m_wszSelectedTexFilePaths.push_back(m_wszTexFilePaths[i]);
            }
        }
    }

    if (ImGui::BeginListBox(u8"##List1"))
    {
        for (int i = 0; i < m_szSelectedTexNames.size(); i++)
        {
            const bool is_selected = (m_iTexIdx2 == i);
            ImGui::Selectable(m_szSelectedTexNames[i].c_str(), is_selected);
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button(u8"리스트 비우기"))
    {
        m_szSelectedTexNames.clear();
        m_wszSelectedTexNames.clear();
        m_wszSelectedTexFilePaths.clear();
    }

    ImGui::PopID();
}
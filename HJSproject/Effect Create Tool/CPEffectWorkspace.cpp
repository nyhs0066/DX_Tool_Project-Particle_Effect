#include "CPEffectWorkspace.h"

CPEffectWorkspace::CPEffectWorkspace()
{
    m_ECProp.fScaleOverLifespan         = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_ECProp.initColorOverLifespan      = { 1.0f, 1.0f ,1.0f ,1.0f };
    m_ECProp.lastColorOverLifespan      = { 1.0f, 1.0f ,1.0f ,1.0f };
    m_ECProp.iMaxParticleNum            = CPIMGUI_MIN_ITEM_COUNT;
    m_ECProp.fSpawnRate                 = 1.0f;
    m_ECProp.fLifespanRange             = { CPIMGUI_MIN_ITEM_LIFESPAN, CPIMGUI_MIN_ITEM_LIFESPAN };
    m_ECProp.vInitScale                 = { 1.0f, 1.0f };

	m_iCreateFlag                       = 0;
	m_iECRowNum                         = 1;
	m_iECColNum                         = 1;  

    m_pDevice                           = nullptr;
    m_pDContext                         = nullptr;

    m_pMainCam                          = nullptr;
    m_pRT                               = nullptr;

    m_iECBlendOpt                       = BS_ALPHABLEND;
    m_bECEnableDepthOpt                 = true;
    m_bECEnableDepthWriteOpt            = false;
    m_bECEnableBackCullOpt              = true;

    m_pCurrentPSystem                   = nullptr;
    m_selectedEmitterIdx                = -1;
    m_emitterSelectionMask              = 0;

    m_pEffectTexSelector                = nullptr;
    m_pCreateUVSpriteTexSelector        = nullptr;
    m_pCreateMTSpriteTexSelector        = nullptr;

    m_iUVNameIdx                        = 0;
    m_iMTNameIdx                        = 0;

    m_pSelectedEmitter                  = nullptr;
    m_pEmitterTexSelector               = nullptr;

    m_vScale = { 1.0f, 1.0f, 1.0f };
    m_fLimitDist = 100.0f;
}

CPEffectWorkspace::~CPEffectWorkspace()
{
}

bool CPEffectWorkspace::init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext, CPGCRenderTarget* pMainRT, CPGCCamera* pMainCam)
{
    //���� ������
    m_pDevice = pDevice;
    m_pDContext = pDContext;
    m_pRT = pMainRT;

    setMainCam(pMainCam);

    m_pEffectTexSelector = new CPIMGUITexSelector;
    m_pEffectTexSelector->init("../../data/texture/particle/");

    m_pCreateUVSpriteTexSelector = new CPIMGUITexSelector;
    m_pCreateUVSpriteTexSelector->init("../../data/texture/particle/");

    m_pCreateMTSpriteTexSelector = new CPIMGUIMultiTexSelector;
    m_pCreateMTSpriteTexSelector->init("../../data/texture/particle/");

    m_pEmitterTexSelector = new CPIMGUITexSelector;
    m_pEmitterTexSelector->init("../../data/texture/particle/");

    CPGC_SPRITE_MGR.getUVSpriteNames(m_szUVNames);
    CPGC_SPRITE_MGR.getMTSpriteNames(m_szMTNames);

	return true;
}

bool CPEffectWorkspace::preUpdate()
{
    return true;
}

bool CPEffectWorkspace::update()
{
    if (m_pCurrentPSystem)
    {
        m_pCurrentPSystem->update();

        if (CPM_Vector3::Distance(CPM_Vector3::Zero, m_vPos) > m_fLimitDist)
        {
            m_vPos = CPM_Vector3::Zero;
            m_vVelocity = m_initVelocity;
        }
        else
        {
            float dt = CPGC_MAINTIMER.getOneFrameTimeF();

            m_vVelocity += m_initAcceleration * dt;
            m_vPos += m_vVelocity * dt;
        }
    }

    return true;
}

bool CPEffectWorkspace::postUpdate()
{
    if (m_pCurrentPSystem)
    {
        world = CPM_Matrix::CreateScale(m_vScale) * CPM_Matrix::CreateTranslation(m_vPos);

        m_pCurrentPSystem->setMatrix(&world, m_pMainCam->getViewMatPtr(), m_pMainCam->getProjMatPtr());
    }

    return true;
}

bool CPEffectWorkspace::preRender()
{
    return true;
}

bool CPEffectWorkspace::render()
{
    if (m_pCurrentPSystem)
    {
        m_pCurrentPSystem->render();
    }

    return true;
}

bool CPEffectWorkspace::postRender()
{
    return true;
}

bool CPEffectWorkspace::release()
{
    if (m_pCurrentPSystem)
    {
        m_pCurrentPSystem->release();
        delete m_pCurrentPSystem;
        m_pCurrentPSystem = nullptr;
    }

    if (m_pEffectTexSelector)
    {
        delete m_pEffectTexSelector;
        m_pEffectTexSelector = nullptr;
    }

    return true;
}

void CPEffectWorkspace::setMainCam(CPGCCamera* pCam)
{
    m_pMainCam = pCam;
}

void CPEffectWorkspace::openWorkspaceWindow()
{
    ImGui::PushID("EFFECT1");

    if(m_pCurrentPSystem)
    {
        showParticleSystemWindow();
        showEmitterListWindow();
        showCreateSpriteWindow();
    }

    if (m_pSelectedEmitter)
    {
        showEmitterPropertyWindow();
    }

    ImGui::PopID();
}

void CPEffectWorkspace::showParticleSystemWindow()
{
    ImGui::PushID("EFFECT1");

    std::string temp = m_pCurrentPSystem->getSzName();
    temp += "##EFFECT1";

    ImGui::Begin(temp.c_str());

    ImGui::Separator();
    ImGui::SeparatorText(u8"��ƼŬ �ý��� �Ӽ�");
    ImGui::Separator();

    ImGui::Text(u8"���ӽð�"); ImGui::SameLine();
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("##Duration0", &m_pCurrentPSystem->m_PSProp.fDuration, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"���� ���� �ð�"); ImGui::SameLine();
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("##StDelay0", &m_pCurrentPSystem->m_PSProp.fStDelay, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"��� �ӵ�"); ImGui::SameLine();
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("##PlaySpeed0", &m_pCurrentPSystem->m_PSProp.fPlaySpeed, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"���� ���"); ImGui::SameLine();
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::Checkbox("##Loop0", &m_pCurrentPSystem->m_PSProp.bInfinitePlay);

    ImGui::Text(u8"ȭ���� ����� �� ���� ���"); ImGui::SameLine();
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::RadioButton(u8"�Ͻ�����##CullMode0", &m_pCurrentPSystem->m_PSProp.iCullMode, (int)CPRS_PARTICLESYSTEM_CULLMODE::CPRS_PSYSTEM_CULLMODE_PAUSE); ImGui::SameLine();
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::RadioButton(u8"��� ����##CullMode0", &m_pCurrentPSystem->m_PSProp.iCullMode, (int)CPRS_PARTICLESYSTEM_CULLMODE::CPRS_PSYSTEM_CULLMODE_ALWAYS);

    ImGui::SeparatorText(u8"Ʈ������");
    ImGui::DragFloat3("Pos##Transform0", (float*)&m_vPos, 0.1f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");
    ImGui::DragFloat3("Scale##Transform0", (float*)&m_vScale, 0.1f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"�ʱ� �ӵ�");
    ImGui::DragFloat3("Velocity##Physics0", (float*)&m_initVelocity, 1.0f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");

    ImGui::Text(u8"�ʱ� ���ӵ�");
    ImGui::DragFloat3("Accel##Physics0", (float*)&m_initAcceleration, 1.0f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");

    ImGui::Text(u8"���� �Ÿ�");
    ImGui::DragFloat("LimitDist##Physics0", &m_fLimitDist, 0.1f, 1.0f, 100.0f, "%.3f");

    if (ImGui::Button(u8"����##Apply0"))
    {
        m_vVelocity = m_initVelocity;
    }

    ImGui::Separator();
    ImGui::SeparatorText(u8"�̹��� ����");
    ImGui::Separator();

    ImGui::Text(u8"Emitter �̸�");
    CPImGui::InputString("##name1", &m_szCreateEmitterName);

    ImGui::Text(u8"Emitter Ÿ��");
    ImGui::RadioButton(u8"�⺻ ��ƼŬ##EmitterType0", (int*)&m_iCreateEmitterType, (int)CPRS_EMITTER_TYPE::DEFAULT_PARTICLE);
    ImGui::RadioButton(u8"Ʈ����##EmitterType0", (int*)&m_iCreateEmitterType, (int)CPRS_EMITTER_TYPE::PARTICLE_TRAIL);
    ImGui::RadioButton(u8"����Ʈ##EmitterType0", (int*)&m_iCreateEmitterType, (int)CPRS_EMITTER_TYPE::BURST_PARTICLE);

    if (ImGui::Button(u8" ���� ", ImVec2(40, 40)))
    {
        if (m_szCreateEmitterName.size())
        {
            if (isValidName(m_szCreateEmitterName))
            {
                switch(m_iCreateEmitterType)
                { 
                    case CPRS_EMITTER_TYPE::DEFAULT_PARTICLE :
                    {
                        createDefaultEmitter();
                    } break;

                    case CPRS_EMITTER_TYPE::PARTICLE_TRAIL:
                    {
                        createTrailEmitter();
                    } break;

                    case CPRS_EMITTER_TYPE::BURST_PARTICLE:
                    {
                        createBurstEmitter();
                    } break;
                }
            }

            m_szCreateEmitterName.clear();
        }
    }

    ImGui::End();

    ImGui::PopID();
}

void CPEffectWorkspace::showEmitterListWindow()
{
    ImGui::PushID("EmitterList");

    ImGui::Begin(u8"�̹��� ����Ʈ");

    if (ImGui::TreeNode(m_pCurrentPSystem->getSzName()))
    {
        int idx = 0;

        static bool bModifyNameFlag;

        if (ImGui::BeginPopupContextItem("ItemModify0"))
        {
            if (ImGui::Selectable(u8"�̸� ����"))
            {
                bModifyNameFlag = true;
            }
            if (ImGui::Selectable(u8"����"))
            {
                auto it = std::find(m_pCurrentPSystem->m_pEmitterList.begin(), m_pCurrentPSystem->m_pEmitterList.end(), m_pSelectedEmitter);
                m_pCurrentPSystem->m_pEmitterList.erase(it);
                m_pSelectedEmitter->release();
                delete m_pSelectedEmitter;
                m_pSelectedEmitter = nullptr;
                m_selectedEmitterIdx = -1;
            }
            if (ImGui::Selectable(u8"�̹��� ���� ����"))
            {
                CPRSEmitter* pCopyEmitter = nullptr;

                switch (m_pSelectedEmitter->m_emitterType)
                {
                    //ComPtr�� ���۱��� ���� ���縦 �����Ѵ�.
                    case CPRS_EMITTER_TYPE::DEFAULT_PARTICLE :
                    {
                        CPRSPointParticleEmitter* pNew = new CPRSPointParticleEmitter;
                        *pNew = *((CPRSPointParticleEmitter*)m_pSelectedEmitter);

                        pCopyEmitter = pNew;
                    }break;

                    case CPRS_EMITTER_TYPE::PARTICLE_TRAIL:
                    {
                        CPRSParticleTrailEmitter* pNew = new CPRSParticleTrailEmitter;
                        *pNew = *((CPRSParticleTrailEmitter*)m_pSelectedEmitter);

                        pCopyEmitter = pNew;
                    }break;

                    case CPRS_EMITTER_TYPE::BURST_PARTICLE:
                    {
                        CPRSParticleBurstEmitter* pNew = new CPRSParticleBurstEmitter;
                        *pNew = *((CPRSParticleBurstEmitter*)m_pSelectedEmitter);

                        pCopyEmitter = pNew;
                    }break;
                }

                if (pCopyEmitter)
                {
                    pCopyEmitter->setName((std::wstring(m_pSelectedEmitter->getWszName()) + L"_COPY").c_str());
                    m_pCurrentPSystem->m_pEmitterList.push_back(pCopyEmitter);
                }
            }
            ImGui::EndPopup();
        }

        if (bModifyNameFlag) 
        {
            ImGui::OpenPopup("EditName0");

            if (ImGui::BeginPopup("EditName0"))
            {
                ImGui::Text(u8"������ �̸�:");
                static std::string szNewName = m_pSelectedEmitter->getSzName();
                CPImGui::InputString("##name0", &szNewName);
                if (ImGui::Button(u8"����"))
                {
                    if (szNewName.size())
                    {
                        auto it = std::find(m_pCurrentPSystem->m_pEmitterList.begin(), m_pCurrentPSystem->m_pEmitterList.end(), m_pSelectedEmitter);
                        m_pCurrentPSystem->m_pEmitterList.erase(it);
                        m_pSelectedEmitter->setName(szNewName);
                        m_pCurrentPSystem->m_pEmitterList.push_back(m_pSelectedEmitter);
                    }

                    bModifyNameFlag = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button(u8"�ݱ�"))
                {
                    bModifyNameFlag = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        for (int idx = 0; idx < m_pCurrentPSystem->m_pEmitterList.size(); idx++)
        {
            ImGuiTreeNodeFlags nodeflags = 0;
            const bool is_selected = (m_emitterSelectionMask & (1 << idx)) != 0;
            if (is_selected)
                nodeflags |= ImGuiTreeNodeFlags_Selected;
            nodeflags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            ImGui::TreeNodeEx((void*)(intptr_t)idx, nodeflags, m_pCurrentPSystem->m_pEmitterList[idx]->getSzName());
            if ((ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right)) && !ImGui::IsItemToggledOpen())
            {
                m_selectedEmitterIdx = idx;
                m_pSelectedEmitter = m_pCurrentPSystem->m_pEmitterList[idx];
            }

            ImGui::OpenPopupOnItemClick("ItemModify0", ImGuiPopupFlags_MouseButtonRight);

            ImGui::PushID(idx);
            // �巡�� �� ��� �������� : ������ �۾� �������� ��ǥ�� �޴´�.
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                // �巡�� �� ��� ���̷ε�
                ImGui::SetDragDropPayload("Emitter", &idx, sizeof(int));

                // �̸����� �ؽ�Ʈ
                ImGui::Text("Swap %s", m_pCurrentPSystem->m_pEmitterList[idx]->getSzName());
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Emitter"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(int));
                    int payloadIdx = *(const int*)payload->Data;

                    CPRSEmitter* temp = m_pCurrentPSystem->m_pEmitterList[idx];
                    m_pCurrentPSystem->m_pEmitterList[idx] = m_pCurrentPSystem->m_pEmitterList[payloadIdx];
                    m_pCurrentPSystem->m_pEmitterList[payloadIdx] = temp;
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::PopID();
        }

        if (m_selectedEmitterIdx != -1)
        {
            m_emitterSelectionMask = (1 << m_selectedEmitterIdx);
        }
        else 
        {
            m_emitterSelectionMask = 0;
            m_pSelectedEmitter = nullptr;
        }

        ImGui::TreePop();
    }

    ImGui::End();

    ImGui::PopID();
}

void CPEffectWorkspace::showEmitterPropertyWindow()
{
    if (m_pSelectedEmitter)
    {
        switch (m_pSelectedEmitter->m_emitterType)
        {
        case CPRS_EMITTER_TYPE::DEFAULT_PARTICLE:
        {
            showDefaultEmitterPropWindow(dynamic_cast<CPRSPointParticleEmitter*>(m_pSelectedEmitter));
        }break;

        case CPRS_EMITTER_TYPE::PARTICLE_TRAIL:
        {
            showTrailEmitterPropWindow(dynamic_cast<CPRSParticleTrailEmitter*>(m_pSelectedEmitter));
        }break;

        case CPRS_EMITTER_TYPE::BURST_PARTICLE:
        {
            showBurstEmitterPropWindow(dynamic_cast<CPRSParticleBurstEmitter*>(m_pSelectedEmitter));
        }break;
        };
    }

}

void CPEffectWorkspace::showCreateSpriteWindow()
{
    ImGui::PushID("CreateSprite");

    ImGui::Begin(u8"��������Ʈ �����");

    static int createFlag = 0;

    ImGui::RadioButton(u8"UV ��������Ʈ##Type", &createFlag, 0); ImGui::SameLine();
    ImGui::RadioButton(u8"MT ��������Ʈ##Type", &createFlag, 1);

    static int iRows;
    static int iCols;

    if (createFlag == 0)
    {
        m_pCreateUVSpriteTexSelector->showUIControl("CreateSprite0");

        ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET + 40);
        ImGui::DragInt(u8"�� ����##Grid0", &iRows, 1.0f, CPIMGUI_MIN_ITEM_COUNT, CPIMGUI_MAX_ITEM_COUNT, "%d"); ImGui::SameLine();

        ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET + 40);
        ImGui::DragInt(u8"�� ����##Grid0", &iCols, 1.0f, CPIMGUI_MIN_ITEM_COUNT, CPIMGUI_MAX_ITEM_COUNT, "%d");
    }
    else if (createFlag == 1)
    {
        CPImGui::InputString(u8"MT ��������Ʈ �̸�", &m_szMTSpriteName);

        m_pCreateMTSpriteTexSelector->showUIControl("CreateSprite1");
    }

    ImGui::Separator();
    ImGui::Separator();

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    if (ImGui::Button(u8"�����"))
    {
        if (createFlag == 0)
        {
            std::wstring wszSpriteName = L"UVS_";
            wszSpriteName += m_pCreateUVSpriteTexSelector->m_wszSelectedTexName;

            CPGC_SPRITE_MGR.createUVSprite(wszSpriteName,
                m_pCreateUVSpriteTexSelector->m_wszSelectedTexName,
                m_pCreateUVSpriteTexSelector->m_wszSelectedTexPath,
                iRows, iCols);

            CPGC_SPRITE_MGR.getUVSpriteNames(m_szUVNames);
        }
        else if (createFlag == 1)
        {
            if (m_szMTSpriteName.size())
            {
                std::wstring wszSpriteName = L"MTS_";
                wszSpriteName += atl_M2W(m_szMTSpriteName);

                CPGC_SPRITE_MGR.createMTSprite(wszSpriteName,
                    &(m_pCreateMTSpriteTexSelector->m_wszSelectedTexNames),
                    &(m_pCreateMTSpriteTexSelector->m_wszSelectedTexFilePaths));

                CPGC_SPRITE_MGR.getMTSpriteNames(m_szMTNames);

                m_szMTSpriteName.clear();

                m_pCreateMTSpriteTexSelector->m_szSelectedTexNames.clear();
                m_pCreateMTSpriteTexSelector->m_wszSelectedTexNames.clear();
                m_pCreateMTSpriteTexSelector->m_wszSelectedTexFilePaths.clear();
            }
        }
    }

    ImGui::End();

    ImGui::PopID();
}

void CPEffectWorkspace::showDefaultEmitterPropWindow(CPRSPointParticleEmitter* pTargetEmitter)
{
    ImGui::PushID("EmitterProperty_Default");

    ImGui::Begin(u8"�̹��� �Ӽ�");

    ImGui::Text(u8"Emitter �̸�");
    ImGui::Text(pTargetEmitter->getSzName()); //CPImGui::InputString("##name1", &m_szCreateEmitterName);

    ImGui::SeparatorText(u8"��ƼŬ �ʱ� Ʈ������");

    ImGui::Text(u8"�̹��� ��ġ");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("X##Pos0", &pTargetEmitter->m_matWorld._41, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f"); ImGui::SameLine();

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("Y##Pos0", &pTargetEmitter->m_matWorld._42, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f"); ImGui::SameLine();

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("Z##Pos0", &pTargetEmitter->m_matWorld._43, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"��ġ ������ �ּҰ�");
    ImGui::DragFloat3("Min##Pos0", (FLOAT*)&pTargetEmitter->m_eProp.vMinPosOffset, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"��ġ ������ �ִ밪");
    ImGui::DragFloat3("Max##Pos0", (FLOAT*)&pTargetEmitter->m_eProp.vMaxPosOffset, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"�ʱ� ȸ�� ����");
    ImGui::DragFloat3("##Rot0", (FLOAT*)&pTargetEmitter->m_eProp.fInitPYR, 1.0f, CPIMGUI_MIN_ITEM_ROTVELOCITY, CPIMGUI_MAX_ITEM_ROTVELOCITY, "%.3f");

    ImGui::Text(u8"ȸ�� ���� ������");
    ImGui::DragFloat3("Min##Rot0", (FLOAT*)&pTargetEmitter->m_eProp.fExtraPYRMinRange, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");
    ImGui::DragFloat3("Max##Rot0", (FLOAT*)&pTargetEmitter->m_eProp.fExtraPYRMaxRange, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"�ʱ� ũ��");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloat2("##Scale0", (FLOAT*)&pTargetEmitter->m_eProp.vInitScale, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.3f");

    ImGui::Text(u8"ũ�� ������(�ּ�/�ִ�)");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("X(Min/Max)##Scale0", &pTargetEmitter->m_eProp.vExtraScaleRange.x, &pTargetEmitter->m_eProp.vExtraScaleRange.z, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.2f");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("Y(Min/Max)##Scale0", &pTargetEmitter->m_eProp.vExtraScaleRange.y, &pTargetEmitter->m_eProp.vExtraScaleRange.w, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.2f");

    ImGui::SeparatorText(u8"��ƼŬ ����");

    ImGui::Text(u8"�ؽ�ó / ��������Ʈ ����");
    ImGui::RadioButton(u8"���� �ؽ�ó##Type", &m_iTexTypeFlag, 0); ImGui::SameLine();
    ImGui::RadioButton(u8"UV �ؽ�ó##Type", &m_iTexTypeFlag, 1); ImGui::SameLine();
    ImGui::RadioButton(u8"��Ƽ �ؽ�ó##Type", &m_iTexTypeFlag, 2);

    if (m_iTexTypeFlag == 0)
    {
        m_pEmitterTexSelector->showUIControl("EffectSel0");

        if (m_pEmitterTexSelector->m_pSelectedTex)
        {
            pTargetEmitter->m_pTexture = m_pEmitterTexSelector->m_pSelectedTex;
            pTargetEmitter->m_pSprite = nullptr;
            pTargetEmitter->m_UVRect.vMin = { 0.0f, 0.0f };
            pTargetEmitter->m_UVRect.vMax = { 1.0f, 1.0f };

            m_iEmitterUVNameIdx = -1;
            m_iEmitterMTNameIdx = -1;
        }
    }

    if (m_iTexTypeFlag == 1)
    {
        ImGui::Text(u8"UV ��������Ʈ");
        if (ImGui::BeginCombo(u8"##Combo0", m_szSelectedEmitterUVName.c_str()))
        {
            for (int i = 0; i < m_szUVNames.size(); i++)
            {
                const bool is_selected = (m_iEmitterUVNameIdx == i);
                if (ImGui::Selectable(m_szUVNames[i].c_str(), is_selected))
                {
                    m_iEmitterUVNameIdx = i;
                    m_szSelectedEmitterUVName = m_szUVNames[m_iEmitterUVNameIdx];
                    m_wszSelectedEmitterUVName = atl_M2W(m_szSelectedEmitterUVName);

                    pTargetEmitter->m_pSprite = CPGC_SPRITE_MGR.getUVPtr(m_wszSelectedEmitterUVName);
                }

                //�޺� �ڽ��� ������ �� ��Ŀ�̵� ��Ҹ� �����ϴ� �κ�
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (m_iEmitterUVNameIdx != -1)
        {
            m_pEmitterTexSelector->selectClear();
        }
    }

    if (m_iTexTypeFlag == 2)
    {
        ImGui::Text(u8"MT ��������Ʈ");
        if (ImGui::BeginCombo(u8"##Combo1", m_szSelectedEmitterMTName.c_str()))
        {
            for (int i = 0; i < m_szMTNames.size(); i++)
            {
                const bool is_selected = (m_iEmitterMTNameIdx == i);
                if (ImGui::Selectable(m_szMTNames[i].c_str(), is_selected))
                {
                    m_iEmitterMTNameIdx = i;
                    m_szSelectedEmitterMTName = m_szMTNames[m_iEmitterMTNameIdx];
                    m_wszSelectedEmitterMTName = atl_M2W(m_szSelectedEmitterMTName);

                    pTargetEmitter->m_pSprite = CPGC_SPRITE_MGR.getMTPtr(m_wszSelectedEmitterMTName);
                    pTargetEmitter->m_UVRect.vMin = { 0.0f, 0.0f };
                    pTargetEmitter->m_UVRect.vMax = { 1.0f, 1.0f };
                }

                //�޺� �ڽ��� ������ �� ��Ŀ�̵� ��Ҹ� �����ϴ� �κ�
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (m_iEmitterMTNameIdx != -1)
        {
            m_pEmitterTexSelector->selectClear();
        }
    }

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragInt(u8"��ƼŬ �ִ� ����##ECbuf0", &pTargetEmitter->m_eProp.iMaxParticleNum, 1.0f, CPIMGUI_MIN_ITEM_COUNT, CPIMGUI_MAX_ITEM_COUNT, "%d");

    if (ImGui::Button(u8"����##modify0"))
    {
        pTargetEmitter->m_vertices.clear();
        pTargetEmitter->m_particles.clear();

        pTargetEmitter->m_vertices.resize(pTargetEmitter->m_eProp.iMaxParticleNum);
        pTargetEmitter->m_particles.resize(pTargetEmitter->m_eProp.iMaxParticleNum);

        pTargetEmitter->m_iVertexCount = pTargetEmitter->m_vertices.size();

        pTargetEmitter->m_pVBuf.ReleaseAndGetAddressOf();

        HRESULT hr = createDXSimpleBuf(m_pDevice, pTargetEmitter->m_iVertexCount * sizeof(CPRS_ParticleVertex), &pTargetEmitter->m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, pTargetEmitter->m_pVBuf.GetAddressOf());
    }

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat(u8"�ʴ� ���� ��##ECbuf0", &pTargetEmitter->m_eProp.fSpawnRate, 0.25f, CPIMGUI_MIN_ITEM_COUNT, CPIMGUI_MAX_ITEM_COUNT, "%.1f");

    if (ImGui::Button(u8"����##modify1"))
    {
        pTargetEmitter->m_fSpawnTime = 1.0f / pTargetEmitter->m_eProp.fSpawnRate;
        pTargetEmitter->m_fTimer = 0.0f;
    }

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::Checkbox(u8"��ƼŬ ���� ������ �ݺ�", &pTargetEmitter->m_eProp.bLoop);

    ImGui::SeparatorText(u8"��ƼŬ �Ӽ�");

    ImGui::Text(u8"����");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("##LifeSpan0", &pTargetEmitter->m_eProp.fLifespanRange.x, &pTargetEmitter->m_eProp.fLifespanRange.y, 0.05f, CPIMGUI_MIN_ITEM_LIFESPAN, CPIMGUI_MAX_ITEM_LIFESPAN, "%.1f");

    if (pTargetEmitter->m_eProp.fLifespanRange.x > pTargetEmitter->m_eProp.fLifespanRange.y)
    {
        pTargetEmitter->m_eProp.fLifespanRange.x = pTargetEmitter->m_eProp.fLifespanRange.y;
    }

    ImGui::Text(u8"�ӵ�");
    ImGui::DragFloat3("Min##Velocity0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMinVelocity, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");
    ImGui::DragFloat3("Max##Velocity0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMaxVelocity, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");

    ImGui::Text(u8"���ӵ�");
    ImGui::DragFloat3("Min##Accel0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMinAcceleration, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");
    ImGui::DragFloat3("Max##Accel0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMaxAcceleration, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");

    ImGui::Text(u8"�ʴ� ȸ�� �ӵ�");
    ImGui::DragFloat3("Min##RotVelocity0", (FLOAT*)&pTargetEmitter->m_eProp.fMinPYRVelocity, 1.0f, CPIMGUI_MIN_ITEM_ROTVELOCITY, CPIMGUI_MAX_ITEM_ROTVELOCITY, "%.1f");
    ImGui::DragFloat3("Max##RotVelocity0", (FLOAT*)&pTargetEmitter->m_eProp.fMaxPYRVelocity, 1.0f, CPIMGUI_MIN_ITEM_ROTVELOCITY, CPIMGUI_MAX_ITEM_ROTVELOCITY, "%.1f");

    ImGui::Text(u8"���� ���� ũ��");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("X(Init/Last)##ScaleOverLifespan0", &pTargetEmitter->m_eProp.fScaleOverLifespan.x, &pTargetEmitter->m_eProp.fScaleOverLifespan.z, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.1f");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("Y(Init/Last)##ScaleOverLifespan0", &pTargetEmitter->m_eProp.fScaleOverLifespan.y, &pTargetEmitter->m_eProp.fScaleOverLifespan.w, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.1f");

    ImGui::Text(u8"���� ���� ����");
    ImGui::ColorEdit4("Init##Color0", (float*)&pTargetEmitter->m_eProp.initColorOverLifespan, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit4("Last##Color0", (float*)&pTargetEmitter->m_eProp.lastColorOverLifespan, ImGuiColorEditFlags_Float);

    ImGui::SeparatorText(u8"���� �Ӽ�");

    //Ʈ�� ������ Push / pop�� ¦�� �¾ƾ� �Ѵ�.
    ImGui::RadioButton(u8"���� �׽�Ʈ", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_MSALPHATEST); ImGui::SameLine();
    ImGui::RadioButton(u8"���� ����", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_ALPHABLEND); ImGui::SameLine();
    ImGui::RadioButton(u8"��� �ҽ� ����", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_DUALSOURCEBLEND);
    ImGui::RadioButton(u8"���� ����", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_ADDITIVEBLEND);

    ImGui::Checkbox(u8"���� ���� ���", &pTargetEmitter->m_bEnableDepth);
    if (m_bECEnableDepthOpt)
    {
        ImGui::SameLine();
        ImGui::Checkbox(u8"���� ���� ����", &pTargetEmitter->m_bEnableDepthWrite);
    }

    ImGui::Checkbox(u8"�޸� �ø� ����", &pTargetEmitter->m_bBackCull);

    ImGui::Text(u8"������ ���");
    ImGui::RadioButton(u8"��� ����", &pTargetEmitter->m_eProp.iUseBillBoard, 0);       ImGui::SameLine();
    ImGui::RadioButton(u8"��üȭ�� ������", &pTargetEmitter->m_eProp.iUseBillBoard, 1);  ImGui::SameLine();
    ImGui::RadioButton(u8"Y�� ������", &pTargetEmitter->m_eProp.iUseBillBoard, 2);

    ImGui::Checkbox(u8"���� ����", &pTargetEmitter->m_eProp.bShow);

    ImGui::End();

    ImGui::PopID();
}

void CPEffectWorkspace::showTrailEmitterPropWindow(CPRSParticleTrailEmitter* pTargetEmitter)
{
    ImGui::PushID("EmitterProperty_Trail");

    ImGui::Begin(u8"�̹��� �Ӽ�");

    ImGui::Text(u8"Emitter �̸�");
    ImGui::Text(pTargetEmitter->getSzName()); //CPImGui::InputString("##name1", &m_szCreateEmitterName);

    ImGui::SeparatorText(u8"��ƼŬ �ʱ� Ʈ������");

    ImGui::Text(u8"�̹��� ��ġ");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("X##Pos0", &pTargetEmitter->m_matWorld._41, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f"); ImGui::SameLine();

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("Y##Pos0", &pTargetEmitter->m_matWorld._42, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f"); ImGui::SameLine();

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("Z##Pos0", &pTargetEmitter->m_matWorld._43, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"��ġ ������ �ּҰ�");
    ImGui::DragFloat3("Min##Pos0", (FLOAT*)&pTargetEmitter->m_eProp.vMinPosOffset, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"��ġ ������ �ִ밪");
    ImGui::DragFloat3("Max##Pos0", (FLOAT*)&pTargetEmitter->m_eProp.vMaxPosOffset, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"�ʱ� ȸ�� ����");
    ImGui::DragFloat3("##Rot0", (FLOAT*)&pTargetEmitter->m_eProp.fInitPYR, 1.0f, CPIMGUI_MIN_ITEM_ROTVELOCITY, CPIMGUI_MAX_ITEM_ROTVELOCITY, "%.3f");

    ImGui::Text(u8"ȸ�� ���� ������");
    ImGui::DragFloat3("Min##Rot0", (FLOAT*)&pTargetEmitter->m_eProp.fExtraPYRMinRange, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");
    ImGui::DragFloat3("Max##Rot0", (FLOAT*)&pTargetEmitter->m_eProp.fExtraPYRMaxRange, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"�ʱ� ũ��");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloat2("##Scale0", (FLOAT*)&pTargetEmitter->m_eProp.vInitScale, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.3f");

    ImGui::Text(u8"ũ�� ������(�ּ�/�ִ�)");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("X(Min/Max)##Scale0", &pTargetEmitter->m_eProp.vExtraScaleRange.x, &pTargetEmitter->m_eProp.vExtraScaleRange.z, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.2f");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("Y(Min/Max)##Scale0", &pTargetEmitter->m_eProp.vExtraScaleRange.y, &pTargetEmitter->m_eProp.vExtraScaleRange.w, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.2f");

    ImGui::SeparatorText(u8"��ƼŬ ����");

    ImGui::Text(u8"�ؽ�ó / ��������Ʈ ����");
    ImGui::RadioButton(u8"���� �ؽ�ó##Type", &m_iTexTypeFlag, 0); ImGui::SameLine();
    ImGui::RadioButton(u8"UV �ؽ�ó##Type", &m_iTexTypeFlag, 1); ImGui::SameLine();
    ImGui::RadioButton(u8"��Ƽ �ؽ�ó##Type", &m_iTexTypeFlag, 2);

    if (m_iTexTypeFlag == 0)
    {
        m_pEmitterTexSelector->showUIControl("EffectSel0");

        if (m_pEmitterTexSelector->m_pSelectedTex)
        {
            pTargetEmitter->m_pTexture = m_pEmitterTexSelector->m_pSelectedTex;
            pTargetEmitter->m_pSprite = nullptr;
            pTargetEmitter->m_UVRect.vMin = { 0.0f, 0.0f };
            pTargetEmitter->m_UVRect.vMax = { 1.0f, 1.0f };

            m_iEmitterUVNameIdx = -1;
            m_iEmitterMTNameIdx = -1;
        }
    }

    if (m_iTexTypeFlag == 1)
    {
        ImGui::Text(u8"UV ��������Ʈ");
        if (ImGui::BeginCombo(u8"##Combo0", m_szSelectedEmitterUVName.c_str()))
        {
            for (int i = 0; i < m_szUVNames.size(); i++)
            {
                const bool is_selected = (m_iEmitterUVNameIdx == i);
                if (ImGui::Selectable(m_szUVNames[i].c_str(), is_selected))
                {
                    m_iEmitterUVNameIdx = i;
                    m_szSelectedEmitterUVName = m_szUVNames[m_iEmitterUVNameIdx];
                    m_wszSelectedEmitterUVName = atl_M2W(m_szSelectedEmitterUVName);

                    pTargetEmitter->m_pSprite = CPGC_SPRITE_MGR.getUVPtr(m_wszSelectedEmitterUVName);
                }

                //�޺� �ڽ��� ������ �� ��Ŀ�̵� ��Ҹ� �����ϴ� �κ�
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (m_iEmitterUVNameIdx != -1)
        {
            m_pEmitterTexSelector->selectClear();
        }
    }

    if (m_iTexTypeFlag == 2)
    {
        ImGui::Text(u8"MT ��������Ʈ");
        if (ImGui::BeginCombo(u8"##Combo1", m_szSelectedEmitterMTName.c_str()))
        {
            for (int i = 0; i < m_szMTNames.size(); i++)
            {
                const bool is_selected = (m_iEmitterMTNameIdx == i);
                if (ImGui::Selectable(m_szMTNames[i].c_str(), is_selected))
                {
                    m_iEmitterMTNameIdx = i;
                    m_szSelectedEmitterMTName = m_szMTNames[m_iEmitterMTNameIdx];
                    m_wszSelectedEmitterMTName = atl_M2W(m_szSelectedEmitterMTName);

                    pTargetEmitter->m_pSprite = CPGC_SPRITE_MGR.getMTPtr(m_wszSelectedEmitterMTName);
                    pTargetEmitter->m_UVRect.vMin = { 0.0f, 0.0f };
                    pTargetEmitter->m_UVRect.vMax = { 1.0f, 1.0f };
                }

                //�޺� �ڽ��� ������ �� ��Ŀ�̵� ��Ҹ� �����ϴ� �κ�
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (m_iEmitterMTNameIdx != -1)
        {
            m_pEmitterTexSelector->selectClear();
        }
    }

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragInt(u8"��ƼŬ �ִ� ����##ECbuf0", &pTargetEmitter->m_eProp.iMaxParticleNum, 1.0f, CPIMGUI_MIN_ITEM_COUNT, CPIMGUI_MAX_ITEM_COUNT, "%d");

    if (ImGui::Button(u8"����##modify0"))
    {
        pTargetEmitter->m_vertices.clear();
        pTargetEmitter->m_particles.clear();

        pTargetEmitter->m_vertices.resize(pTargetEmitter->m_eProp.iMaxParticleNum);
        pTargetEmitter->m_particles.resize(pTargetEmitter->m_eProp.iMaxParticleNum);

        pTargetEmitter->m_iVertexCount = pTargetEmitter->m_vertices.size();

        pTargetEmitter->m_pVBuf.ReleaseAndGetAddressOf();

        HRESULT hr = createDXSimpleBuf(m_pDevice, pTargetEmitter->m_iVertexCount * sizeof(CPRS_ParticleVertex), &pTargetEmitter->m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, pTargetEmitter->m_pVBuf.GetAddressOf());
    }

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat(u8"�ʴ� ���� ��##ECbuf0", &pTargetEmitter->m_eProp.fSpawnRate, 0.25f, CPIMGUI_MIN_ITEM_COUNT, CPIMGUI_MAX_ITEM_COUNT, "%.1f");

    if (ImGui::Button(u8"����##modify1"))
    {
        pTargetEmitter->m_fSpawnTime = 1.0f / pTargetEmitter->m_eProp.fSpawnRate;
        pTargetEmitter->m_fTimer = 0.0f;
    }

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::Checkbox(u8"��ƼŬ ���� ������ �ݺ�", &pTargetEmitter->m_eProp.bLoop);

    ImGui::SeparatorText(u8"��ƼŬ �Ӽ�");

    ImGui::Text(u8"����");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("##LifeSpan0", &pTargetEmitter->m_eProp.fLifespanRange.x, &pTargetEmitter->m_eProp.fLifespanRange.y, 0.05f, CPIMGUI_MIN_ITEM_LIFESPAN, CPIMGUI_MAX_ITEM_LIFESPAN, "%.1f");

    if (pTargetEmitter->m_eProp.fLifespanRange.x > pTargetEmitter->m_eProp.fLifespanRange.y)
    {
        pTargetEmitter->m_eProp.fLifespanRange.x = pTargetEmitter->m_eProp.fLifespanRange.y;
    }

    ImGui::Text(u8"�ӵ�");
    ImGui::DragFloat3("Min##Velocity0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMinVelocity, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");
    ImGui::DragFloat3("Max##Velocity0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMaxVelocity, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");

    ImGui::Text(u8"���ӵ�");
    ImGui::DragFloat3("Min##Accel0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMinAcceleration, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");
    ImGui::DragFloat3("Max##Accel0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMaxAcceleration, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");

    ImGui::Text(u8"�ʴ� ȸ�� �ӵ�");
    ImGui::DragFloat3("Min##RotVelocity0", (FLOAT*)&pTargetEmitter->m_eProp.fMinPYRVelocity, 1.0f, CPIMGUI_MIN_ITEM_ROTVELOCITY, CPIMGUI_MAX_ITEM_ROTVELOCITY, "%.1f");
    ImGui::DragFloat3("Max##RotVelocity0", (FLOAT*)&pTargetEmitter->m_eProp.fMaxPYRVelocity, 1.0f, CPIMGUI_MIN_ITEM_ROTVELOCITY, CPIMGUI_MAX_ITEM_ROTVELOCITY, "%.1f");

    ImGui::Text(u8"���� ���� ũ��");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("X(Init/Last)##ScaleOverLifespan0", &pTargetEmitter->m_eProp.fScaleOverLifespan.x, &pTargetEmitter->m_eProp.fScaleOverLifespan.z, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.1f");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("Y(Init/Last)##ScaleOverLifespan0", &pTargetEmitter->m_eProp.fScaleOverLifespan.y, &pTargetEmitter->m_eProp.fScaleOverLifespan.w, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.1f");

    ImGui::Text(u8"���� ���� ����");
    ImGui::ColorEdit4("Init##Color0", (float*)&pTargetEmitter->m_eProp.initColorOverLifespan, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit4("Last##Color0", (float*)&pTargetEmitter->m_eProp.lastColorOverLifespan, ImGuiColorEditFlags_Float);

    ImGui::SeparatorText(u8"���� �Ӽ�");

    //Ʈ�� ������ Push / pop�� ¦�� �¾ƾ� �Ѵ�.
    ImGui::RadioButton(u8"���� �׽�Ʈ", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_MSALPHATEST); ImGui::SameLine();
    ImGui::RadioButton(u8"���� ����", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_ALPHABLEND); ImGui::SameLine();
    ImGui::RadioButton(u8"��� �ҽ� ����", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_DUALSOURCEBLEND);
    ImGui::RadioButton(u8"���� ����", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_ADDITIVEBLEND);

    ImGui::Checkbox(u8"���� ���� ���", &pTargetEmitter->m_bEnableDepth);
    if (m_bECEnableDepthOpt)
    {
        ImGui::SameLine();
        ImGui::Checkbox(u8"���� ���� ����", &pTargetEmitter->m_bEnableDepthWrite);
    }

    ImGui::Checkbox(u8"�޸� �ø� ����", &pTargetEmitter->m_bBackCull);

    ImGui::Text(u8"������ ���");
    ImGui::RadioButton(u8"��� ����", &pTargetEmitter->m_eProp.iUseBillBoard, 0);       ImGui::SameLine();
    ImGui::RadioButton(u8"��üȭ�� ������", &pTargetEmitter->m_eProp.iUseBillBoard, 1);  ImGui::SameLine();
    ImGui::RadioButton(u8"Y�� ������", &pTargetEmitter->m_eProp.iUseBillBoard, 2);

    ImGui::Checkbox(u8"���� ����", &pTargetEmitter->m_eProp.bShow);

    ImGui::End();

    ImGui::PopID();
}

void CPEffectWorkspace::showBurstEmitterPropWindow(CPRSParticleBurstEmitter* pTargetEmitter)
{
    ImGui::PushID("EmitterProperty_Burst");

    ImGui::Begin(u8"�̹��� �Ӽ�");

    ImGui::Text(u8"Emitter �̸�");
    ImGui::Text(pTargetEmitter->getSzName()); //CPImGui::InputString("##name1", &m_szCreateEmitterName);

    ImGui::SeparatorText(u8"��ƼŬ �ʱ� Ʈ������");

    ImGui::Text(u8"�̹��� ��ġ");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("X##Pos0", &pTargetEmitter->m_matWorld._41, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f"); ImGui::SameLine();

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("Y##Pos0", &pTargetEmitter->m_matWorld._42, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f"); ImGui::SameLine();

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragFloat("Z##Pos0", &pTargetEmitter->m_matWorld._43, 1.0f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"��ġ ������ �ּҰ�");
    ImGui::DragFloat3("Min##Pos0", (FLOAT*)&pTargetEmitter->m_eProp.vMinPosOffset, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"��ġ ������ �ִ밪");
    ImGui::DragFloat3("Max##Pos0", (FLOAT*)&pTargetEmitter->m_eProp.vMaxPosOffset, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"�ʱ� ȸ�� ����");
    ImGui::DragFloat3("##Rot0", (FLOAT*)&pTargetEmitter->m_eProp.fInitPYR, 1.0f, CPIMGUI_MIN_ITEM_ROTVELOCITY, CPIMGUI_MAX_ITEM_ROTVELOCITY, "%.3f");

    ImGui::Text(u8"ȸ�� ���� ������");
    ImGui::DragFloat3("Min##Rot0", (FLOAT*)&pTargetEmitter->m_eProp.fExtraPYRMinRange, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");
    ImGui::DragFloat3("Max##Rot0", (FLOAT*)&pTargetEmitter->m_eProp.fExtraPYRMaxRange, 0.05f, CPIMGUI_MIN_ITEM_POS, CPIMGUI_MAX_ITEM_POS, "%.3f");

    ImGui::Text(u8"�ʱ� ũ��");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloat2("##Scale0", (FLOAT*)&pTargetEmitter->m_eProp.vInitScale, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.3f");

    ImGui::Text(u8"ũ�� ������(�ּ�/�ִ�)");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("X(Min/Max)##Scale0", &pTargetEmitter->m_eProp.vExtraScaleRange.x, &pTargetEmitter->m_eProp.vExtraScaleRange.z, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.2f");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("Y(Min/Max)##Scale0", &pTargetEmitter->m_eProp.vExtraScaleRange.y, &pTargetEmitter->m_eProp.vExtraScaleRange.w, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.2f");

    ImGui::SeparatorText(u8"��ƼŬ ����");

    ImGui::Text(u8"�ؽ�ó / ��������Ʈ ����");
    ImGui::RadioButton(u8"���� �ؽ�ó##Type", &m_iTexTypeFlag, 0); ImGui::SameLine();
    ImGui::RadioButton(u8"UV �ؽ�ó##Type", &m_iTexTypeFlag, 1); ImGui::SameLine();
    ImGui::RadioButton(u8"��Ƽ �ؽ�ó##Type", &m_iTexTypeFlag, 2);

    if (m_iTexTypeFlag == 0)
    {
        m_pEmitterTexSelector->showUIControl("EffectSel0");

        if (m_pEmitterTexSelector->m_pSelectedTex)
        {
            pTargetEmitter->m_pTexture = m_pEmitterTexSelector->m_pSelectedTex;
            pTargetEmitter->m_pSprite = nullptr;
            pTargetEmitter->m_UVRect.vMin = { 0.0f, 0.0f };
            pTargetEmitter->m_UVRect.vMax = { 1.0f, 1.0f };

            m_iEmitterUVNameIdx = -1;
            m_iEmitterMTNameIdx = -1;
        }
    }

    if (m_iTexTypeFlag == 1)
    {
        ImGui::Text(u8"UV ��������Ʈ");
        if (ImGui::BeginCombo(u8"##Combo0", m_szSelectedEmitterUVName.c_str()))
        {
            for (int i = 0; i < m_szUVNames.size(); i++)
            {
                const bool is_selected = (m_iEmitterUVNameIdx == i);
                if (ImGui::Selectable(m_szUVNames[i].c_str(), is_selected))
                {
                    m_iEmitterUVNameIdx = i;
                    m_szSelectedEmitterUVName = m_szUVNames[m_iEmitterUVNameIdx];
                    m_wszSelectedEmitterUVName = atl_M2W(m_szSelectedEmitterUVName);

                    pTargetEmitter->m_pSprite = CPGC_SPRITE_MGR.getUVPtr(m_wszSelectedEmitterUVName);
                }

                //�޺� �ڽ��� ������ �� ��Ŀ�̵� ��Ҹ� �����ϴ� �κ�
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (m_iEmitterUVNameIdx != -1)
        {
            m_pEmitterTexSelector->selectClear();
        }
    }

    if (m_iTexTypeFlag == 2)
    {
        ImGui::Text(u8"MT ��������Ʈ");
        if (ImGui::BeginCombo(u8"##Combo1", m_szSelectedEmitterMTName.c_str()))
        {
            for (int i = 0; i < m_szMTNames.size(); i++)
            {
                const bool is_selected = (m_iEmitterMTNameIdx == i);
                if (ImGui::Selectable(m_szMTNames[i].c_str(), is_selected))
                {
                    m_iEmitterMTNameIdx = i;
                    m_szSelectedEmitterMTName = m_szMTNames[m_iEmitterMTNameIdx];
                    m_wszSelectedEmitterMTName = atl_M2W(m_szSelectedEmitterMTName);

                    pTargetEmitter->m_pSprite = CPGC_SPRITE_MGR.getMTPtr(m_wszSelectedEmitterMTName);
                    pTargetEmitter->m_UVRect.vMin = { 0.0f, 0.0f };
                    pTargetEmitter->m_UVRect.vMax = { 1.0f, 1.0f };
                }

                //�޺� �ڽ��� ������ �� ��Ŀ�̵� ��Ҹ� �����ϴ� �κ�
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (m_iEmitterMTNameIdx != -1)
        {
            m_pEmitterTexSelector->selectClear();
        }
    }

    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
    ImGui::DragInt(u8"��ƼŬ �ִ� ����##ECbuf0", &pTargetEmitter->m_eProp.iMaxParticleNum, 1.0f, CPIMGUI_MIN_ITEM_COUNT, CPIMGUI_MAX_ITEM_COUNT, "%d");

    if (ImGui::Button(u8"����##modify0"))
    {
        pTargetEmitter->m_vertices.clear();
        pTargetEmitter->m_particles.clear();

        pTargetEmitter->m_vertices.resize(pTargetEmitter->m_eProp.iMaxParticleNum);
        pTargetEmitter->m_particles.resize(pTargetEmitter->m_eProp.iMaxParticleNum);

        pTargetEmitter->m_iVertexCount = pTargetEmitter->m_vertices.size();

        pTargetEmitter->m_pVBuf.ReleaseAndGetAddressOf();

        HRESULT hr = createDXSimpleBuf(m_pDevice, pTargetEmitter->m_iVertexCount * sizeof(CPRS_ParticleVertex), &pTargetEmitter->m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, pTargetEmitter->m_pVBuf.GetAddressOf());
    }

    if (ImGui::CollapsingHeader(u8"����Ʈ �ֱ�"))
    {
        int pendingDelete = -1;

        static std::string nodeName;

        for (int idx = 0; idx < pTargetEmitter->m_eProp.iAnimKeyCnt; idx++)
        {
            ImGui::PushID(idx);

            ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET);
            nodeName = "Cycle " + std::to_string(idx);
            ImGui::DragFloat(nodeName.c_str(), &pTargetEmitter->m_eProp.animkeyList[idx].fTime, 0.01f, CPCPIMGUI_MIN_ANIM_TIME, CPCPIMGUI_MAX_ANIM_TIME);

            if (idx)
            {
                if (pTargetEmitter->m_eProp.animkeyList[idx - 1].fTime > pTargetEmitter->m_eProp.animkeyList[idx].fTime - pTargetEmitter->m_eProp.fLifespanRange.y)
                {
                    pTargetEmitter->m_eProp.animkeyList[idx].fTime = pTargetEmitter->m_eProp.animkeyList[idx - 1].fTime + pTargetEmitter->m_eProp.fLifespanRange.y;
                }
            }

            ImGui::SameLine(); 
            if (ImGui::Button(u8"����")) 
            { 
                pendingDelete = idx; 
            }

            ImGui::PopID();
        }

        if (pTargetEmitter->m_eProp.iAnimKeyCnt < EFFECT_MAX_ANIMKEY - 1)
        {
            if (ImGui::Button(u8"�߰�"))
            {
                if (pTargetEmitter->m_eProp.iAnimKeyCnt == 0)
                {
                    pTargetEmitter->m_eProp.animkeyList[pTargetEmitter->m_eProp.iAnimKeyCnt].fTime = 0.0f;
                }
                else
                {
                    pTargetEmitter->m_eProp.animkeyList[pTargetEmitter->m_eProp.iAnimKeyCnt].fTime =
                        pTargetEmitter->m_eProp.animkeyList[pTargetEmitter->m_eProp.iAnimKeyCnt - 1].fTime + pTargetEmitter->m_eProp.fLifespanRange.y;
                }

                pTargetEmitter->m_eProp.iAnimKeyCnt++;
            }
        }

        if (pendingDelete != -1)
        {
            for(int idx = pendingDelete;  idx < EFFECT_MAX_ANIMKEY - 1; idx++)
            { 
                pTargetEmitter->m_eProp.animkeyList[idx].fTime =
                    pTargetEmitter->m_eProp.animkeyList[idx + 1].fTime;
            }

            pTargetEmitter->m_eProp.iAnimKeyCnt--;
        }
    }

    ImGui::Checkbox(u8"�ֱ� ������ �ݺ�", &pTargetEmitter->m_eProp.bLoop);

    ImGui::SeparatorText(u8"��ƼŬ �Ӽ�");

    ImGui::Text(u8"����");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("##LifeSpan0", &pTargetEmitter->m_eProp.fLifespanRange.x, &pTargetEmitter->m_eProp.fLifespanRange.y, 0.05f, CPIMGUI_MIN_ITEM_LIFESPAN, CPIMGUI_MAX_ITEM_LIFESPAN, "%.1f");

    if (pTargetEmitter->m_eProp.fLifespanRange.x > pTargetEmitter->m_eProp.fLifespanRange.y)
    {
        pTargetEmitter->m_eProp.fLifespanRange.x = pTargetEmitter->m_eProp.fLifespanRange.y;
    }

    ImGui::Text(u8"�ӵ�");
    ImGui::DragFloat3("Min##Velocity0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMinVelocity, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");
    ImGui::DragFloat3("Max##Velocity0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMaxVelocity, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");

    ImGui::Text(u8"���ӵ�");
    ImGui::DragFloat3("Min##Accel0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMinAcceleration, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");
    ImGui::DragFloat3("Max##Accel0", (FLOAT*)&pTargetEmitter->m_eProp.vInitMaxAcceleration, 0.05f, CPIMGUI_MIN_ITEM_VELOCITY, CPIMGUI_MAX_ITEM_VELOCITY, "%.3f");

    ImGui::Text(u8"�ʴ� ȸ�� �ӵ�");
    ImGui::DragFloat3("Min##RotVelocity0", (FLOAT*)&pTargetEmitter->m_eProp.fMinPYRVelocity, 1.0f, CPIMGUI_MIN_ITEM_ROTVELOCITY, CPIMGUI_MAX_ITEM_ROTVELOCITY, "%.1f");
    ImGui::DragFloat3("Max##RotVelocity0", (FLOAT*)&pTargetEmitter->m_eProp.fMaxPYRVelocity, 1.0f, CPIMGUI_MIN_ITEM_ROTVELOCITY, CPIMGUI_MAX_ITEM_ROTVELOCITY, "%.1f");

    ImGui::Text(u8"���� ���� ũ��");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("X(Init/Last)##ScaleOverLifespan0", &pTargetEmitter->m_eProp.fScaleOverLifespan.x, &pTargetEmitter->m_eProp.fScaleOverLifespan.z, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.1f");
    ImGui::SetNextItemWidth(CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET * 2);
    ImGui::DragFloatRange2("Y(Init/Last)##ScaleOverLifespan0", &pTargetEmitter->m_eProp.fScaleOverLifespan.y, &pTargetEmitter->m_eProp.fScaleOverLifespan.w, 1.0f, CPIMGUI_MIN_ITEM_SCALE, CPIMGUI_MAX_ITEM_SCALE, "%.1f");

    ImGui::Text(u8"���� ���� ����");
    ImGui::ColorEdit4("Init##Color0", (float*)&pTargetEmitter->m_eProp.initColorOverLifespan, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit4("Last##Color0", (float*)&pTargetEmitter->m_eProp.lastColorOverLifespan, ImGuiColorEditFlags_Float);

    ImGui::SeparatorText(u8"���� �Ӽ�");

    //Ʈ�� ������ Push / pop�� ¦�� �¾ƾ� �Ѵ�.
    ImGui::RadioButton(u8"���� �׽�Ʈ", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_MSALPHATEST); ImGui::SameLine();
    ImGui::RadioButton(u8"���� ����", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_ALPHABLEND); ImGui::SameLine();
    ImGui::RadioButton(u8"��� �ҽ� ����", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_DUALSOURCEBLEND);
    ImGui::RadioButton(u8"���� ����", (int*)&pTargetEmitter->m_iBlendStateOption, CPRS_BSType::BS_ADDITIVEBLEND);

    ImGui::Checkbox(u8"���� ���� ���", &pTargetEmitter->m_bEnableDepth);
    if (m_bECEnableDepthOpt)
    {
        ImGui::SameLine();
        ImGui::Checkbox(u8"���� ���� ����", &pTargetEmitter->m_bEnableDepthWrite);
    }

    ImGui::Checkbox(u8"�޸� �ø� ����", &pTargetEmitter->m_bBackCull);

    ImGui::Text(u8"������ ���");
    ImGui::RadioButton(u8"��� ����", &pTargetEmitter->m_eProp.iUseBillBoard, 0);       ImGui::SameLine();
    ImGui::RadioButton(u8"��üȭ�� ������", &pTargetEmitter->m_eProp.iUseBillBoard, 1);  ImGui::SameLine();
    ImGui::RadioButton(u8"Y�� ������", &pTargetEmitter->m_eProp.iUseBillBoard, 2);

    ImGui::Checkbox(u8"���� ����", &pTargetEmitter->m_eProp.bShow);

    ImGui::End();

    ImGui::PopID();
}

void CPEffectWorkspace::createNewParticleSystemPopup(int& popOpt, bool& bOpenWorkspace)
{
    ImGui::PushID("EFFECT_CREATE");

    ImGui::OpenPopup(u8"���� ����");

    //�׻� ������� �����ϰ� �Ѵ�.
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(u8"���� ����", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SeparatorText(u8"��ƼŬ �ý��� �̸�");
        static std::string tempName;
        CPImGui::InputString("##Name0", &tempName);

        if (ImGui::Button(u8"����") && tempName.size())
        {
            if (m_pCurrentPSystem)
            {
                m_pCurrentPSystem->release();
                delete m_pCurrentPSystem;
                m_pCurrentPSystem = nullptr;

                m_pSelectedEmitter = nullptr; 
                m_selectedEmitterIdx = -1;
                m_emitterSelectionMask = 0;
            }

            m_pCurrentPSystem = new CPGCParticleSystem;
            bool bRet = m_pCurrentPSystem->create(atl_M2W(tempName), m_pDevice, m_pDContext);
            if (bRet)
            {
                tempName.clear();

                popOpt = 0;
                bOpenWorkspace = true;
                ImGui::CloseCurrentPopup();
            }
            else
            {
                m_pCurrentPSystem->release();
                delete m_pCurrentPSystem;
                m_pCurrentPSystem = nullptr;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button(u8"�ݱ�"))
        {
            popOpt = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void CPEffectWorkspace::loadParticleSystemPopup(int& popOpt, bool& bOpenWorkspace, int loadOpt)
{
    ImGui::PushID("EFFECT_LOAD");

    //�׻� ������� �����ϰ� �Ѵ�.
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", u8"���� �ҷ�����", ".PSystem", DEFAULT_EFFECT_DIRECTORY_SZPATH, 1, nullptr, ImGuiFileDialogFlags_Modal);

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            bool bRet = true;

            bRet = loadPSBinaryFile(atl_M2W(filePathName), loadOpt);

            if (bRet)
            {
                popOpt = 0;
                bOpenWorkspace = true;
                ImGui::CloseCurrentPopup();

                m_selectedEmitterIdx = -1;
                m_emitterSelectionMask = 0;
                m_pSelectedEmitter = nullptr;
                m_pEmitterTexSelector->selectClear();
            }
        }
        else
        {
            popOpt = 0;
            ImGui::CloseCurrentPopup();
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::PopID();
}

void CPEffectWorkspace::saveParticleSystemPopup(int& popOpt)
{
    ImGui::PushID("EFFECT_SAVE");

    //�׻� ������� �����ϰ� �Ѵ�.
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", u8"���� �����ϱ�", ".PSystem", DEFAULT_EFFECT_DIRECTORY_SZPATH, 1, nullptr, ImGuiFileDialogFlags_Modal);

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            bool bRet = true;

            bRet = savePSBinaryFile(atl_M2W(filePathName));

            if (bRet)
            {
                popOpt = 0;
                ImGui::CloseCurrentPopup();
            }
        }
        else
        {
            popOpt = 0;
            ImGui::CloseCurrentPopup();
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::PopID();
}

bool CPEffectWorkspace::loadPSBinaryFile(std::wstring wszPSystemFilepath, int loadOpt)
{
    if (m_pCurrentPSystem)
    {
        m_pCurrentPSystem->release();
        delete m_pCurrentPSystem;
        m_pCurrentPSystem = nullptr;
    }

    //���� �ε� �� ������ ��
    bool bRet = true;
    CPRS_PSYSTEM_FILESTRUCT* pLoadFile = new CPRS_PSYSTEM_FILESTRUCT;
    ZeroMemory(pLoadFile, sizeof(pLoadFile));

    if (loadOpt == 0)
    {
        //�ֽ� ���� ���� �ε�
        bRet = CPGC_FILEIO_MGR.loadFile(wszPSystemFilepath, pLoadFile);
    }
    else
    {
        //���� ���� ���� �ε�
        CPRS_PSYSTEM_PREVVERLOADFILESTRUCT* pPrevFile = new CPRS_PSYSTEM_PREVVERLOADFILESTRUCT;
        ZeroMemory(pPrevFile, sizeof(CPRS_PSYSTEM_PREVVERLOADFILESTRUCT));

        bRet = CPGC_FILEIO_MGR.loadFile(wszPSystemFilepath, pPrevFile);

        if (!bRet) 
        { 
            delete pLoadFile;
            delete pPrevFile;
            return false; 
        }

        bRet = convertFile(*pPrevFile, *pLoadFile);

        delete pPrevFile;
    }

    //��ƼŬ �ý��� ������
    if (bRet)
    {
        m_pCurrentPSystem = new CPGCParticleSystem;

        m_pCurrentPSystem->create(pLoadFile->wszPSystemName, m_pDevice, m_pDContext);

        for (int i = 0; i < pLoadFile->iEmiiterCnt; i++)
        {
            if (pLoadFile->emitters[i].wszEmitterName[0] != NULL)
            {
                CPRSEmitter* newE = nullptr;


                switch (pLoadFile->emitters[i].emitterType)
                {
                    case CPRS_EMITTER_TYPE::DEFAULT_PARTICLE:
                    {
                        newE = new CPRSPointParticleEmitter;
                    } break;

                    case CPRS_EMITTER_TYPE::PARTICLE_TRAIL:
                    {
                        newE = new CPRSParticleTrailEmitter;
                    }break;

                    case CPRS_EMITTER_TYPE::BURST_PARTICLE:
                    {
                        newE = new CPRSParticleBurstEmitter;
                    } break;

                    default:
                        newE = new CPRSPointParticleEmitter;
                }

                //�̹��� �̸�
                newE->setName(pLoadFile->emitters[i].wszEmitterName);

                //���� ����̽� ����
                newE->setDevice(m_pDevice, m_pDContext);

                //��������Ʈ ����
                if (pLoadFile->emitters[i].wszSpriteName[0] != NULL)
                {
                    if (pLoadFile->emitters[i].iSpriteType == CPRS_SPRITE_TYPE::SPRITE_UV)
                    {
                        //UV ��������Ʈ
                        if (!CPGC_SPRITE_MGR.getUVPtr(pLoadFile->emitters[i].wszSpriteName))
                        {
                            newE->release();
                            delete newE;
                            newE = nullptr;

                            m_pCurrentPSystem->release();
                            delete m_pCurrentPSystem;
                            m_pCurrentPSystem = nullptr;

                            delete pLoadFile;
                            return false;
                        }

                        newE->createUVSpriteEmitter(pLoadFile->emitters[i].eProp,
                            pLoadFile->emitters[i].wszSpriteName);
                    }
                    else
                    {
                        //MT ��������Ʈ
                        if (!CPGC_SPRITE_MGR.getMTPtr(pLoadFile->emitters[i].wszSpriteName))
                        {
                            newE->release();
                            delete newE;
                            newE = nullptr;

                            m_pCurrentPSystem->release();
                            delete m_pCurrentPSystem;
                            m_pCurrentPSystem = nullptr;

                            delete pLoadFile;
                            return false;
                        }

                        newE->createMTSpriteEmitter(pLoadFile->emitters[i].eProp,
                            pLoadFile->emitters[i].wszSpriteName);
                    }

                    //���� �ɼ� ����
                    newE->setRenderOption(pLoadFile->emitters[i].iBlendStateOption,
                        pLoadFile->emitters[i].bEnableDepth,
                        pLoadFile->emitters[i].bEnableDepthWrite,
                        pLoadFile->emitters[i].bBackCull);

                    //�̹��� �ʱ�ȭ
                    newE->init();

                    //�̹��� ����Ʈ�� �߰�
                    m_pCurrentPSystem->m_pEmitterList.push_back(newE);
                }
                else
                {
                    //���� �ؽ�ó�� ���
                    CPGC_TEXTURE_MGR.createTexture(pLoadFile->emitters[i].wszTextureName, 
                        pLoadFile->emitters[i].wszTexturePath);

                    newE->createEmitter(pLoadFile->emitters[i].eProp,
                        pLoadFile->emitters[i].wszTextureName);

                    newE->setRenderOption(pLoadFile->emitters[i].iBlendStateOption,
                        pLoadFile->emitters[i].bEnableDepth,
                        pLoadFile->emitters[i].bEnableDepthWrite,
                        pLoadFile->emitters[i].bBackCull);

                    newE->init();

                    m_pCurrentPSystem->m_pEmitterList.push_back(newE);
                }
            }
        }
    }

    if (pLoadFile) { delete pLoadFile; }

    return bRet;
}

bool CPEffectWorkspace::savePSBinaryFile(std::wstring wszPSystemFilepath)
{
    if (m_pCurrentPSystem)
    {
        CPRS_PSYSTEM_FILESTRUCT* pSaveFile = new CPRS_PSYSTEM_FILESTRUCT;
        ZeroMemory(pSaveFile, sizeof(CPRS_PSYSTEM_FILESTRUCT));

        std::wstring strBuf = m_pCurrentPSystem->getWszName();
        memcpy(pSaveFile->wszPSystemName, strBuf.data(), min(CPIMGUI_PATH_WSTRING_BYTE_LIMIT, sizeof(WCHAR) * strBuf.size()));

        pSaveFile->iEmiiterCnt = m_pCurrentPSystem->m_pEmitterList.size();

        int cnt = 0;

        for (auto it : m_pCurrentPSystem->m_pEmitterList)
        {
            strBuf = it->getWszName();
            memcpy(pSaveFile->emitters[cnt].wszEmitterName, strBuf.data(), min(CPIMGUI_PATH_WSTRING_BYTE_LIMIT, sizeof(WCHAR) * strBuf.size()));

            pSaveFile->emitters[cnt].emitterType = it->m_emitterType;

            if (it->m_pSprite)
            {
                //��������Ʈ �̸�
                strBuf = it->m_pSprite->m_wszName;
                memcpy(pSaveFile->emitters[cnt].wszSpriteName, strBuf.data(), min(CPIMGUI_PATH_WSTRING_BYTE_LIMIT, sizeof(WCHAR) * strBuf.size()));

                //��������Ʈ ���
                strBuf = it->m_pSprite->m_wszTexFilePath;
                memcpy(pSaveFile->emitters[cnt].wszSpritePath, strBuf.data(), min(CPIMGUI_PATH_WSTRING_BYTE_LIMIT, sizeof(WCHAR) * strBuf.size()));

                //��������Ʈ Ÿ��
                pSaveFile->emitters[cnt].iSpriteType = it->m_pSprite->m_spriteInfo.m_spriteType;

                //UV ��������Ʈ �ళ��
                pSaveFile->emitters[cnt].spriteRC[0] = it->m_pSprite->m_spriteInfo.m_iRows;

                //UV ��������Ʈ ������
                pSaveFile->emitters[cnt].spriteRC[1] = it->m_pSprite->m_spriteInfo.m_iCols;
            }

            //���� �ؽ�ó / UV��������Ʈ �ؽ�ó�� ���� �ؽ�ó �̸�
            strBuf = it->m_pTexture->getName();
            memcpy(pSaveFile->emitters[cnt].wszTextureName, strBuf.data(), min(CPIMGUI_PATH_WSTRING_BYTE_LIMIT, sizeof(WCHAR) * strBuf.size()));

            //���� �ؽ�ó / UV��������Ʈ �ؽ�ó�� ���� �ؽ�ó ���
            strBuf = it->m_pTexture->getFilePath();
            memcpy(pSaveFile->emitters[cnt].wszTexturePath, strBuf.data(), min(CPIMGUI_PATH_WSTRING_BYTE_LIMIT, sizeof(WCHAR) * strBuf.size()));

            //�̹��� ���� �ʱ� ��ġ
            pSaveFile->emitters[cnt].vEmitterPos = it->m_vPos;

            //�̹��� �Ӽ�
            pSaveFile->emitters[cnt].eProp = it->m_eProp;

            //BS State
            pSaveFile->emitters[cnt].iBlendStateOption = it->m_iBlendStateOption;

            //DSS State
            pSaveFile->emitters[cnt].bEnableDepth = it->m_bEnableDepth;
            pSaveFile->emitters[cnt].bEnableDepthWrite = it->m_bEnableDepthWrite;

            //RS State
            pSaveFile->emitters[cnt].bBackCull = it->m_bBackCull;

            cnt++;
        }

        bool bRet = CPGC_FILEIO_MGR.saveFile(wszPSystemFilepath, pSaveFile, sizeof(CPRS_PSYSTEM_FILESTRUCT));

        if (pSaveFile) { delete pSaveFile; }

        return bRet;
    }
    else { return false; }

    return true;
}

bool CPEffectWorkspace::convertFile(const CPRS_PSYSTEM_PREVVERLOADFILESTRUCT& prevfile, CPRS_PSYSTEM_FILESTRUCT& outFile)
{
    //���� ������ ��
    memcpy(outFile.wszPSystemName, prevfile.wszPSystemName, CPIMGUI_PATH_WSTRING_BYTE_LIMIT);
    outFile.iEmiiterCnt = prevfile.iEmiiterCnt;

    for (int i = 0; i < prevfile.iEmiiterCnt; i++)
    {
        if (prevfile.emitters[i].wszEmitterName[0] != NULL)
        {
            /////////////////////////////////////
            //  �̹��� ���� ���ҽ� �ɼ�
            /////////////////////////////////////

            outFile.emitters[i].emitterType = prevfile.emitters[i].emitterType;

            memcpy(outFile.emitters[i].wszEmitterName, prevfile.emitters[i].wszEmitterName, CPIMGUI_PATH_WSTRING_BYTE_LIMIT);
            memcpy(outFile.emitters[i].wszSpriteName, prevfile.emitters[i].wszSpriteName, CPIMGUI_PATH_WSTRING_BYTE_LIMIT);
            memcpy(outFile.emitters[i].wszSpritePath, prevfile.emitters[i].wszSpritePath, CPIMGUI_PATH_WSTRING_BYTE_LIMIT);
            memcpy(outFile.emitters[i].wszTextureName, prevfile.emitters[i].wszTextureName, CPIMGUI_PATH_WSTRING_BYTE_LIMIT);
            memcpy(outFile.emitters[i].wszTexturePath, prevfile.emitters[i].wszTexturePath, CPIMGUI_PATH_WSTRING_BYTE_LIMIT);

            outFile.emitters[i].vEmitterPos = prevfile.emitters[i].vEmitterPos;

            outFile.emitters[i].iSpriteType = prevfile.emitters[i].iSpriteType;

            outFile.emitters[i].spriteRC[0] = prevfile.emitters[i].spriteRC[0];
            outFile.emitters[i].spriteRC[1] = prevfile.emitters[i].spriteRC[1];

            outFile.emitters[i].iBlendStateOption = prevfile.emitters[i].iBlendStateOption;
            outFile.emitters[i].bEnableDepth = prevfile.emitters[i].bEnableDepth;
            outFile.emitters[i].bEnableDepthWrite = prevfile.emitters[i].bEnableDepthWrite;
            outFile.emitters[i].bBackCull = prevfile.emitters[i].bBackCull;

            /////////////////////////////////////
            //�̹��� Ư�� �ɼ�
            /////////////////////////////////////
            convertEmitterProp(prevfile.emitters[i].eProp, outFile.emitters[i].eProp);
        }
        else break;
    }

    return true;
}

void CPEffectWorkspace::convertEmitterProp(const CPRS_DeprecatedEmitterProperty& in, CPRS_EmitterProperty& out)
{
    ////////////////////////////
    // ���� ���� ���� �̹��� �ɼ�
    ////////////////////////////
    out.vInitPos = in.vInitPos;
    out.vMinPosOffset = in.vMinPosOffset;
    out.vMaxPosOffset = in.vMaxPosOffset;

    out.fInitPYR = in.fInitPYR;

    out.fExtraPYRMinRange = in.fExtraPYRMinRange;
    out.fExtraPYRMaxRange = in.fExtraPYRMaxRange;

    out.vInitScale = in.vInitScale;
    out.vExtraScaleRange = in.vExtraScaleRange;

    out.iMaxParticleNum = in.iMaxParticleNum;
    out.fSpawnRate = in.fSpawnRate;
    out.fLifespanRange = in.fLifespanRange;

    out.vInitMinVelocity = in.vInitMinVelocity;
    out.vInitMaxVelocity = in.vInitMaxVelocity;

    out.fMinPYRVelocity = in.fMinPYRVelocity;
    out.fMaxPYRVelocity = in.fMaxPYRVelocity;

    out.fScaleOverLifespan = in.fScaleOverLifespan;

    out.vInitMinAcceleration = in.vInitMinAcceleration;
    out.vInitMaxAcceleration = in.vInitMaxAcceleration;

    out.initColorOverLifespan = in.initColorOverLifespan;
    out.lastColorOverLifespan = in.lastColorOverLifespan;

    out.bShow = in.bShow;

    out.iUseBillBoard = in.iUseBillBoard;

    out.bLoop = in.bLoop;

    //////////////////////////
    // �߰��� �̹��� �ɼ�
    //////////////////////////
    
    out.iAnimKeyCnt = in.iAnimKeyCnt;
    memcpy(out.animkeyList, in.animkeyList, sizeof(CPRSParticleAnimKey) * in.iAnimKeyCnt);

    //////////////////////////
    // �̹��� ������ �ΰ� �ɼ�
    //////////////////////////
}

bool CPEffectWorkspace::convertDirectory(std::wstring targetDirPath, std::wstring outDirPath)
{
    std::vector<std::wstring> exts;
    exts.push_back(L".PSystem");

    std::vector<std::wstring> paths;
    std::vector<std::wstring> names;

    CPRS_PSYSTEM_PREVVERLOADFILESTRUCT* pInFile = new CPRS_PSYSTEM_PREVVERLOADFILESTRUCT;
    CPRS_PSYSTEM_FILESTRUCT* pOutFile = new CPRS_PSYSTEM_FILESTRUCT;

    bool bRet = loadDir(targetDirPath, &exts, &paths, &names);

    if (bRet)
    {
        for (int i = 0; i < paths.size(); i++)
        {
            ZeroMemory(pInFile, sizeof(pInFile));
            ZeroMemory(pOutFile, sizeof(pOutFile));

            bRet = CPGC_FILEIO_MGR.loadFile(paths[i], pInFile);

            convertFile(*pInFile, *pOutFile);

            bRet = CPGC_FILEIO_MGR.saveFile(outDirPath + names[i], pOutFile, sizeof(CPRS_PSYSTEM_FILESTRUCT));
        }
    }

    if (pInFile) { delete pInFile; }
    if (pOutFile) { delete pOutFile; }

    return true;
}

HRESULT CPEffectWorkspace::createDefaultEmitter()
{
    HRESULT hr = S_OK;

    //�̹��� ������
    CPRSPointParticleEmitter* newE = new CPRSPointParticleEmitter;
    newE->setName(atl_M2W(m_szCreateEmitterName));

    newE->setDevice(m_pDevice, m_pDContext);
    newE->setDevice(CPGC_DXMODULE.getDevice(), CPGC_DXMODULE.getImDContext());

    if (m_iCreateFlag == 0)
    {
        hr = newE->createEmitter(m_ECProp,
            m_pEffectTexSelector->m_wszSelectedTexName);

        if (FAILED(hr))
        {
            newE->release();
            delete newE;
            newE = nullptr;
            return hr;
        }
    }
    else if (m_iCreateFlag == 1)
    {
        hr = newE->createUVSpriteEmitter(m_ECProp,
            m_wszSelectedUVName);

        if (FAILED(hr))
        {
            newE->release();
            delete newE;
            newE = nullptr;
            return hr;
        }
    }
    else if (m_iCreateFlag == 2)
    {
        hr = newE->createMTSpriteEmitter(m_ECProp,
            m_wszSelectedMTName);

        if (FAILED(hr))
        {
            newE->release();
            delete newE;
            newE = nullptr;
            return hr;
        }
    }

    newE->setRenderOption(m_iECBlendOpt, m_bECEnableDepthOpt, m_bECEnableDepthWriteOpt, m_bECEnableBackCullOpt);

    newE->init();

    m_pCurrentPSystem->m_pEmitterList.push_back(newE);

    return hr;
}

HRESULT CPEffectWorkspace::createTrailEmitter()
{
    HRESULT hr = S_OK;

    //�̹��� ������
    CPRSParticleTrailEmitter* newE = new CPRSParticleTrailEmitter;
    newE->setName(atl_M2W(m_szCreateEmitterName));

    newE->setDevice(m_pDevice, m_pDContext);
    newE->setDevice(CPGC_DXMODULE.getDevice(), CPGC_DXMODULE.getImDContext());

    if (m_iCreateFlag == 0)
    {
        hr = newE->createEmitter(m_ECProp,
            m_pEffectTexSelector->m_wszSelectedTexName);

        if (FAILED(hr))
        {
            newE->release();
            delete newE;
            newE = nullptr;
            return hr;
        }
    }
    else if (m_iCreateFlag == 1)
    {
        hr = newE->createUVSpriteEmitter(m_ECProp,
            m_wszSelectedUVName);

        if (FAILED(hr))
        {
            newE->release();
            delete newE;
            newE = nullptr;
            return hr;
        }
    }
    else if (m_iCreateFlag == 2)
    {
        hr = newE->createMTSpriteEmitter(m_ECProp,
            m_wszSelectedMTName);

        if (FAILED(hr))
        {
            newE->release();
            delete newE;
            newE = nullptr;
            return hr;
        }
    }

    newE->setRenderOption(m_iECBlendOpt, m_bECEnableDepthOpt, m_bECEnableDepthWriteOpt, m_bECEnableBackCullOpt);

    newE->init();

    m_pCurrentPSystem->m_pEmitterList.push_back(newE);

    return hr;
}

HRESULT CPEffectWorkspace::createBurstEmitter()
{
    HRESULT hr = S_OK;

    //�̹��� ������
    CPRSParticleBurstEmitter* newE = new CPRSParticleBurstEmitter;
    newE->setName(atl_M2W(m_szCreateEmitterName));

    newE->setDevice(m_pDevice, m_pDContext);
    newE->setDevice(CPGC_DXMODULE.getDevice(), CPGC_DXMODULE.getImDContext());

    if (m_iCreateFlag == 0)
    {
        hr = newE->createEmitter(m_ECProp,
            m_pEffectTexSelector->m_wszSelectedTexName);

        if (FAILED(hr))
        {
            newE->release();
            delete newE;
            newE = nullptr;
            return hr;
        }
    }
    else if (m_iCreateFlag == 1)
    {
        hr = newE->createUVSpriteEmitter(m_ECProp,
            m_wszSelectedUVName);

        if (FAILED(hr))
        {
            newE->release();
            delete newE;
            newE = nullptr;
            return hr;
        }
    }
    else if (m_iCreateFlag == 2)
    {
        hr = newE->createMTSpriteEmitter(m_ECProp,
            m_wszSelectedMTName);

        if (FAILED(hr))
        {
            newE->release();
            delete newE;
            newE = nullptr;
            return hr;
        }
    }

    newE->setRenderOption(m_iECBlendOpt, m_bECEnableDepthOpt, m_bECEnableDepthWriteOpt, m_bECEnableBackCullOpt);

    newE->init();

    m_pCurrentPSystem->m_pEmitterList.push_back(newE);

    return hr;
}

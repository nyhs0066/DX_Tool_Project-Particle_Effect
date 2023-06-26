#include "CPGCEffect.h"

CPGCParticleEmitter::CPGCParticleEmitter()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;

	m_fSpawnTime = 0.0f;
	m_fTimer = 0.0f;

	m_pEProp = nullptr;

	m_iVertexCount = 0;;
	m_iStride = 0;
	m_iOffset = 0;

	m_pVLayout = nullptr;

	m_playSpeed = 1.0f;

	m_pSprite = nullptr;
	m_pTexture = nullptr;

	m_pRState = nullptr;
	m_pBState = nullptr;
	m_DSState = nullptr;

	m_UVRect = { {0.0f, 0.0f}, {1.0f, 1.0f} };
}

CPGCParticleEmitter::~CPGCParticleEmitter()
{
}

bool CPGCParticleEmitter::init()
{
	return true;
}

bool CPGCParticleEmitter::update()
{
	float dt = CPGC_MAINTIMER.getOneFrameTimeF() * m_playSpeed;

	m_fTimer += dt;

	activateTarget(dt);

	for (UINT i = 0; i < m_iVertexCount; i++)
	{
		m_particles[i].fElapsedTime += dt;

		if (m_particles[i].fElapsedTime > m_particles[i].fLifeSpan)
		{
			m_particles[i].bEnable = false;

			//정점의 스케일 속성의 마지막 성분을 활성화 여부로 사용한다.
			m_vertices[i].scale.z = 0.0f;
		}

		if (m_particles[i].fElapsedTime <= m_particles[i].fLifeSpan)
		{
			updateParticleState(i, dt);
		}
	}

	updateBuffer(m_pVBuf.Get(), m_vertices.data(), m_vertices.size() * sizeof(CPRS_ParticleVertex));

	return true;
}

bool CPGCParticleEmitter::render()
{
	if (m_pEProp->bShow)
	{
		bindToPipeline();

		m_pDContext->Draw(m_iVertexCount, 0);
	}

	return true;
}

bool CPGCParticleEmitter::release()
{
	return true;
}

void CPGCParticleEmitter::bindToPipeline()
{
	m_pDContext->IASetVertexBuffers(0, 1, m_pVBuf.GetAddressOf(), &m_iStride, &m_iOffset);
	m_pDContext->IASetInputLayout(m_pVLayout);

	//점 입자를 GS에서 늘릴 것이므로 IA스테이지에서의 입력 기하요소는 POINTLIST로 설정한다.
	m_pDContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	if (m_pTexture)
	{
		ID3D11ShaderResourceView* pTemp = m_pTexture->getSRV();
		m_pDContext->PSSetShaderResources(0, 1, &pTemp);
	}

	m_pDContext->OMSetBlendState(m_pBState, NULL, -1);

	m_shaderGroup.bindToPipeline(m_pDContext);

	m_pDContext->PSSetSamplers(0, m_pSamplers.size(), m_pSamplers.data());
	m_pDContext->RSSetState(m_pRState);
	m_pDContext->OMSetDepthStencilState(m_DSState, 1);
}

void CPGCParticleEmitter::setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	m_pDevice = pDevice;
	m_pDContext = pDContext;
}

HRESULT CPGCParticleEmitter::createEmitter(CPRS_EmitterProperty* eProp, std::wstring wszTexPathName)
{
	HRESULT hr = S_OK;

	m_pEProp = eProp;

	m_fSpawnTime = 1.0f / m_pEProp->fSpawnRate;
	m_fTimer = 0.0f;

	m_pTexture = CPGC_TEXTURE_MGR.getPtr(wszTexPathName);

	m_vertices.clear();
	m_particles.clear();

	m_vertices.resize(m_pEProp->iMaxParticleNum);
	m_particles.resize(m_pEProp->iMaxParticleNum);

	m_iVertexCount = m_vertices.size();

	m_iStride = sizeof(CPRS_ParticleVertex);
	m_iOffset = 0;

	hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * m_iStride, &m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	return hr;
}

HRESULT CPGCParticleEmitter::createUVSpriteEmitter(CPRS_EmitterProperty* eProp, std::wstring wszSpriteName)
{
	HRESULT hr = S_OK;

	m_pEProp = eProp;

	m_fSpawnTime = 1.0f / m_pEProp->fSpawnRate;
	m_fTimer = 0.0f;

	m_pSprite = CPGC_SPRITE_MGR.getUVPtr(wszSpriteName);
	m_pTexture = m_pSprite->m_pTexArr[0];

	m_vertices.clear();
	m_particles.clear();

	m_vertices.resize(m_pEProp->iMaxParticleNum);
	m_particles.resize(m_pEProp->iMaxParticleNum);

	m_iVertexCount = m_vertices.size();

	m_iStride = sizeof(CPRS_ParticleVertex);
	m_iOffset = 0;

	hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * m_iStride, &m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	return hr;
}

HRESULT CPGCParticleEmitter::createMTSpriteEmitter(CPRS_EmitterProperty* eProp, std::wstring wszSpriteName)
{
	HRESULT hr = S_OK;

	m_pEProp = eProp;

	m_fSpawnTime = 1.0f / m_pEProp->fSpawnRate;
	m_fTimer = 0.0f;

	m_pSprite = CPGC_SPRITE_MGR.getMTPtr(wszSpriteName);
	if (m_pSprite)
	{
		m_pTexture = m_pSprite->m_pTexArr[0];
	}

	m_vertices.clear();
	m_particles.clear();

	m_vertices.resize(m_pEProp->iMaxParticleNum);
	m_particles.resize(m_pEProp->iMaxParticleNum);

	m_iVertexCount = m_vertices.size();

	m_iStride = sizeof(CPRS_ParticleVertex);
	m_iOffset = 0;

	hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * m_iStride, m_vertices.data(), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	return hr;
}

void CPGCParticleEmitter::setRenderOption(int iBlendStateOption, bool bEnableDepth, bool bEnableDepthWrite)
{
	m_pVLayout = CPGC_VLAYOUT_MGR.getPtr(L"VL_POINTPS");

	m_shaderGroup.pVS = CPGC_SHADER_MGR.getVS(L"VS_PointParticle");
	m_shaderGroup.pGS = CPGC_SHADER_MGR.getGS(L"GS_PointParticle");

	m_pSamplers.push_back(CPGC_DXSTATE_MGR.getSState(L"SS_LINEAR_WRAP"));

	m_pRState = CPGC_DXSTATE_MGR.getRState(CPGC_VNAME2WSTR(RS_SOLID_NOCULL));

	switch (iBlendStateOption)
	{
	case CPRS_BSType::BS_ALPHABLEND:
	{
		m_pBState = CPGC_DXSTATE_MGR.getBState(L"BS_ALPHABLEND");
		m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_PointParticle_Alphablend");
	} break;

	case CPRS_BSType::BS_DUALSOURCEBLEND:
	{
		m_pBState = CPGC_DXSTATE_MGR.getBState(L"BS_DUALSOURCEBLEND");
		m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_PointParticle_Dualsource");
	} break;

	case CPRS_BSType::BS_MSALPHATEST:
	{
		m_pBState = CPGC_DXSTATE_MGR.getBState(L"BS_MSALPHATEST");
		m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_PointParticle_Alphatest");
	} break;

	default:
	{
		m_pBState = CPGC_DXSTATE_MGR.getBState(L"BS_ALPHABLEND");
		m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_PointParticle_Alphablend");
	}
	}

	if (bEnableDepth)
	{
		if (bEnableDepthWrite)
		{
			m_DSState = CPGC_DXSTATE_MGR.getDSState(L"DSS_D_COMPLESS");
		}
		else
		{
			m_DSState = CPGC_DXSTATE_MGR.getDSState(L"DSS_D_COMPLESS_NOWRITE");
		}
	}
	else
	{
		m_DSState = CPGC_DXSTATE_MGR.getDSState(L"DSS_DISABLE");
	}
}

HRESULT CPGCParticleEmitter::updateBuffer(ID3D11Resource* pBuf, void* pResource, UINT iBufSize)
{
	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedRS;
	ZeroMemory(&mappedRS, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = m_pDContext->Map(pBuf, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRS);

	if (SUCCEEDED(hr))
	{
		memcpy(mappedRS.pData, pResource, iBufSize);
		m_pDContext->Unmap(pBuf, 0);
	}

	return hr;
}

void CPGCParticleEmitter::activateTarget(float dt)
{
	if (m_fTimer > m_fSpawnTime)
	{
		m_fTimer -= m_fSpawnTime;

		CPRSPointParticle* replaceTarget = nullptr;

		for (UINT i = 0; i < m_iVertexCount; i++)
		{
			if (m_particles[i].bEnable == false)
			{
				replaceTarget = &m_particles[i];
				break;
			}
		}

		if (replaceTarget)
		{
			replaceTarget->setParticle(m_pEProp);
			int k = 0;
		}
	}
}

void CPGCParticleEmitter::updateParticleState(int idx, float dt)
{
	if (m_pSprite)
	{
		UINT animIdx = (m_particles[idx].fElapsedTime / m_particles[idx].fLifeSpan) * m_pSprite->m_spriteInfo.m_iTotalFrame;
		if (animIdx >= m_pSprite->m_spriteInfo.m_iTotalFrame)
		{
			animIdx = m_pSprite->m_spriteInfo.m_iTotalFrame - 1;
		}

		if (m_pSprite->m_spriteInfo.m_spriteType == CPRS_SPRITE_TYPE::SPRITE_UV)
		{
			m_UVRect = m_pSprite->m_UVSpriteArr[animIdx];
			m_pTexture = m_pSprite->m_pTexArr[0];
		}
		else
		{
			m_pTexture = m_pSprite->m_pTexArr[animIdx];
		}
	}

	m_particles[idx].vVelocity += m_particles[idx].vAccelelation * dt;
	m_particles[idx].vPos = m_particles[idx].vPos + m_particles[idx].vVelocity * dt;

	m_particles[idx].fPYR += m_particles[idx].fPYRVelocity * dt;

	float T = m_particles[idx].fElapsedTime / m_particles[idx].fLifeSpan;
	float invT = (m_particles[idx].fLifeSpan - m_particles[idx].fElapsedTime) / m_particles[idx].fLifeSpan;

	m_particles[idx].vScale =
	{
		m_particles[idx].vInitScale.x * (m_pEProp->fScaleOverLifespan.x * invT + m_pEProp->fScaleOverLifespan.z * T),
		m_particles[idx].vInitScale.y * (m_pEProp->fScaleOverLifespan.y * invT + m_pEProp->fScaleOverLifespan.w * T)
	};

	CPM_Color curColor = CPM_Color::Lerp(m_particles[idx].initColor, m_particles[idx].lastColor, T);

	m_vertices[idx].p = m_particles[idx].vPos;
	m_vertices[idx].n = { 0.0f, 0.0f, 0.0f };
	m_vertices[idx].c = curColor;
	m_vertices[idx].t = { 0.0f, 0.0f };
	m_vertices[idx].spriteRect = { m_UVRect.vMin.x, m_UVRect.vMin.y, m_UVRect.vMax.x, m_UVRect.vMax.y }; //Sprite UV Coord는 GS단계에서 처리한다.
	m_vertices[idx].rot = DirectX::XMMatrixRotationRollPitchYawFromVector(m_particles[idx].fPYR);
	m_vertices[idx].rot = m_vertices[idx].rot.Transpose();
	m_vertices[idx].scale = { m_particles[idx].vScale.x, m_particles[idx].vScale.y, 1.0f };
}


void CPGCEffect::setName(std::wstring name)
{
	m_wszName = name;
}

std::wstring CPGCEffect::getName()
{
	return m_wszName;
}

void CPGCEffect::setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	m_pDevice = pDevice;
	m_pDContext = pDContext;
}

CPGCParticleEffect::CPGCParticleEffect()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;

	m_pPSystem = nullptr;

	m_bPendingDelete = false;

	m_bActivated = false;
	m_fElapsedTime = 0.0f;
}

CPGCParticleEffect::CPGCParticleEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext, CPRS_PSYSTEM_FILESTRUCT* pPSystem, CPM_Vector3 vPos, CPGCParticleEffectProperty& effectOpt)
{
	bool bRet = create(pDevice, pDContext, pPSystem, vPos, effectOpt);
}

CPGCParticleEffect::~CPGCParticleEffect()
{
}

bool CPGCParticleEffect::create(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext, CPRS_PSYSTEM_FILESTRUCT* pPSystem, CPM_Vector3 vPos, CPGCParticleEffectProperty& effectOpt)
{
	m_pPSystem = pPSystem;

	if (!m_pPSystem) { return false; }

	m_effectOpt.setOpt(effectOpt);

	m_bPendingDelete = false;

	int len = m_pPSystem->iEmiiterCnt;

	m_emitterList.resize(len);

	setDevice(pDevice, pDContext);

	for (int i = 0; i < len; i++)
	{
		m_emitterList[i].setDevice(pDevice, pDContext);
		m_emitterList[i].m_playSpeed = m_effectOpt.fPlaySpeed;

		if (m_pPSystem->emitters[i].wszSpriteName[0] != NULL)
		{
			if (m_pPSystem->emitters[i].iSpriteType == CPRS_SPRITE_TYPE::SPRITE_UV)
			{
				m_emitterList[i].createUVSpriteEmitter(&m_pPSystem->emitters[i].eProp, m_pPSystem->emitters[i].wszSpriteName);
			}
			else
			{
				m_emitterList[i].createMTSpriteEmitter(&m_pPSystem->emitters[i].eProp, m_pPSystem->emitters[i].wszSpriteName);
			}

		}
		else
		{
			m_emitterList[i].createEmitter(&m_pPSystem->emitters[i].eProp, m_pPSystem->emitters[i].wszTextureName);
		}

		m_emitterList[i].setRenderOption(m_pPSystem->emitters[i].iBlendStateOption,
			m_pPSystem->emitters[i].bEnableDepth,
			m_pPSystem->emitters[i].bEnableDepthWrite);
	}

	m_vInitPos = vPos;
	m_vPos = vPos;

	m_matWorld = CPM_Matrix::CreateTranslation(m_vPos);

	HRESULT hr = createDXSimpleBuf(m_pDevice,
		sizeof(CPRS_CBUF_COORDCONV_MATSET),
		&m_wvpMat,
		D3D11_BIND_CONSTANT_BUFFER,
		m_pCBWVPMat.GetAddressOf());

	hr = createDXSimpleBuf(m_pDevice,
		sizeof(CPM_Matrix),
		&m_BillTMat,
		D3D11_BIND_CONSTANT_BUFFER,
		m_pCBBillMat.GetAddressOf());

	return true;
}

bool CPGCParticleEffect::init()
{
	return true;
}

bool CPGCParticleEffect::update()
{
	if (!m_bPendingDelete)
	{
		float dt = CPGC_MAINTIMER.getOneFrameTimeF();

		m_fElapsedTime += dt;

		if (m_fElapsedTime < m_effectOpt.fDuration + m_effectOpt.fStDelay)
		{
			if (m_fElapsedTime >= m_effectOpt.fStDelay)
			{
				m_velocity += m_initAccel * dt;
				m_vPos += m_velocity * dt;

				m_matWorld = CPM_Matrix::CreateTranslation(m_vPos);

				for (auto& it : m_emitterList)
				{
					it.update();
				}
			}
		}
		else
		{
			if (m_effectOpt.bLoop)
			{
				m_fElapsedTime = 0.0f;
				m_vPos = m_vInitPos;
				m_velocity = m_initVelocity;
				m_accel = m_initAccel;
			}
			else
			{
				m_bPendingDelete = true;
			}
		}
	}

	return true;
}

bool CPGCParticleEffect::render()
{
	if (!m_bPendingDelete)
	{
		bindPipeline();

		for (auto& it : m_emitterList)
		{
			it.render();
		}
	}

	return true;
}

bool CPGCParticleEffect::release()
{
	for (auto& it : m_emitterList)
	{
		it.release();
	}

	return true;
}

void CPGCParticleEffect::setVelocity(CPM_Vector3 initV, CPM_Vector3 initA)
{
	m_velocity = m_initVelocity = initV;
	m_accel = m_initAccel = initA;
}

void CPGCParticleEffect::setMatrix(const CPM_Matrix* pWorld, const CPM_Matrix* pView, const CPM_Matrix* pProj)
{
	if (pWorld) { m_matWorld = *pWorld; }
	if (pView) { m_matView = *pView; }
	if (pProj) { m_matProj = *pProj; }

	//카메라 회전 행렬의 역행렬을 사용해 월드행렬을 재구성한다.
	//기존의 이동정보는 그대로 가지고 있어야 하기에 이동 성분을 반영한다.

	m_BillTMat = m_matView.Invert();
	m_BillTMat._41 = m_matWorld._41;
	m_BillTMat._42 = m_matWorld._42;
	m_BillTMat._43 = m_matWorld._43;
	m_BillTMat = m_BillTMat.Transpose();

	m_wvpMat.matTWorld = m_matWorld.Transpose();
	m_wvpMat.matTInvWorld = m_matWorld.Invert().Transpose();
	m_wvpMat.matTView = m_matView.Transpose();
	m_wvpMat.matTProj = m_matProj.Transpose();

	updateBuffer(m_pCBWVPMat.Get(), &m_wvpMat, sizeof(CPRS_CBUF_COORDCONV_MATSET));
	updateBuffer(m_pCBBillMat.Get(), &m_BillTMat, sizeof(CPM_Matrix));

	updateState();
}

void CPGCParticleEffect::bindPipeline()
{
	//GS에 공통으로 적용할 월드, 뷰, 투영 행렬 상수버퍼 리소스 바인딩
	m_pDContext->GSSetConstantBuffers(0, 1, m_pCBWVPMat.GetAddressOf());
	m_pDContext->GSSetConstantBuffers(1, 1, m_pCBBillMat.GetAddressOf());
}

void CPGCParticleEffect::updateBuffer(ID3D11Resource* pBuf, void* pResource, UINT iBufSize)
{
	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedRS;
	ZeroMemory(&mappedRS, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = m_pDContext->Map(pBuf, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRS);

	if (SUCCEEDED(hr))
	{
		memcpy(mappedRS.pData, pResource, iBufSize);
		m_pDContext->Unmap(pBuf, 0);
	}
}

void CPGCParticleEffect::updateState()
{
	//회전을 정규 기저 벡터의 회전값의 벡터로 표현하기에는 연산이 불필요하게 많다.
	//임의의 축에 대한 회전을 수행 또는 다른 회전 행렬의 복합적인 곱셈 이후에 
	//적용된 최종행렬에서 추출하는 것이 일반적이다.
	//이 함수는 복잡한 스케일 변화를 반영하지는 않는다.

	// 월드 행렬 R * T
	// Right.x	Right.y	Right.z	0
	// Up.x		Up.y	Up.z	0
	// Look.x	Look.y	Look.z	0
	// Pos.x	Pos.y	Pos.z	1

	//전치 이전의 행렬에서 추출한다.
	m_vPos = { m_matWorld._41, m_matWorld._42, m_matWorld._43 };

	//obj Right-Axis
	m_sightAxis.vRight = { m_matWorld._11, m_matWorld._12, m_matWorld._13 };

	//obj Up-Axis
	m_sightAxis.vUp = { m_matWorld._21, m_matWorld._22, m_matWorld._23 };

	//obj At-Axis
	m_sightAxis.vLook = { m_matWorld._31, m_matWorld._32 , m_matWorld._33 };

	//회전 행렬에서 추출한 각 축벡터는 스케일 행렬에 의해 정규화되어 있지 않은 상태라고 가정한다.
	m_sightAxis.vRight.Normalize();
	m_sightAxis.vUp.Normalize();
	m_sightAxis.vLook.Normalize();

	//회전 쿼터니언 갱신
	m_qRot = CPM_Quaternion::LookRotation(m_sightAxis.vLook, m_sightAxis.vUp);
}

void CPGCParticleEffect::reset()
{
	m_bActivated = false;
	m_bPendingDelete = false;
	m_fElapsedTime = 0.0f;

	m_velocity = m_initVelocity;
	m_accel = m_initAccel;

	m_sightAxis.vUp = CPM_Vector3::Up;
	m_sightAxis.vRight = CPM_Vector3::Right;
	m_sightAxis.vLook = CPM_Vector3::Forward;

	m_qRot = CPM_Quaternion();

	m_matWorld = CPM_Matrix::Identity;
	m_matView = CPM_Matrix::Identity;
	m_matProj = CPM_Matrix::Identity;

	m_wvpMat.matTWorld = CPM_Matrix::Identity;
	m_wvpMat.matTInvWorld = CPM_Matrix::Identity;
	m_wvpMat.matTView = CPM_Matrix::Identity;
	m_wvpMat.matTProj = CPM_Matrix::Identity;

	m_BillTMat = CPM_Matrix::Identity;

	for (auto& it : m_emitterList)
	{
		ZeroMemory(it.m_particles.data(), sizeof(CPRSPointParticle) * it.m_particles.size());
		ZeroMemory(it.m_vertices.data(), sizeof(CPRS_ParticleVertex) * it.m_vertices.size());
	}
}
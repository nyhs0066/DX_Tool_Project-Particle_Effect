#include "CPRSPaticle.h"

CPRSPointParticle::CPRSPointParticle()
{
	bEnable = false;
	bLoop = false;
	fElapsedTime = 0.0f;

	initColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	lastColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	fLifeSpan = 0.0f;
}

void CPRSPointParticle::setParticle(CPRS_EmitterProperty& eProp, CPM_Vector3 worldScaleFactor)
{
	bEnable = true;
	bLoop = eProp.bLoop;
	fElapsedTime = 0.0f;
	vPos =
	{
		eProp.vInitPos.x + CPGC_RNG.getF(eProp.vMinPosOffset.x, eProp.vMaxPosOffset.x),
		eProp.vInitPos.y + CPGC_RNG.getF(eProp.vMinPosOffset.y, eProp.vMaxPosOffset.y),
		eProp.vInitPos.z + CPGC_RNG.getF(eProp.vMinPosOffset.z, eProp.vMaxPosOffset.z)
	};

	vPos = vPos * worldScaleFactor;

	vVelocity =
	{
		CPGC_RNG.getF(eProp.vInitMinVelocity.x, eProp.vInitMaxVelocity.x),
		CPGC_RNG.getF(eProp.vInitMinVelocity.y, eProp.vInitMaxVelocity.y),
		CPGC_RNG.getF(eProp.vInitMinVelocity.z, eProp.vInitMaxVelocity.z)
	};

	vVelocity = vVelocity * worldScaleFactor;

	vAccelelation =
	{
		CPGC_RNG.getF(eProp.vInitMinAcceleration.x, eProp.vInitMaxAcceleration.x),
		CPGC_RNG.getF(eProp.vInitMinAcceleration.y, eProp.vInitMaxAcceleration.y),
		CPGC_RNG.getF(eProp.vInitMinAcceleration.z, eProp.vInitMaxAcceleration.z)
	};

	vAccelelation = vAccelelation * worldScaleFactor;

	fPYR =
	{
		CPGC_DEG2RAD(eProp.fInitPYR.x + CPGC_RNG.getF(eProp.fExtraPYRMinRange.x, eProp.fExtraPYRMaxRange.x)),
		CPGC_DEG2RAD(eProp.fInitPYR.y + CPGC_RNG.getF(eProp.fExtraPYRMinRange.y, eProp.fExtraPYRMaxRange.y)),
		CPGC_DEG2RAD(eProp.fInitPYR.z + CPGC_RNG.getF(eProp.fExtraPYRMinRange.z, eProp.fExtraPYRMaxRange.z))
	};

	fPYRVelocity =
	{
		CPGC_DEG2RAD(CPGC_RNG.getF(eProp.fMinPYRVelocity.x, eProp.fMaxPYRVelocity.x)),
		CPGC_DEG2RAD(CPGC_RNG.getF(eProp.fMinPYRVelocity.y, eProp.fMaxPYRVelocity.y)),
		CPGC_DEG2RAD(CPGC_RNG.getF(eProp.fMinPYRVelocity.z, eProp.fMaxPYRVelocity.z))
	};

	vInitScale =
	{
		(eProp.vInitScale.x + CPGC_RNG.getF(eProp.vExtraScaleRange.x, eProp.vExtraScaleRange.z)) * eProp.fScaleOverLifespan.x,
		(eProp.vInitScale.y + CPGC_RNG.getF(eProp.vExtraScaleRange.y, eProp.vExtraScaleRange.w)) * eProp.fScaleOverLifespan.y
	};

	vScale = vInitScale;

	initColor = eProp.initColorOverLifespan;
	lastColor = eProp.lastColorOverLifespan;
	fLifeSpan = CPGC_RNG.getF(eProp.fLifespanRange.x, eProp.fLifespanRange.y);
}

void CPRSPointParticle::setParticle(CPRS_EmitterProperty* eProp, CPM_Vector3 worldScaleFactor)
{
	if (eProp)
	{
		bEnable = true;
		bLoop = eProp->bLoop;
		fElapsedTime = 0.0f;
		vPos =
		{
			eProp->vInitPos.x + CPGC_RNG.getF(eProp->vMinPosOffset.x, eProp->vMaxPosOffset.x),
			eProp->vInitPos.y + CPGC_RNG.getF(eProp->vMinPosOffset.y, eProp->vMaxPosOffset.y),
			eProp->vInitPos.z + CPGC_RNG.getF(eProp->vMinPosOffset.z, eProp->vMaxPosOffset.z)
		};

		vPos = vPos * worldScaleFactor;

		vVelocity =
		{
			CPGC_RNG.getF(eProp->vInitMinVelocity.x, eProp->vInitMaxVelocity.x),
			CPGC_RNG.getF(eProp->vInitMinVelocity.y, eProp->vInitMaxVelocity.y),
			CPGC_RNG.getF(eProp->vInitMinVelocity.z, eProp->vInitMaxVelocity.z)
		};

		vVelocity = vVelocity * worldScaleFactor;

		vAccelelation =
		{
			CPGC_RNG.getF(eProp->vInitMinAcceleration.x, eProp->vInitMaxAcceleration.x),
			CPGC_RNG.getF(eProp->vInitMinAcceleration.y, eProp->vInitMaxAcceleration.y),
			CPGC_RNG.getF(eProp->vInitMinAcceleration.z, eProp->vInitMaxAcceleration.z)
		};

		vAccelelation = vAccelelation * worldScaleFactor;

		fPYR =
		{
			CPGC_DEG2RAD(eProp->fInitPYR.x + CPGC_RNG.getF(eProp->fExtraPYRMinRange.x, eProp->fExtraPYRMaxRange.x)),
			CPGC_DEG2RAD(eProp->fInitPYR.y + CPGC_RNG.getF(eProp->fExtraPYRMinRange.y, eProp->fExtraPYRMaxRange.y)),
			CPGC_DEG2RAD(eProp->fInitPYR.z + CPGC_RNG.getF(eProp->fExtraPYRMinRange.z, eProp->fExtraPYRMaxRange.z))
		};

		fPYRVelocity =
		{
			CPGC_DEG2RAD(CPGC_RNG.getF(eProp->fMinPYRVelocity.x, eProp->fMaxPYRVelocity.x)),
			CPGC_DEG2RAD(CPGC_RNG.getF(eProp->fMinPYRVelocity.y, eProp->fMaxPYRVelocity.y)),
			CPGC_DEG2RAD(CPGC_RNG.getF(eProp->fMinPYRVelocity.z, eProp->fMaxPYRVelocity.z))
		};

		vInitScale =
		{
			(eProp->vInitScale.x + CPGC_RNG.getF(eProp->vExtraScaleRange.x, eProp->vExtraScaleRange.z)) * eProp->fScaleOverLifespan.x,
			(eProp->vInitScale.y + CPGC_RNG.getF(eProp->vExtraScaleRange.y, eProp->vExtraScaleRange.w)) * eProp->fScaleOverLifespan.y
		};

		vScale = vInitScale;

		initColor = eProp->initColorOverLifespan;
		lastColor = eProp->lastColorOverLifespan;
		fLifeSpan = CPGC_RNG.getF(eProp->fLifespanRange.x, eProp->fLifespanRange.y);
	}
}

void CPRSPointParticle::setPivotPosParticle(CPRS_EmitterProperty& eProp, CPM_Vector3 vInitWorldPos, CPM_Vector3 worldScaleFactor)
{
	bEnable = true;
	bLoop = eProp.bLoop;
	fElapsedTime = 0.0f;
	vPos =
	{
		vInitWorldPos.x + CPGC_RNG.getF(eProp.vMinPosOffset.x, eProp.vMaxPosOffset.x) * worldScaleFactor.x,
		vInitWorldPos.y + CPGC_RNG.getF(eProp.vMinPosOffset.y, eProp.vMaxPosOffset.y) * worldScaleFactor.y,
		vInitWorldPos.z + CPGC_RNG.getF(eProp.vMinPosOffset.z, eProp.vMaxPosOffset.z) * worldScaleFactor.z
	};

	vVelocity =
	{
		CPGC_RNG.getF(eProp.vInitMinVelocity.x, eProp.vInitMaxVelocity.x),
		CPGC_RNG.getF(eProp.vInitMinVelocity.y, eProp.vInitMaxVelocity.y),
		CPGC_RNG.getF(eProp.vInitMinVelocity.z, eProp.vInitMaxVelocity.z)
	};

	vVelocity = vVelocity * worldScaleFactor;

	vAccelelation =
	{
		CPGC_RNG.getF(eProp.vInitMinAcceleration.x, eProp.vInitMaxAcceleration.x),
		CPGC_RNG.getF(eProp.vInitMinAcceleration.y, eProp.vInitMaxAcceleration.y),
		CPGC_RNG.getF(eProp.vInitMinAcceleration.z, eProp.vInitMaxAcceleration.z)
	};

	vAccelelation = vAccelelation * worldScaleFactor;

	fPYR =
	{
		CPGC_DEG2RAD(eProp.fInitPYR.x + CPGC_RNG.getF(eProp.fExtraPYRMinRange.x, eProp.fExtraPYRMaxRange.x)),
		CPGC_DEG2RAD(eProp.fInitPYR.y + CPGC_RNG.getF(eProp.fExtraPYRMinRange.y, eProp.fExtraPYRMaxRange.y)),
		CPGC_DEG2RAD(eProp.fInitPYR.z + CPGC_RNG.getF(eProp.fExtraPYRMinRange.z, eProp.fExtraPYRMaxRange.z))
	};

	fPYRVelocity =
	{
		CPGC_DEG2RAD(CPGC_RNG.getF(eProp.fMinPYRVelocity.x, eProp.fMaxPYRVelocity.x)),
		CPGC_DEG2RAD(CPGC_RNG.getF(eProp.fMinPYRVelocity.y, eProp.fMaxPYRVelocity.y)),
		CPGC_DEG2RAD(CPGC_RNG.getF(eProp.fMinPYRVelocity.z, eProp.fMaxPYRVelocity.z))
	};

	vInitScale =
	{
		(eProp.vInitScale.x + CPGC_RNG.getF(eProp.vExtraScaleRange.x, eProp.vExtraScaleRange.z)) * eProp.fScaleOverLifespan.x,
		(eProp.vInitScale.y + CPGC_RNG.getF(eProp.vExtraScaleRange.y, eProp.vExtraScaleRange.w)) * eProp.fScaleOverLifespan.y
	};

	vScale = vInitScale;

	initColor = eProp.initColorOverLifespan;
	lastColor = eProp.lastColorOverLifespan;
	fLifeSpan = CPGC_RNG.getF(eProp.fLifespanRange.x, eProp.fLifespanRange.y);
}

void CPRSPointParticle::setPivotPosParticle(CPRS_EmitterProperty* eProp, CPM_Vector3 vInitWorldPos, CPM_Vector3 worldScaleFactor)
{
	if (eProp)
	{
		bEnable = true;
		bLoop = eProp->bLoop;
		fElapsedTime = 0.0f;
		vPos =
		{
			vInitWorldPos.x + CPGC_RNG.getF(eProp->vMinPosOffset.x, eProp->vMaxPosOffset.x),
			vInitWorldPos.y + CPGC_RNG.getF(eProp->vMinPosOffset.y, eProp->vMaxPosOffset.y),
			vInitWorldPos.z + CPGC_RNG.getF(eProp->vMinPosOffset.z, eProp->vMaxPosOffset.z)
		};

		vPos = vPos * worldScaleFactor;

		vVelocity =
		{
			CPGC_RNG.getF(eProp->vInitMinVelocity.x, eProp->vInitMaxVelocity.x),
			CPGC_RNG.getF(eProp->vInitMinVelocity.y, eProp->vInitMaxVelocity.y),
			CPGC_RNG.getF(eProp->vInitMinVelocity.z, eProp->vInitMaxVelocity.z)
		};

		vVelocity = vVelocity * worldScaleFactor;

		vAccelelation =
		{
			CPGC_RNG.getF(eProp->vInitMinAcceleration.x, eProp->vInitMaxAcceleration.x),
			CPGC_RNG.getF(eProp->vInitMinAcceleration.y, eProp->vInitMaxAcceleration.y),
			CPGC_RNG.getF(eProp->vInitMinAcceleration.z, eProp->vInitMaxAcceleration.z)
		};

		vAccelelation = vAccelelation * worldScaleFactor;

		fPYR =
		{
			CPGC_DEG2RAD(eProp->fInitPYR.x + CPGC_RNG.getF(eProp->fExtraPYRMinRange.x, eProp->fExtraPYRMaxRange.x)),
			CPGC_DEG2RAD(eProp->fInitPYR.y + CPGC_RNG.getF(eProp->fExtraPYRMinRange.y, eProp->fExtraPYRMaxRange.y)),
			CPGC_DEG2RAD(eProp->fInitPYR.z + CPGC_RNG.getF(eProp->fExtraPYRMinRange.z, eProp->fExtraPYRMaxRange.z))
		};

		fPYRVelocity =
		{
			CPGC_DEG2RAD(CPGC_RNG.getF(eProp->fMinPYRVelocity.x, eProp->fMaxPYRVelocity.x)),
			CPGC_DEG2RAD(CPGC_RNG.getF(eProp->fMinPYRVelocity.y, eProp->fMaxPYRVelocity.y)),
			CPGC_DEG2RAD(CPGC_RNG.getF(eProp->fMinPYRVelocity.z, eProp->fMaxPYRVelocity.z))
		};

		vInitScale =
		{
			(eProp->vInitScale.x + CPGC_RNG.getF(eProp->vExtraScaleRange.x, eProp->vExtraScaleRange.z)) * eProp->fScaleOverLifespan.x,
			(eProp->vInitScale.y + CPGC_RNG.getF(eProp->vExtraScaleRange.y, eProp->vExtraScaleRange.w)) * eProp->fScaleOverLifespan.y
		};

		vScale = vInitScale;

		initColor = eProp->initColorOverLifespan;
		lastColor = eProp->lastColorOverLifespan;
		fLifeSpan = CPGC_RNG.getF(eProp->fLifespanRange.x, eProp->fLifespanRange.y);
	}
}

CPRSPointParticleEmitter::CPRSPointParticleEmitter()
{
	m_emitterType = CPRS_EMITTER_TYPE::DEFAULT_PARTICLE;

	//파티클 생성 타이머
	m_fSpawnTime = 0.0f;
	m_fTimer = 0.0f;

	m_pSprite = nullptr;
	m_pTexture = nullptr;

	m_iVertexCount = 0;
	m_iStride = sizeof(CPRS_ParticleVertex);
	m_iOffset = 0;

	m_pVLayout = nullptr;
	
	m_iBlendStateOption = CPRS_BSType::BS_DEFAULT;
	m_bEnableDepth = false;
	m_bEnableDepthWrite = false;
	m_bBackCull = false;

	m_pSampler = nullptr;
	m_UVRect = { {0.0f, 0.0f}, {1.0f, 1.0f} };
}

CPRSPointParticleEmitter::~CPRSPointParticleEmitter()
{
}

bool CPRSPointParticleEmitter::init()
{
	return true;
}

bool CPRSPointParticleEmitter::update(float dt)
{
	activateTarget(dt);

	for (UINT i = 0; i < m_iVertexCount; i++)
	{
		m_particles[i].fElapsedTime += dt;

		if (!m_particles[i].bLoop)
		{
			if (m_particles[i].fElapsedTime > m_particles[i].fLifeSpan)
			{
				m_particles[i].bEnable = false;

				//정점의 스케일 속성의 마지막 성분을 활성화 여부로 사용한다.
				m_vertices[i].scale.z = 0.0f;
			}

			if (m_particles[i].bEnable)
			{
				updateParticleState(i, dt);
			}
		}
		else
		{
			updateParticleState(i, dt);
		}
	}

	updateBuffer(m_pVBuf.Get(), &m_vertices.at(0), m_vertices.size() * sizeof(CPRS_ParticleVertex));
	return true;
}

bool CPRSPointParticleEmitter::render()
{
	if (m_eProp.bShow && m_pVBuf.Get() && m_pCBuf.Get())
	{
		bindToPipeline();

		m_pDContext->Draw(m_iVertexCount, 0);
	}

	return true;
}

bool CPRSPointParticleEmitter::release()
{
	m_pVLayout = nullptr;

	m_vertices.clear();
	m_particles.clear();

	return true;
}

void CPRSPointParticleEmitter::bindToPipeline()
{
	m_pDContext->IASetVertexBuffers(0, 1, m_pVBuf.GetAddressOf(), &m_iStride, &m_iOffset);
	m_pDContext->IASetInputLayout(m_pVLayout);

	//점 입자를 GS에서 늘릴 것이므로 IA스테이지에서의 입력 기하요소는 POINTLIST로 설정한다.
	m_pDContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//GS에 공통으로 적용할 월드, 뷰, 투영 행렬 상수버퍼 리소스 바인딩
	m_pDContext->GSSetConstantBuffers(0, 1, m_pCBuf.GetAddressOf());
	m_pDContext->GSSetConstantBuffers(1, 1, m_pCBuf_billboard.GetAddressOf());

	if (m_pTexture)
	{
		ID3D11ShaderResourceView* pTemp = m_pTexture->getSRV();
		m_pDContext->PSSetShaderResources(0, 1, &pTemp);
	}

	switch (m_iBlendStateOption)
	{
		case CPRS_BSType::BS_ALPHABLEND :
		{
			m_pDContext->OMSetBlendState(CPGC_DXSTATE_MGR.getBState(L"BS_ALPHABLEND"), NULL, -1);
			m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_PointParticle_Alphablend_UsedOnTool");
		} break;

		case CPRS_BSType::BS_DUALSOURCEBLEND:
		{
			m_pDContext->OMSetBlendState(CPGC_DXSTATE_MGR.getBState(L"BS_DUALSOURCEBLEND"), NULL, -1);
			m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_PointParticle_Dualsource_UsedOnTool");
		} break;

		case CPRS_BSType::BS_MSALPHATEST:
		{
			m_pDContext->OMSetBlendState(CPGC_DXSTATE_MGR.getBState(L"BS_MSALPHATEST"), NULL, -1);
			m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_PointParticle_Alphatest_UsedOnTool");
		} break;

		case CPRS_BSType::BS_ADDITIVEBLEND:
		{
			m_pDContext->OMSetBlendState(CPGC_DXSTATE_MGR.getBState(L"BS_ADDITIVEBLEND"), NULL, -1);
			m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_PointParticle_Alphablend_UsedOnTool");
		} break;

		default :
		{
			m_pDContext->OMSetBlendState(CPGC_DXSTATE_MGR.getBState(L"BS_DEFAULT"), NULL, -1);
			m_shaderGroup.pPS = CPGC_SHADER_MGR.getPS(L"PS_PointParticle_Alphablend_UsedOnTool");
		}
	}

	m_shaderGroup.bindToPipeline(m_pDContext);
	m_pDContext->PSSetSamplers(0, 1, &m_pSampler);

	if (m_bBackCull)
	{
		m_pDContext->RSSetState(CPGC_DXSTATE_MGR.getRState(CPGC_VNAME2WSTR(RS_SOLID)));
	}
	else
	{
		m_pDContext->RSSetState(CPGC_DXSTATE_MGR.getRState(CPGC_VNAME2WSTR(RS_SOLID_NOCULL)));
	}
	

	if (m_bEnableDepth)
	{
		if (m_bEnableDepthWrite)
		{
			m_pDContext->OMSetDepthStencilState(CPGC_DXSTATE_MGR.getDSState(L"DSS_D_COMPLESS"), 1);
		}
		else
		{
			m_pDContext->OMSetDepthStencilState(CPGC_DXSTATE_MGR.getDSState(L"DSS_D_COMPLESS_NOWRITE"), 1);
		}
	}
	else
	{
		m_pDContext->OMSetDepthStencilState(CPGC_DXSTATE_MGR.getDSState(L"DSS_DISABLE"), 1);
	}
}

void CPRSPointParticleEmitter::setMatrix(const CPM_Matrix* pWorldM, const CPM_Matrix* pViewM, const CPM_Matrix* pProjM, const CPM_Matrix* pParentWorldM)
{
	if (pParentWorldM)	{ m_matParentWorld = *pParentWorldM; }
	if (pWorldM)		{ m_matLocalWorld = *pWorldM; }
	if (pViewM)			{ m_matView = *pViewM; }
	if (pProjM)			{ m_matProj = *pProjM; }

	m_matWorld = m_matLocalWorld * m_matParentWorld;

	static CPM_Matrix WorldRot;
	static CPM_Matrix CamRot;

	//카메라 회전 행렬의 역행렬을 사용해 월드행렬을 재구성한다.
	//기존의 이동정보는 그대로 가지고 있어야 하기에 이동 성분을 반영한다.
	switch (m_eProp.iUseBillBoard)
	{
		//빌보드 사용 안함
		case 0 :
		{
			m_billboardMat.m_billboardTMat = CPM_Matrix::Identity;
		} break;

		//전체화면 빌보드
		case 1:
		{
			WorldRot = m_matWorld;
			WorldRot._41 = 0.0f;
			WorldRot._42 = 0.0f;
			WorldRot._43 = 0.0f;
			WorldRot._44 = 1.0f;

			CamRot = m_matView;
			CamRot._41 = 0.0f;
			CamRot._42 = 0.0f;
			CamRot._43 = 0.0f;
			CamRot._44 = 1.0f;

			m_billboardMat.m_billboardTMat = WorldRot * CamRot;
		} break;

		//Y축 빌보드
		case 2:
		{
			WorldRot = m_matWorld;
			WorldRot._41 = 0.0f;
			WorldRot._42 = 0.0f;
			WorldRot._43 = 0.0f;
			WorldRot._44 = 1.0f;

			CamRot = CPM_Matrix::Identity;
			CamRot._11 = m_matView._11;
			CamRot._13 = m_matView._13;
			CamRot._31 = m_matView._31;
			CamRot._33 = m_matView._33;

			m_billboardMat.m_billboardTMat = WorldRot * CamRot;

		} break;
	}

	updateBuffer(m_pCBuf_billboard.Get(), &m_billboardMat, sizeof(CPRS_CBUF_BILLBOARDMAT));

	updateState();
	updateCoordConvMat();
}

void CPRSPointParticleEmitter::updateState()
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
	m_vPrevPos = m_vPos;
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
}

void CPRSPointParticleEmitter::updateCoordConvMat()
{
	m_wvpMat.matTWorld = m_matWorld.Transpose();
	m_matWorld.Invert(m_wvpMat.matTInvWorld);
	m_wvpMat.matTInvWorld = m_wvpMat.matTInvWorld.Transpose();

	m_matView.Transpose(m_wvpMat.matTView);
	m_matProj.Transpose(m_wvpMat.matTProj);

	updateBuffer(m_pCBuf.Get(), &m_wvpMat, sizeof(CPRS_CBUF_COORDCONV_MATSET));
}

HRESULT CPRSPointParticleEmitter::createEmitter(CPRS_EmitterProperty eProp,
	std::wstring wszTexName)
{
	HRESULT hr = S_OK;

	m_eProp = eProp;

	m_matWorld = CPM_Matrix::CreateTranslation(m_eProp.vInitPos);
	m_fSpawnTime = 1.0f / m_eProp.fSpawnRate;
	m_fTimer = 0.0f;

	m_pTexture = CPGC_TEXTURE_MGR.getPtr(wszTexName);
	m_pVLayout = CPGC_VLAYOUT_MGR.getPtr(L"VL_POINTPS");

	m_shaderGroup.pVS = CPGC_SHADER_MGR.getVS(L"VS_PointParticle");
	m_shaderGroup.pGS = CPGC_SHADER_MGR.getGS(L"GS_PointParticle");

	m_pSampler = CPGC_DXSTATE_MGR.getSState(L"SS_LINEAR_WRAP");

	m_vertices.clear();
	m_particles.clear();

	m_vertices.resize(m_eProp.iMaxParticleNum);
	m_particles.resize(m_eProp.iMaxParticleNum);

	m_iVertexCount = m_vertices.size();

	hr = createBuffers();

	return hr;
}

HRESULT CPRSPointParticleEmitter::createUVSpriteEmitter(CPRS_EmitterProperty eProp,
	std::wstring wszSpriteName)
{
	HRESULT hr = S_OK;

	m_eProp = eProp;

	m_matWorld = CPM_Matrix::CreateTranslation(m_eProp.vInitPos);
	m_fSpawnTime = 1.0f / m_eProp.fSpawnRate;
	m_fTimer = 0.0f;

	m_pSprite = CPGC_SPRITE_MGR.getUVPtr(wszSpriteName);
	m_pTexture = m_pSprite->m_pTexArr[0];

	m_pVLayout = CPGC_VLAYOUT_MGR.getPtr(L"VL_POINTPS");

	m_shaderGroup.pVS = CPGC_SHADER_MGR.getVS(L"VS_PointParticle");
	m_shaderGroup.pGS = CPGC_SHADER_MGR.getGS(L"GS_PointParticle");

	m_pSampler = CPGC_DXSTATE_MGR.getSState(L"SS_LINEAR_WRAP");

	m_vertices.clear();
	m_particles.clear();

	m_vertices.resize(m_eProp.iMaxParticleNum);
	m_particles.resize(m_eProp.iMaxParticleNum);

	m_iVertexCount = m_vertices.size();

	hr = createBuffers();

	return hr;
}

HRESULT CPRSPointParticleEmitter::createMTSpriteEmitter(CPRS_EmitterProperty eProp, 
	std::wstring wszSpriteName)
{
	HRESULT hr = S_OK;

	m_eProp = eProp;

	m_matWorld = CPM_Matrix::CreateTranslation(m_eProp.vInitPos);
	m_fSpawnTime = 1.0f / m_eProp.fSpawnRate;
	m_fTimer = 0.0f;

	m_pSprite = CPGC_SPRITE_MGR.getMTPtr(wszSpriteName);
	if (m_pSprite)
	{
		m_pTexture = m_pSprite->m_pTexArr[0];
	}

	m_pVLayout = CPGC_VLAYOUT_MGR.getPtr(L"VL_POINTPS");

	m_shaderGroup.pVS = CPGC_SHADER_MGR.getVS(L"VS_PointParticle");
	m_shaderGroup.pGS = CPGC_SHADER_MGR.getGS(L"GS_PointParticle");

	m_pSampler = CPGC_DXSTATE_MGR.getSState(L"SS_LINEAR_WRAP");

	m_vertices.clear();
	m_particles.clear();

	m_vertices.resize(m_eProp.iMaxParticleNum);
	m_particles.resize(m_eProp.iMaxParticleNum);

	m_iVertexCount = m_vertices.size();

	hr = createBuffers();

	return hr;
}

void CPRSPointParticleEmitter::setRenderOption(int iBlendStateOption,
	bool bEnableDepth,
	bool bEnableDepthWrite,
	bool bBackCull)
{
	m_iBlendStateOption = iBlendStateOption;
	m_bEnableDepth = bEnableDepth;
	m_bEnableDepthWrite = bEnableDepthWrite;
	m_bBackCull = bBackCull;
}

HRESULT CPRSPointParticleEmitter::updateBuffer(ID3D11Resource* pBuf, void* pResource, UINT iBufSize)
{
	HRESULT hr = S_OK;

	if (pBuf && pResource)
	{
		D3D11_MAPPED_SUBRESOURCE mappedRS;
		ZeroMemory(&mappedRS, sizeof(D3D11_MAPPED_SUBRESOURCE));

		hr = m_pDContext->Map(pBuf, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRS);

		if (SUCCEEDED(hr))
		{
			memcpy(mappedRS.pData, pResource, iBufSize);
			m_pDContext->Unmap(pBuf, 0);
		}
	}
	else { hr = E_POINTER; }

	return hr;
}

void CPRSPointParticleEmitter::activateTarget(float dt, bool bActivate)
{
	m_fTimer += dt;

	if (m_fTimer > m_fSpawnTime)
	{
		m_fTimer -= m_fSpawnTime;

		CPRSPointParticle* replaceTarget = nullptr;

		if (bActivate)
		{
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
				CPM_Vector3 scale = CPM_Vector3(m_wvpMat.matTWorld._11, m_wvpMat.matTWorld._22, m_wvpMat.matTWorld._33);
				replaceTarget->setParticle(m_eProp, scale);
			}
		}
	}
}

void CPRSPointParticleEmitter::updateParticleState(int idx, float dt)
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
		m_particles[idx].vInitScale.x * (m_eProp.fScaleOverLifespan.x * invT + m_eProp.fScaleOverLifespan.z * T),
		m_particles[idx].vInitScale.y * (m_eProp.fScaleOverLifespan.y * invT + m_eProp.fScaleOverLifespan.w * T)
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

void CPRSPointParticleEmitter::stateReset()
{
	for (int i = 0; i < m_particles.size(); i++)
	{
		m_particles[i].bEnable = false;
		m_vertices[i].scale.z = 0.0f;
	}

	m_fTimer = 0.0f;
}

HRESULT CPRSPointParticleEmitter::createBuffers()
{
	HRESULT hr = S_OK;

	if (m_pVBuf.Get()) { m_pVBuf.ReleaseAndGetAddressOf(); }
	if (m_pCBuf.Get()) { m_pCBuf.ReleaseAndGetAddressOf(); }
	if (m_pCBuf_billboard.Get()) { m_pCBuf_billboard.ReleaseAndGetAddressOf(); }

	hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * sizeof(CPRS_ParticleVertex), &m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	hr = createDXSimpleBuf(m_pDevice, sizeof(CPRS_CBUF_COORDCONV_MATSET), &m_wvpMat, D3D11_BIND_CONSTANT_BUFFER, m_pCBuf.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	hr = createDXSimpleBuf(m_pDevice, sizeof(CPRS_CBUF_BILLBOARDMAT), &m_billboardMat, D3D11_BIND_CONSTANT_BUFFER, m_pCBuf_billboard.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	return hr;
}

CPRSParticleTrailEmitter::CPRSParticleTrailEmitter() : CPRSPointParticleEmitter()
{
	m_emitterType = CPRS_EMITTER_TYPE::PARTICLE_TRAIL;
	m_iActivateIdx = 0;
}

CPRSParticleTrailEmitter::~CPRSParticleTrailEmitter()
{
}

bool CPRSParticleTrailEmitter::init()
{
	return true;
}

bool CPRSParticleTrailEmitter::update(float dt)
{
	activateTarget(dt);

	for (UINT i = 0; i < m_iVertexCount; i++)
	{
		m_particles[i].fElapsedTime += dt;

		if (!m_particles[i].bLoop)
		{
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
		else
		{
			updateParticleState(i, dt);
		}
	}

	updateBuffer(m_pVBuf.Get(), &m_vertices.at(0), m_vertices.size() * sizeof(CPRS_ParticleVertex));
	updateBuffer(m_pCBuf.Get(), &m_wvpMat, sizeof(CPRS_CBUF_COORDCONV_MATSET));

	return true;
}

HRESULT CPRSParticleTrailEmitter::createEmitter(CPRS_EmitterProperty eProp, std::wstring wszTexName)
{
	HRESULT hr = CPRSPointParticleEmitter::createEmitter(eProp, wszTexName);

	if (FAILED(hr)) { return hr; }

	m_shaderGroup.pGS = CPGC_SHADER_MGR.getGS(L"GS_ParticleTrail");

	return hr;
}

HRESULT CPRSParticleTrailEmitter::createUVSpriteEmitter(CPRS_EmitterProperty eProp, std::wstring wszSpriteName)
{
	HRESULT hr = CPRSPointParticleEmitter::createUVSpriteEmitter(eProp, wszSpriteName);

	if (FAILED(hr)) { return hr; }

	m_shaderGroup.pGS = CPGC_SHADER_MGR.getGS(L"GS_ParticleTrail");

	return hr;
}

HRESULT CPRSParticleTrailEmitter::createMTSpriteEmitter(CPRS_EmitterProperty eProp, std::wstring wszSpriteName)
{
	HRESULT hr = CPRSPointParticleEmitter::createMTSpriteEmitter(eProp, wszSpriteName);

	if (FAILED(hr)) { return hr; }

	m_shaderGroup.pGS = CPGC_SHADER_MGR.getGS(L"GS_ParticleTrail");

	return hr;
}

void CPRSParticleTrailEmitter::activateTarget(float dt, bool bActivate)
{
	m_fTimer += dt;

	if (m_fTimer > m_fSpawnTime)
	{
		m_fTimer -= m_fSpawnTime;

		if (bActivate)
		{
			if (m_iActivateIdx >= m_particles.size()) { m_iActivateIdx = 0; }
			CPRSPointParticle* replaceTarget = &m_particles[m_iActivateIdx++];

			CPM_Vector3 scale = CPM_Vector3(m_wvpMat.matTWorld._11, m_wvpMat.matTWorld._22, m_wvpMat.matTWorld._33);
			replaceTarget->setPivotPosParticle(m_eProp, CPM_Vector3(m_vPos), scale);
		}
	}
}

CPRSParticleBurstEmitter::CPRSParticleBurstEmitter()
{
	m_emitterType = CPRS_EMITTER_TYPE::BURST_PARTICLE;
	m_iActivateIdx = 0;
}

CPRSParticleBurstEmitter::~CPRSParticleBurstEmitter()
{
}

bool CPRSParticleBurstEmitter::init()
{
	return true;
}

bool CPRSParticleBurstEmitter::update(float dt)
{
	activateTarget(dt);

	for (UINT i = 0; i < m_iVertexCount; i++)
	{
		m_particles[i].fElapsedTime += dt;

		if (!m_particles[i].bLoop)
		{
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
		else
		{
			updateParticleState(i, dt);
		}
	}

	updateBuffer(m_pVBuf.Get(), &m_vertices.at(0), m_vertices.size() * sizeof(CPRS_ParticleVertex));
	updateBuffer(m_pCBuf.Get(), &m_wvpMat, sizeof(CPRS_CBUF_COORDCONV_MATSET));

	return true;
}

//HRESULT CPRSParticleBurstEmitter::createEmitter(CPRS_EmitterProperty eProp, std::wstring wszTexName)
//{
//	return E_NOTIMPL;
//}
//
//HRESULT CPRSParticleBurstEmitter::createUVSpriteEmitter(CPRS_EmitterProperty eProp, std::wstring wszSpriteName)
//{
//	return E_NOTIMPL;
//}
//
//HRESULT CPRSParticleBurstEmitter::createMTSpriteEmitter(CPRS_EmitterProperty eProp, std::wstring wszSpriteName)
//{
//	return E_NOTIMPL;
//}

void CPRSParticleBurstEmitter::activateTarget(float dt, bool bActivate)
{
	if (m_eProp.iAnimKeyCnt)
	{
		m_fTimer += dt;

		if (bActivate)
		{
			if (m_fTimer > m_eProp.animkeyList[m_iActivateIdx].fTime)
			{
				for (int i = 0; i < m_particles.size(); i++)
				{
					CPRSPointParticle* replaceTarget = &m_particles[i];

					CPM_Vector3 scale = CPM_Vector3(m_wvpMat.matTWorld._11, m_wvpMat.matTWorld._22, m_wvpMat.matTWorld._33);
					replaceTarget->setPivotPosParticle(m_eProp, CPM_Vector3(m_vPos), scale);
				}

				if (m_iActivateIdx >= m_eProp.iAnimKeyCnt - 1)
				{
					if (m_eProp.bLoop)
					{
						stateReset();
					}
					else
					{
						//m_bActivate = false;
						stateReset();
					}
				}
				else { m_iActivateIdx++; }
			}
		}
	}
}

void CPRSParticleBurstEmitter::stateReset()
{
	m_fTimer = 0.0f;
	m_iActivateIdx = 0;
	m_bActivate = true;
}

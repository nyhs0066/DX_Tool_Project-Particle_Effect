#pragma once
#include "CPGCParticleSystem.h"

struct CPGCParticleEffectProperty
{
	float												fDuration;
	bool												bLoop;
	float												fStDelay;
	float												fPlaySpeed;
	int													iCullMode;

	CPGCParticleEffectProperty()
	{
		fDuration = 0.0f;
		bLoop = false;
		fStDelay = 0.0f;
		fPlaySpeed = 1.0f;
		iCullMode = 0;
	}

	CPGCParticleEffectProperty(const CPGCParticleEffectProperty& other)
	{
		setOpt(other);
	}

	void setOpt(const CPGCParticleEffectProperty& other)
	{
		*this = other;
	}
};

class CPGCParticleEmitter
{
public:
	ID3D11Device*									m_pDevice;
	ID3D11DeviceContext*							m_pDContext;

	//이미터의 위치
	CPM_Vector3										m_vPos;

	//이미터의 회전
	CPM_Quaternion									m_qRot;
	CPM_Vector3										m_PitchYawRoll;

	//파티클 생성 타이머
	float											m_fSpawnTime;
	float											m_fTimer;

	//파티클의 속성 정보
	CPRS_EmitterProperty*							m_pEProp;

	//렌더 옵션
	//파티클 리스트 : 일단 리스트의 순서는 정점 리스트의 순서와 동일하게 처리한다.
	std::vector<CPRSPointParticle>					m_particles;

	//파티클 정점 정보
	std::vector<CPRS_ParticleVertex>				m_vertices;
	UINT											m_iVertexCount;
	UINT											m_iStride;
	UINT											m_iOffset;

	//정점 레이아웃 정보
	ID3D11InputLayout*								m_pVLayout;

	//버퍼 정보
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pVBuf;

	//시스템에서 넘겨받는 재생속도
	float											m_playSpeed;

	//텍스처 & 스프라이트
	CPRS_Sprite* m_pSprite;
	CPRS_Texture* m_pTexture;
	CPRS_RECT										m_UVRect;

	//Shader
	CPRS_ShaderGroup								m_shaderGroup;

	//렌더 옵션 프리셋 사용여부
	std::vector<ID3D11SamplerState*>				m_pSamplers;
	ID3D11RasterizerState*							m_pRState;
	ID3D11BlendState*								m_pBState;
	ID3D11DepthStencilState*						m_DSState;

public:
	CPGCParticleEmitter();
	virtual ~CPGCParticleEmitter();

	virtual bool	init();
	virtual bool	update();
	virtual bool	render();
	virtual bool	release();

	virtual void	bindToPipeline();

	void setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);

	virtual HRESULT	createEmitter(CPRS_EmitterProperty* eProp,
		std::wstring wszTexPathName);

	virtual HRESULT	createUVSpriteEmitter(CPRS_EmitterProperty* eProp,
		std::wstring wszSpriteName);

	virtual HRESULT	createMTSpriteEmitter(CPRS_EmitterProperty* eProp,
		std::wstring wszSpriteName);

	virtual void	setRenderOption(int iBlendStateOption,
		bool bEnableDepth,
		bool bEnableDepthWrite);

	HRESULT updateBuffer(ID3D11Resource* pBuf, void* pResource, UINT iBufSize);

public:
	void activateTarget(float dt);
	void updateParticleState(int idx, float dt);
};

class CPGCEffect
{
protected:
	std::wstring			m_wszName;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDContext = nullptr;

public:
	bool					m_bPendingDelete = false;

	CPGCEffect() {};
	virtual ~CPGCEffect() {};

	virtual bool init() = 0;

	virtual bool preUpdate() { return true; };
	virtual bool update() = 0;
	virtual bool postUpdate() { return true; };

	virtual bool preRender() { return true; };
	virtual bool render() = 0;
	virtual bool postRender() { return true; };

	virtual bool release() = 0;

	virtual void setMatrix(const CPM_Matrix* pWorld, const CPM_Matrix* pView, const CPM_Matrix* pProj) = 0;

	void setName(std::wstring name);
	std::wstring getName();
	void setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);
};

class CPGCParticleEffect : public CPGCEffect
{
public:
	//파티클 설정 옵션
	CPGCParticleEffectProperty							m_effectOpt;

	bool												m_bActivated;
	float												m_fElapsedTime;

	CPRS_PSYSTEM_FILESTRUCT*							m_pPSystem;

	//이펙트 배치 위치
	CPM_Vector3											m_vInitPos;
	CPM_Vector3											m_vPos;

	//속도 & 가속도
	CPM_Vector3											m_initVelocity;
	CPM_Vector3											m_initAccel;

	CPM_Vector3											m_velocity;
	CPM_Vector3											m_accel;

	//기저 축
	CPRS_AXIS											m_sightAxis;

	//회전 쿼터니언
	CPM_Quaternion										m_qRot;

	//좌표계 변환 행렬
	CPM_Matrix											m_matWorld;
	CPM_Matrix											m_matView;
	CPM_Matrix											m_matProj;

	CPRS_CBUF_COORDCONV_MATSET							m_wvpMat;
	CPM_Matrix											m_BillTMat;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_pCBWVPMat;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_pCBBillMat;

	std::vector<CPGCParticleEmitter>					m_emitterList;

public:
	CPGCParticleEffect();

	CPGCParticleEffect(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDContext,
		CPRS_PSYSTEM_FILESTRUCT* pPSystem,
		CPM_Vector3 vPos,
		CPGCParticleEffectProperty& effectOpt);

	virtual ~CPGCParticleEffect();

	bool create(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDContext,
		CPRS_PSYSTEM_FILESTRUCT* pPSystem,
		CPM_Vector3 vPos,
		CPGCParticleEffectProperty& effectOpt);

	bool init() override;
	bool update() override;
	bool render() override;
	bool release() override;

	void setVelocity(CPM_Vector3 initV, CPM_Vector3 initA);
	void setMatrix(const CPM_Matrix* pWorld, const CPM_Matrix* pView, const CPM_Matrix* pProj) override;
	void bindPipeline();

	void updateBuffer(ID3D11Resource* pBuf, void* pResource, UINT iBufSize);
	void updateState();

	void reset();
};
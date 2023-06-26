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

	//�̹����� ��ġ
	CPM_Vector3										m_vPos;

	//�̹����� ȸ��
	CPM_Quaternion									m_qRot;
	CPM_Vector3										m_PitchYawRoll;

	//��ƼŬ ���� Ÿ�̸�
	float											m_fSpawnTime;
	float											m_fTimer;

	//��ƼŬ�� �Ӽ� ����
	CPRS_EmitterProperty*							m_pEProp;

	//���� �ɼ�
	//��ƼŬ ����Ʈ : �ϴ� ����Ʈ�� ������ ���� ����Ʈ�� ������ �����ϰ� ó���Ѵ�.
	std::vector<CPRSPointParticle>					m_particles;

	//��ƼŬ ���� ����
	std::vector<CPRS_ParticleVertex>				m_vertices;
	UINT											m_iVertexCount;
	UINT											m_iStride;
	UINT											m_iOffset;

	//���� ���̾ƿ� ����
	ID3D11InputLayout*								m_pVLayout;

	//���� ����
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pVBuf;

	//�ý��ۿ��� �Ѱܹ޴� ����ӵ�
	float											m_playSpeed;

	//�ؽ�ó & ��������Ʈ
	CPRS_Sprite* m_pSprite;
	CPRS_Texture* m_pTexture;
	CPRS_RECT										m_UVRect;

	//Shader
	CPRS_ShaderGroup								m_shaderGroup;

	//���� �ɼ� ������ ��뿩��
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
	//��ƼŬ ���� �ɼ�
	CPGCParticleEffectProperty							m_effectOpt;

	bool												m_bActivated;
	float												m_fElapsedTime;

	CPRS_PSYSTEM_FILESTRUCT*							m_pPSystem;

	//����Ʈ ��ġ ��ġ
	CPM_Vector3											m_vInitPos;
	CPM_Vector3											m_vPos;

	//�ӵ� & ���ӵ�
	CPM_Vector3											m_initVelocity;
	CPM_Vector3											m_initAccel;

	CPM_Vector3											m_velocity;
	CPM_Vector3											m_accel;

	//���� ��
	CPRS_AXIS											m_sightAxis;

	//ȸ�� ���ʹϾ�
	CPM_Quaternion										m_qRot;

	//��ǥ�� ��ȯ ���
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
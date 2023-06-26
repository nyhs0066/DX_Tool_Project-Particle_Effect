#pragma once
#include "CPGCObject.h"
#include "CPGCSpriteManager.h"

#define EFFECT_MAX_ANIMKEY					32

struct CPRS_CBUF_BILLBOARDMAT
{
	CPM_Matrix								m_billboardTMat;
};

struct CPRSParticleAnimKey
{
	//72Byte

	CPM_Vector3			vPos;			
	CPM_Vector3			vRot;			
	CPM_Quaternion		qRot;			
	CPM_Vector3			vScale;			
	CPM_Color			vColor;			
	float				fTime;			

	CPM_Matrix getT() { return CPM_Matrix::CreateTranslation(vPos); };
	CPM_Matrix getR() { return CPM_Matrix::CreateFromQuaternion(qRot); };
	CPM_Matrix getS() { return CPM_Matrix::CreateScale(vScale); };

	CPM_Matrix getWorld() { return CPM_Matrix::CreateScale(vScale) * CPM_Matrix::CreateFromQuaternion(qRot) * CPM_Matrix::CreateTranslation(vPos); };
};

enum CPRS_EMITTER_TYPE
{
	DEFAULT_PARTICLE,
	PARTICLE_TRAIL,
	BURST_PARTICLE,
	NUMBER_OF_EMITTER_TYPE
};

struct CPRS_DeprecatedEmitterProperty
{
	//2548 Byte

	//��ƼŬ �ʱ� Ʈ������
	CPM_Vector3			vInitPos;							
	CPM_Vector3			vMinPosOffset;						
	CPM_Vector3			vMaxPosOffset;						

	CPM_Vector3			fInitPYR;							

	CPM_Vector3			fExtraPYRMinRange;					
	CPM_Vector3			fExtraPYRMaxRange;					

	CPM_Vector2			vInitScale;							
	CPM_Vector4			vExtraScaleRange;					

	//��ƼŬ ����
	int					iMaxParticleNum;					
	float				fSpawnRate;							
	CPM_Vector2			fLifespanRange;						

	//��ƼŬ ���� ��ȭ
	CPM_Vector3			vInitMinVelocity;					
	CPM_Vector3			vInitMaxVelocity;					

	CPM_Vector3			fMinPYRVelocity;					
	CPM_Vector3			fMaxPYRVelocity;					

	CPM_Vector4			fScaleOverLifespan;					

	CPM_Vector3			vInitMinAcceleration;				
	CPM_Vector3			vInitMaxAcceleration;				

	CPM_Color			initColorOverLifespan;				
	CPM_Color			lastColorOverLifespan;				

	//����� ������ ��� 
	int					iUseBillBoard;						

	//��ƼŬ ���� ����
	bool				bShow;								

	//���� ����
	bool				bLoop;								

	//��ƼŬ �ִϸ��̼��� ���� Ű ����Ʈ & Ű ���� : ����Ʈ �ɼ����ε� ����Ѵ�.
	int					iAnimKeyCnt;						
	CPRSParticleAnimKey animkeyList[EFFECT_MAX_ANIMKEY];

	CPRS_DeprecatedEmitterProperty()
	{
		bShow = true;
		bLoop = false;
		iUseBillBoard = 0;
		vInitScale = { 1.0f, 1.0f };
		iMaxParticleNum = 0;
		fSpawnRate = 0.0f;

		iAnimKeyCnt = 0;
	}
};

struct CPRS_EmitterProperty
{
	//��ƼŬ �ʱ� Ʈ������
	CPM_Vector3			vInitPos;
	CPM_Vector3			vMinPosOffset;
	CPM_Vector3			vMaxPosOffset;

	CPM_Vector3			fInitPYR;

	CPM_Vector3			fExtraPYRMinRange;
	CPM_Vector3			fExtraPYRMaxRange;

	CPM_Vector2			vInitScale;
	CPM_Vector4			vExtraScaleRange;

	//��ƼŬ ����
	int					iMaxParticleNum;
	float				fSpawnRate;
	CPM_Vector2			fLifespanRange;

	//��ƼŬ ���� ��ȭ
	CPM_Vector3			vInitMinVelocity;
	CPM_Vector3			vInitMaxVelocity;

	CPM_Vector3			fMinPYRVelocity;
	CPM_Vector3			fMaxPYRVelocity;

	CPM_Vector4			fScaleOverLifespan;

	CPM_Vector3			vInitMinAcceleration;
	CPM_Vector3			vInitMaxAcceleration;

	CPM_Color			initColorOverLifespan;
	CPM_Color			lastColorOverLifespan;

	//����� ������ ��� 
	int					iUseBillBoard;

	//��ƼŬ ���� ����
	bool				bShow;

	//���� ����
	bool				bLoop;

	//��ƼŬ �ִϸ��̼��� ���� Ű ����Ʈ & Ű ����
	int					iAnimKeyCnt;
	CPRSParticleAnimKey animkeyList[EFFECT_MAX_ANIMKEY];

	CPRS_EmitterProperty()
	{
		bShow = true;
		bLoop = false;
		iUseBillBoard = 0;
		vInitScale = { 1.0f, 1.0f };
		iMaxParticleNum = 0;
		fSpawnRate = 0.0f;

		iAnimKeyCnt = 0;
	}
};

struct CPRSPointParticle
{
	bool			bEnable;
	bool			bLoop;

	CPM_Vector3		vPos;
	CPM_Vector3		vVelocity;
	CPM_Vector3		vAccelelation;
	CPM_Vector2		vInitScale;
	CPM_Vector2		vScale;

	CPM_Vector3		fPYR;
	CPM_Vector3		fPYRVelocity;

	CPM_Color		initColor;
	CPM_Color		lastColor;

	float			fLifeSpan;

	float			fElapsedTime;

	CPRSPointParticle();

	void setParticle(CPRS_EmitterProperty& eProp, CPM_Vector3 worldScaleFactor = {1.0f, 1.0f, 1.0f});
	void setParticle(CPRS_EmitterProperty* eProp, CPM_Vector3 worldScaleFactor = { 1.0f, 1.0f, 1.0f });

	//Ʈ���� ��ƼŬ�� ���� ��ǥ �̵��� ��ķ� �ݿ����� �ʴ� ��� �ʱ� ���� ��ǥ�� �Ѱ��ش�.
	void setPivotPosParticle(CPRS_EmitterProperty& eProp, CPM_Vector3 vInitWorldPos, CPM_Vector3 worldScaleFactor = { 1.0f, 1.0f, 1.0f });
	void setPivotPosParticle(CPRS_EmitterProperty* eProp, CPM_Vector3 vInitWorldPos, CPM_Vector3 worldScaleFactor = { 1.0f, 1.0f, 1.0f });
};

class CPRSEmitter : public CPGCObject
{
public:
	CPRS_EMITTER_TYPE						m_emitterType;

	//�̹����� ��ġ
	CPM_Vector3								m_vPos;
	CPM_Vector3								m_vPrevPos;

	//�̹����� ȸ��
	CPRS_AXIS								m_sightAxis;
	CPRS_BASE_PHYSICS_STATE					m_physicState;

	//��ƼŬ�� �Ӽ� ����
	CPRS_EmitterProperty					m_eProp;

	CPRS_Sprite*							m_pSprite;
	CPRS_Texture*							m_pTexture;
	CPRS_RECT								m_UVRect;

	//���� �ɼ� ������ ��뿩��
	int										m_iBlendStateOption;
	bool									m_bEnableDepth;
	bool									m_bEnableDepthWrite;
	bool									m_bBackCull;

	//�̹��� Ȱ��ȭ ����
	bool									m_bActivate;

public:
	CPRSEmitter() { m_emitterType = CPRS_EMITTER_TYPE::DEFAULT_PARTICLE; }
	virtual ~CPRSEmitter() {}

	virtual bool	update(float dt) = 0;
	virtual void	setMatrix(const CPM_Matrix* pWorldM, const CPM_Matrix* pViewM, const CPM_Matrix* pProjM, const CPM_Matrix* pParentWorldM = nullptr) = 0;

	virtual HRESULT		createEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszTexName) = 0;

	virtual HRESULT		createUVSpriteEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszSpriteName) = 0;

	virtual HRESULT		createMTSpriteEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszSpriteName) = 0;

	virtual	void stateReset() = 0;

	virtual void setRenderOption(int iBlendStateOption,
		bool bEnableDepth,
		bool bEnableDepthWrite,
		bool bBackCull) = 0;

	virtual HRESULT		createBuffers() = 0;
};

class CPRSPointParticleEmitter : public CPRSEmitter
{
public:
	//������ۿ� ���ε��� ��ǥ�� ��ȯ ���
	CPRS_CBUF_COORDCONV_MATSET				m_wvpMat;

	//��ǥ�� ��ȯ ���
	CPM_Matrix								m_matWorld;
	CPM_Matrix								m_matView;
	CPM_Matrix								m_matProj;

	CPM_Matrix								m_matLocalWorld;
	CPM_Matrix								m_matParentWorld;

	//��ƼŬ ���� Ÿ�̸�
	float									m_fSpawnTime;
	float									m_fTimer;

	ID3D11SamplerState*						m_pSampler;
	CPRS_ShaderGroup						m_shaderGroup;

	//���� �ɼ�
	//��ƼŬ ����Ʈ : �ϴ� ����Ʈ�� ������ ���� ����Ʈ�� ������ �����ϰ� ó���Ѵ�.
	std::vector<CPRSPointParticle>			m_particles;

	//��ƼŬ ���� ����
	std::vector<CPRS_ParticleVertex>		m_vertices;
	UINT									m_iVertexCount;
	UINT									m_iStride;
	UINT									m_iOffset;

	//���� ����
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pVBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pCBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pCBuf_billboard;

	//���� ���̾ƿ� ����
	ID3D11InputLayout*						m_pVLayout;

	//������ ���
	CPRS_CBUF_BILLBOARDMAT					m_billboardMat;

public:
	CPRSPointParticleEmitter();
	virtual ~CPRSPointParticleEmitter();

	virtual bool		init();

	virtual bool		update(float dt);

	virtual bool		render();
	virtual bool		release();

	virtual void		bindToPipeline();

	virtual void		setMatrix(const CPM_Matrix* pWorldM, const CPM_Matrix* pViewM, const CPM_Matrix* pProjM, const CPM_Matrix* pParentWorldM = nullptr);
	virtual void		updateState();
	virtual void		updateCoordConvMat();

	virtual HRESULT		createEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszTexName);

	virtual HRESULT		createUVSpriteEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszSpriteName);

	virtual HRESULT		createMTSpriteEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszSpriteName);

	virtual void		setRenderOption(int iBlendStateOption, 
		bool bEnableDepth, 
		bool bEnableDepthWrite, 
		bool bBackCull);

	virtual HRESULT		updateBuffer(ID3D11Resource* pBuf, void* pResource, UINT iBufSize);

	virtual void		activateTarget(float dt, bool bActivate = true);
	virtual void		updateParticleState(int i, float dt);

	virtual void		stateReset();

	virtual HRESULT		createBuffers();
};

class CPRSParticleTrailEmitter : public CPRSPointParticleEmitter
{
public:
	int		m_iActivateIdx;
public:
	CPRSParticleTrailEmitter();
	virtual ~CPRSParticleTrailEmitter();

	virtual bool	init();
	virtual bool	update(float dt);

	virtual HRESULT		createEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszTexName);

	virtual HRESULT		createUVSpriteEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszSpriteName);

	virtual HRESULT		createMTSpriteEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszSpriteName);

	virtual void	activateTarget(float dt, bool bActivate = true);
};

class CPRSParticleBurstEmitter : public CPRSPointParticleEmitter
{
public:
	int m_iActivateIdx;
public:
	CPRSParticleBurstEmitter();
	virtual ~CPRSParticleBurstEmitter();

	virtual bool	init();
	virtual bool	update(float dt);

	/*virtual HRESULT		createEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszTexName);

	virtual HRESULT		createUVSpriteEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszSpriteName);

	virtual HRESULT		createMTSpriteEmitter(CPRS_EmitterProperty eProp,
		std::wstring wszSpriteName);*/

	virtual void	activateTarget(float dt, bool bActivate = true);

	virtual void	stateReset();
};
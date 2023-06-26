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

	//파티클 초기 트랜스폼
	CPM_Vector3			vInitPos;							
	CPM_Vector3			vMinPosOffset;						
	CPM_Vector3			vMaxPosOffset;						

	CPM_Vector3			fInitPYR;							

	CPM_Vector3			fExtraPYRMinRange;					
	CPM_Vector3			fExtraPYRMaxRange;					

	CPM_Vector2			vInitScale;							
	CPM_Vector4			vExtraScaleRange;					

	//파티클 스폰
	int					iMaxParticleNum;					
	float				fSpawnRate;							
	CPM_Vector2			fLifespanRange;						

	//파티클 상태 변화
	CPM_Vector3			vInitMinVelocity;					
	CPM_Vector3			vInitMaxVelocity;					

	CPM_Vector3			fMinPYRVelocity;					
	CPM_Vector3			fMaxPYRVelocity;					

	CPM_Vector4			fScaleOverLifespan;					

	CPM_Vector3			vInitMinAcceleration;				
	CPM_Vector3			vInitMaxAcceleration;				

	CPM_Color			initColorOverLifespan;				
	CPM_Color			lastColorOverLifespan;				

	//사용할 빌보드 행렬 
	int					iUseBillBoard;						

	//파티클 가시 여부
	bool				bShow;								

	//루프 여부
	bool				bLoop;								

	//파티클 애니메이션을 위한 키 리스트 & 키 개수 : 버스트 옵션으로도 사용한다.
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
	//파티클 초기 트랜스폼
	CPM_Vector3			vInitPos;
	CPM_Vector3			vMinPosOffset;
	CPM_Vector3			vMaxPosOffset;

	CPM_Vector3			fInitPYR;

	CPM_Vector3			fExtraPYRMinRange;
	CPM_Vector3			fExtraPYRMaxRange;

	CPM_Vector2			vInitScale;
	CPM_Vector4			vExtraScaleRange;

	//파티클 스폰
	int					iMaxParticleNum;
	float				fSpawnRate;
	CPM_Vector2			fLifespanRange;

	//파티클 상태 변화
	CPM_Vector3			vInitMinVelocity;
	CPM_Vector3			vInitMaxVelocity;

	CPM_Vector3			fMinPYRVelocity;
	CPM_Vector3			fMaxPYRVelocity;

	CPM_Vector4			fScaleOverLifespan;

	CPM_Vector3			vInitMinAcceleration;
	CPM_Vector3			vInitMaxAcceleration;

	CPM_Color			initColorOverLifespan;
	CPM_Color			lastColorOverLifespan;

	//사용할 빌보드 행렬 
	int					iUseBillBoard;

	//파티클 가시 여부
	bool				bShow;

	//루프 여부
	bool				bLoop;

	//파티클 애니메이션을 위한 키 리스트 & 키 개수
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

	//트레일 파티클은 월드 좌표 이동을 행렬로 반영하지 않는 대신 초기 월드 좌표를 넘겨준다.
	void setPivotPosParticle(CPRS_EmitterProperty& eProp, CPM_Vector3 vInitWorldPos, CPM_Vector3 worldScaleFactor = { 1.0f, 1.0f, 1.0f });
	void setPivotPosParticle(CPRS_EmitterProperty* eProp, CPM_Vector3 vInitWorldPos, CPM_Vector3 worldScaleFactor = { 1.0f, 1.0f, 1.0f });
};

class CPRSEmitter : public CPGCObject
{
public:
	CPRS_EMITTER_TYPE						m_emitterType;

	//이미터의 위치
	CPM_Vector3								m_vPos;
	CPM_Vector3								m_vPrevPos;

	//이미터의 회전
	CPRS_AXIS								m_sightAxis;
	CPRS_BASE_PHYSICS_STATE					m_physicState;

	//파티클의 속성 정보
	CPRS_EmitterProperty					m_eProp;

	CPRS_Sprite*							m_pSprite;
	CPRS_Texture*							m_pTexture;
	CPRS_RECT								m_UVRect;

	//렌더 옵션 프리셋 사용여부
	int										m_iBlendStateOption;
	bool									m_bEnableDepth;
	bool									m_bEnableDepthWrite;
	bool									m_bBackCull;

	//이미터 활성화 여부
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
	//상수버퍼에 바인딩할 좌표계 변환 행렬
	CPRS_CBUF_COORDCONV_MATSET				m_wvpMat;

	//좌표계 변환 행렬
	CPM_Matrix								m_matWorld;
	CPM_Matrix								m_matView;
	CPM_Matrix								m_matProj;

	CPM_Matrix								m_matLocalWorld;
	CPM_Matrix								m_matParentWorld;

	//파티클 생성 타이머
	float									m_fSpawnTime;
	float									m_fTimer;

	ID3D11SamplerState*						m_pSampler;
	CPRS_ShaderGroup						m_shaderGroup;

	//렌더 옵션
	//파티클 리스트 : 일단 리스트의 순서는 정점 리스트의 순서와 동일하게 처리한다.
	std::vector<CPRSPointParticle>			m_particles;

	//파티클 정점 정보
	std::vector<CPRS_ParticleVertex>		m_vertices;
	UINT									m_iVertexCount;
	UINT									m_iStride;
	UINT									m_iOffset;

	//버퍼 정보
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pVBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pCBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pCBuf_billboard;

	//정점 레이아웃 정보
	ID3D11InputLayout*						m_pVLayout;

	//빌보드 행렬
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
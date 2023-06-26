#pragma once
#include "CPRSPaticle.h"
#include "CPGCCamera.h"

#define EFFECT_PROP_PATH_LIMIT	256
#define EFFECT_MAX_NUMBER_OF_EMITTER	32

enum CPRS_PARTICLESYSTEM_CULLMODE
{
	CPRS_PSYSTEM_CULLMODE_PAUSE,
	CPRS_PSYSTEM_CULLMODE_ALWAYS,
	NUMBER_OF_CPRS_PSYSTEM_CULLMODE
};

struct CPRS_ParticlesystemProperty
{
	float	fDuration;
	bool	bInfinitePlay;
	float	fStDelay;
	float	fPlaySpeed;
	int		iCullMode;

	CPRS_ParticlesystemProperty()
	{
		fDuration = 10.0f;
		bInfinitePlay = true;
		fStDelay = 0.0f;
		fPlaySpeed = 1.0f;
		iCullMode = CPRS_PARTICLESYSTEM_CULLMODE::CPRS_PSYSTEM_CULLMODE_PAUSE;
	}
};

struct CPRS_PSEMITTER_PREVVERFILESTRUCT
{
	//5144 Byte

	CPRS_EMITTER_TYPE				emitterType;

	WCHAR							wszEmitterName[EFFECT_PROP_PATH_LIMIT];

	WCHAR							wszSpriteName[EFFECT_PROP_PATH_LIMIT];
	WCHAR							wszSpritePath[EFFECT_PROP_PATH_LIMIT];

	WCHAR							wszTextureName[EFFECT_PROP_PATH_LIMIT];
	WCHAR							wszTexturePath[EFFECT_PROP_PATH_LIMIT];

	CPM_Vector3						vEmitterPos;

	UINT							iSpriteType;
	UINT							spriteRC[2];

	int								iBlendStateOption;
	bool							bEnableDepth;
	bool							bEnableDepthWrite;
	bool							bBackCull;

	CPRS_DeprecatedEmitterProperty	eProp;
};

struct CPRS_PSYSTEM_PREVVERLOADFILESTRUCT
{
	//165,214 Byte

	WCHAR								wszPSystemName[256];
	CPRS_PSEMITTER_PREVVERFILESTRUCT	emitters[EFFECT_MAX_NUMBER_OF_EMITTER];
	int									iEmiiterCnt;
};

struct CPRS_PSEMITTER_FILESTRUCT
{
	//5144 Byte

	CPRS_EMITTER_TYPE				emitterType;

	WCHAR							wszEmitterName[EFFECT_PROP_PATH_LIMIT];

	WCHAR							wszSpriteName[EFFECT_PROP_PATH_LIMIT];
	WCHAR							wszSpritePath[EFFECT_PROP_PATH_LIMIT];

	WCHAR							wszTextureName[EFFECT_PROP_PATH_LIMIT];
	WCHAR							wszTexturePath[EFFECT_PROP_PATH_LIMIT];

	CPM_Vector3						vEmitterPos;

	UINT							iSpriteType;
	UINT							spriteRC[2];

	int								iBlendStateOption;
	bool							bEnableDepth;
	bool							bEnableDepthWrite;
	bool							bBackCull;

	CPRS_EmitterProperty			eProp;
};

struct CPRS_PSYSTEM_FILESTRUCT
{
	//165,214 Byte

	WCHAR							wszPSystemName[256];
	CPRS_PSEMITTER_FILESTRUCT		emitters[EFFECT_MAX_NUMBER_OF_EMITTER];
	int								iEmiiterCnt;
};

class CPGCParticleSystem : public CPGCObject
{
public:
	CPRS_ParticlesystemProperty							m_PSProp;
	std::vector<CPRSEmitter*>							m_pEmitterList;

	float												m_fElapsedTime;

	CPM_Matrix											m_matWorld;
	CPM_Matrix											m_matView;
	CPM_Matrix											m_matProj;

	CPM_Matrix											m_matLocalWorld;
	CPM_Matrix											m_matParentWorld;

public:
	CPGCParticleSystem();
	~CPGCParticleSystem();

	bool create(std::wstring wszPSystemName,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDContext);

	bool init();
	bool update();
	bool setMatrix(const CPM_Matrix* pWorld, const CPM_Matrix* pView, const CPM_Matrix* pProj, const CPM_Matrix* pParentWorld = nullptr);
	bool render();
	bool release();

	void stateReset();

	CPRSEmitter* getPtr(std::wstring wszEmitterName);
};


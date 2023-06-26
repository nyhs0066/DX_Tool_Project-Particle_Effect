#pragma once
#include "CPGameCore.h"
#include "CPImGuiControls.h"

#define	CPIMGUI_DEFAULT_ITEM_WIDTH_OFFSET	40.0f

#define CPCPIMGUI_MAX_ANIM_TIME				1000.0f
#define CPCPIMGUI_MIN_ANIM_TIME				0.0f

#define CPIMGUI_MAX_PSYSTEM_DURATION		100.0f
#define CPIMGUI_MIN_PSYSTEM_DURATION		0.0f

#define CPIMGUI_MAX_ITEM_POS				100.0f
#define CPIMGUI_MIN_ITEM_POS				-100.0f

#define CPIMGUI_MAX_ITEM_COUNT				100
#define CPIMGUI_MIN_ITEM_COUNT				1

#define CPIMGUI_MAX_ITEM_VELOCITY			200.0f
#define CPIMGUI_MIN_ITEM_VELOCITY			-200.0f

#define CPIMGUI_MAX_ITEM_ROTVELOCITY		360.0f
#define CPIMGUI_MIN_ITEM_ROTVELOCITY		-360.0f

#define CPIMGUI_MAX_ITEM_LIFESPAN			7.5f
#define CPIMGUI_MIN_ITEM_LIFESPAN			0.5f

#define CPIMGUI_MAX_ITEM_SCALE				10.0f
#define CPIMGUI_MIN_ITEM_SCALE				0.001f

#define CPIMGUI_COLOR_UPPERBOUND			1.0f
#define CPIMGUI_COLOR_LOWERBOUND			0.0f

#define DEFAULT_EFFECT_DIRECTORY_WSZPATH	L"../../data/save/particlesystem/"
#define DEFAULT_EFFECT_DIRECTORY_SZPATH		"../../data/save/particlesystem/"

#define DEFAULT_EFFECT_DIRECTORY_WSZPATH	L"../../data/save/particlesystem/"
#define DEFAULT_EFFECT_DIRECTORY_SZPATH		"../../data/save/particlesystem/"

#define CPIMGUI_STRBUF_ELEMENT_LIMIT		EFFECT_PROP_PATH_LIMIT

#define CPIMGUI_PATH_WSTRING_BYTE_LIMIT		(sizeof(wchar_t) * CPIMGUI_STRBUF_ELEMENT_LIMIT)
#define CPIMGUI_PATH_STRING_BYTE_LIMIT		(sizeof(char) * CPIMGUI_STRBUF_ELEMENT_LIMIT)

class CPEffectWorkspace
{
private:
	ID3D11Device*									m_pDevice;
	ID3D11DeviceContext*							m_pDContext;

	CPGCCamera*										m_pMainCam;
	CPGCRenderTarget*								m_pRT;

	//�ؽ�ó �˻� �� ����
	CPIMGUITexSelector*								m_pEffectTexSelector;
	CPIMGUITexSelector*								m_pCreateUVSpriteTexSelector;
	CPIMGUIMultiTexSelector*						m_pCreateMTSpriteTexSelector;
	std::string										m_szMTSpriteName;

	//��������Ʈ ����
	std::vector<std::string>						m_szUVNames;
	std::string										m_szSelectedUVName;
	std::wstring									m_wszSelectedUVName;
	int												m_iUVNameIdx;

	std::vector<std::string>						m_szMTNames;
	std::string										m_szSelectedMTName;
	std::wstring									m_wszSelectedMTName;
	int												m_iMTNameIdx;

	//�۾� ��� ��ƼŬ �ý���
	CPGCParticleSystem*								m_pCurrentPSystem;
	int												m_selectedEmitterIdx;
	int												m_emitterSelectionMask;
	CPRSEmitter*									m_pSelectedEmitter;
	CPIMGUITexSelector*								m_pEmitterTexSelector;

	int												m_iTexTypeFlag;

	std::string										m_szSelectedEmitterUVName;
	std::wstring									m_wszSelectedEmitterUVName;
	int												m_iEmitterUVNameIdx;

	std::string										m_szSelectedEmitterMTName;
	std::wstring									m_wszSelectedEmitterMTName;
	int												m_iEmitterMTNameIdx;

	CPM_Vector3										m_vPos;
	CPM_Vector3										m_vVelocity;

	CPM_Vector3										m_vPYRRot;
	CPM_Vector3										m_vScale;

	CPM_Vector3										m_initVelocity;
	CPM_Vector3										m_initAcceleration;

	float											m_fLimitDist;

	CPM_Matrix										world;

	//����Ʈ ���� ����
	std::string										m_szCreateEmitterName;
	CPRS_EMITTER_TYPE								m_iCreateEmitterType;

	CPRS_EmitterProperty							m_ECProp;

	CPRS_BSType										m_iECBlendOpt;
	bool											m_bECEnableDepthOpt;
	bool											m_bECEnableDepthWriteOpt;
	bool											m_bECEnableBackCullOpt;

	std::vector<std::string>						m_multiTex;

	//���� �÷��� ���� : 0 ���� �ؽ�ó / 1 : UV �ؽ�ó / 2 : ��Ƽ �ؽ�ó
	int												m_iCreateFlag;
	int												m_iECRowNum;
	int												m_iECColNum;

	CPEffectWorkspace();
	~CPEffectWorkspace();
public:
	CPEffectWorkspace(const CPEffectWorkspace& other) = delete;
	CPEffectWorkspace& operator=(const CPEffectWorkspace& other) = delete;

public:
	bool init(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDContext,
		CPGCRenderTarget* pMainRT,
		CPGCCamera* pMainCam);

	bool preUpdate();
	bool update();
	bool postUpdate();

	bool preRender();
	bool render();
	bool postRender();

	bool release();

	HRESULT createDefaultEmitter();
	HRESULT createTrailEmitter();
	HRESULT createBurstEmitter();

	void setMainCam(CPGCCamera* pCam);

	//���� â �Լ�
	void openWorkspaceWindow();
	void showParticleSystemWindow();
	void showEmitterListWindow();
	void showEmitterPropertyWindow();
	void showCreateSpriteWindow();

	void showDefaultEmitterPropWindow(CPRSPointParticleEmitter* pTargetEmitter);
	void showTrailEmitterPropWindow(CPRSParticleTrailEmitter* pTargetEmitter);
	void showBurstEmitterPropWindow(CPRSParticleBurstEmitter* pTargetEmitter);
	
	void createNewParticleSystemPopup(int& popOpt, bool& bOpenWorkspace);
	void loadParticleSystemPopup(int& popOpt, bool& bOpenWorkspace, int loadOpt);
	void saveParticleSystemPopup(int& popOpt);
	bool loadPSBinaryFile(std::wstring wszPSystemFilepath, int loadOpt);

	bool savePSBinaryFile(std::wstring wszPSystemFilepath);

	bool convertFile(const CPRS_PSYSTEM_PREVVERLOADFILESTRUCT& prevfile, CPRS_PSYSTEM_FILESTRUCT& outFile);
	void convertEmitterProp(const CPRS_DeprecatedEmitterProperty& in, CPRS_EmitterProperty& out);
	bool convertDirectory(std::wstring targetDirPath, std::wstring outDirPath);

	static CPEffectWorkspace& getInstance()
	{
		static CPEffectWorkspace singleInst;
		return singleInst;
	}
};

#define EFFECT_SPACE CPEffectWorkspace::getInstance()


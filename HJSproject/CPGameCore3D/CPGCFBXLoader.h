/////////////////////////////////////////////////////
//
//  CPGCFBXLoader.h
//	확장자가 .FBX인 3D 모델 및 애니메이션 렌더를 위한 정보를 추출하는 헤더
// 
//	좌표계 변경 및 시스템 거리 단위 변경 
//		FbxAxisSystem ConvertScene()
//			루트 FBX node의 회전 행렬만 변경하고자 하는 좌표계의 축으로 반영한다.
//			LH / RH를 변경하는 것이 아님에 유의
//		FbxAxisSystem DeepConvertScene()
//			행렬, 정점 위치, 애니메이션 커브와 속성들의 상속 구조 까지 수정한다.
//			LH / RH까지 변경한다.
//		FbxSystemUnit ConvertScene()
//			기본단위 cm : 모델 1 : 시스템 유닛(글로벌) 1의 비율, 모델 좌표계에서의 1단위를 글로벌 좌표계 1단위로 간주.
//			스케일 행렬에 영향을 미친다.
// 
//	Mesh Deformer : 많은 수의 정점으로 이루어진 복잡한 메시를 조작하기 위해 사용하는 기능 도구
//		고밀도의 복잡한 메시를 둘러싸는 프레임 메시를 조작하는 것으로 메시를 쉽게 변형시킨다.
//		이 때, 이러한 프레임 메시를 Deformation Cage라고 한다.
//		여기서는 Deformation Cage == Cluster에 해당한다.
// 
//		FbxDeformer는 두가지 타입으로 나뉜다. 
//			VertexCache
//			Skin
//				FbxSkin은 다수의 FbxCluster로 구성되며 FbxCluster는 영향을 받는 bone의 노드(LinkNode)와 
//				1:1대응 되며 영향을 받는 제어점 정보, 트랜스폼, 결합 가중치도 함께 가진다.
//	
/////////////////////////////////////////////////////

#pragma once
#include "CPRSBaseStruct.h"
#include "CPGCFbxStruct.h"
#include "CPRSMeshManager.h"
#include "CPGCTextureManager.h"
#include "CPGCShaderManager.h"
#include "CPGCFileIO.h"
#include "CPGCTimer.h"
//#include "CPGCBVRender.h"

class CPRSFBXRenderUnit
{
public:
	//로드 정보부

	//유닛 식별 정보
	std::wstring											m_wszName;
	int														m_idx;

	//메시 리소스 변수
	std::vector<std::vector<CPRS_SkinningVertex>>			m_vertexGroups;
	std::vector<std::vector<UINT>>							m_IndexGroups;

	//리소스 카운트
	std::vector<UINT>										m_iVertexCountArray;
	std::vector<UINT>										m_iIndexCountArray;
	UINT													m_iCBufCount;

	//텍스처 이름 및 경로 리스트
	bool													m_isExistTexture;	//텍스처가 하나라도 로드 됐는지 유무
	std::vector<std::wstring>								m_wszTexNameList;
	std::vector<std::wstring>								m_wszTexPathList;

	//유닛 스키닝 애니메이션 정보
	bool													m_bSkinned;
	std::map<int, CPRS_BONEMATRIX>							m_LocalBoneMatMap;

	//유닛 애니메이션 정보
	std::vector<CPRS_AnimTrack>								m_animTracks;			//프레임단위로 적용되는 전역 트랜스폼 리스트

	//자동 생성부
	
	//DX 인터페이스
	ID3D11Device*											m_pDevice;
	ID3D11DeviceContext*									m_pDContext;

	//정점버퍼와 기하 기본요소 세팅을 위한 추가 변수	
	UINT													m_iStride;
	UINT													m_iOffset;
	D3D11_PRIMITIVE_TOPOLOGY								m_primitive;

	//텍스처
	std::vector<CPRS_Texture*>								m_pTextureList;

	//셰이더와 기본 샘플러 및 정점 레이아웃
	CPRS_ShaderGroup										m_shaderGroup;
	ID3D11PixelShader*										m_pNoTexPS;
	ID3D11SamplerState*										m_pSampler;
	ID3D11InputLayout*										m_pVLayout;

	//정점 버퍼
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>		m_pVBufs;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>		m_pIBufs;

	CPRSFBXRenderUnit();
	virtual ~CPRSFBXRenderUnit();

	virtual HRESULT											create(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDContext,
		CPGCFBXLoadUnit* pLoadUnit,
		CPGCFBXLoadData* pLoadData);

	virtual bool											init();
	virtual bool											release();

	virtual void											setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);
	virtual void											draw();
	virtual void											draw(CPRS_ShaderGroup& sGruop, ID3D11PixelShader* pNoTexPS);
	virtual void											instanceDraw();

	//프레임간 행렬 보간
	virtual CPM_Matrix										frameInterpolate(FLOAT fFrame, CPRS_AnimInfo& animInfo, bool bLoop);
};

class CPRSFBXMesh
{
public:
	CPRS_AnimInfo													m_animInfo;

	std::vector<CPRSFBXRenderUnit*>									m_renderMeshUnitList;

	std::map<std::string, std::vector<CPRS_AnimTrack>>				m_fbxUnitMap;
	std::map<std::string, int>										m_fbxUnitIdxMap;

	CPRSFBXMesh();
	~CPRSFBXMesh();

	virtual HRESULT											create(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDContext,
		CPGCFBXLoadData* pLoadData);


	virtual bool											init();
	virtual bool											release();

	virtual void											draw();	
	virtual void											draw(CPRS_ShaderGroup& sGruop, ID3D11PixelShader* pNoTexPS);

	virtual void											instanceDraw();
};

struct CPRSFBXAnimation
{
	CPRS_AnimInfo													m_animInfo;

	std::map<std::string, std::vector<CPRS_AnimTrack>>				m_fbxUnitMap;
	std::map<std::string, int>										m_fbxUnitIdxMap;
};

class CPGCFBXObject
{
public:
	//DX 인터페이스
	ID3D11Device*											m_pDevice;
	ID3D11DeviceContext*									m_pDContext;

	CPM_Vector3												m_vPos;
	CPM_Vector3												m_vPrevPos;

	CPRS_AXIS												m_sightAxis;

	//mesh & Animation
	CPRSFBXMesh*											m_pFBXMesh;
	CPRSFBXAnimation*										m_pFBXAnimation;

	//에니메이션을 위한 변수
	CPRS_AnimState											m_animState;

	CPM_Matrix												m_matWorld;
	CPM_Matrix												m_matView;
	CPM_Matrix												m_matProj;

	//상수버퍼 세팅 : 0번 버퍼 사용
	CPRS_CBUF_COORDCONV_MATSET								m_wvpMat;

	//애니메이션 행렬 상수 버퍼 리소스 : 1번 슬롯, m_pCAnimBufs 사용
	std::vector<CPRS_CBUF_ANIMATION_MATS>					m_animMats;

	//상수 버퍼
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>		m_pCBufs;				//VS단계용 상수버퍼 그룹
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>		m_pCAnimBufs;			//애니메이션 행렬을 위한 부분

	//기초 피직스 세팅
	CPRS_BASE_PHYSICS_STATE									m_physicsState;

	CPGCFBXObject();
	virtual ~CPGCFBXObject();

	virtual bool				init();
	virtual bool				update();
	virtual bool				preRender();
	virtual bool				render();
	virtual bool				render(CPRS_ShaderGroup& sGruop, ID3D11PixelShader* pNoTexPS);
	virtual bool				instanceRender();
	virtual bool				release();

	virtual HRESULT				setDeviceAndMesh(std::wstring wszFBXMeshName, ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);
	virtual bool				setAnimation(std::wstring wszFBXAnimationName);

	virtual void				setMatrix(const CPM_Matrix* pWorldM, const CPM_Matrix* pViewM, const CPM_Matrix* pProjM);

	virtual void				updateState();
	virtual void				updateCoordConvMat(const CPM_Matrix& matWorld, const CPM_Matrix& matView, const CPM_Matrix& matProj);
	virtual void				updateAnimMats();
	virtual CPM_Matrix			frameInterpolate(FLOAT fFrame, std::vector<CPRS_AnimTrack>& animTrack, CPRS_AnimInfo& animInfo);
};


class CPGCFBXManager
{
private:
	ID3D11Device*											m_pDevice;
	ID3D11DeviceContext*									m_pDContext;

	FbxManager*												m_pFBXMgr;
	FbxImporter*											m_pFBXImporter;

	FbxAxisSystem											m_axisSystem;
	FbxSystemUnit											m_sysUnit;
	FbxTime::EMode											m_timemode;

	std::set<std::wstring>									m_scriptFileNameSet;
	std::map<std::wstring, CPGCFBXLoadData*>				m_FBXDataMap;
	std::map<std::wstring, std::wstring>					m_FBXFileMap;

	std::map<std::wstring, CPRSFBXMesh*>					m_FBXMeshMap;
	std::map<std::wstring, CPRSFBXAnimation*>				m_FBXAnimationMap;

	//임시 사용변수 - 로드가 끝나면 정리를 바로 해주자
	std::vector<FbxMesh*>									m_pMeshList;

	CPGCFBXManager();
	~CPGCFBXManager();

	void getAnimInfo(FbxScene* pScene, CPGCFBXLoadData* pLoadData);
	void preProcess(FbxNode* pNode, CPGCFBXLoadUnit* pParentUnit, CPGCFBXLoadData* pLoadData);

	void getAnimTrackInfo(FbxLongLong iFrame, FbxTime& frameTime, CPGCFBXLoadData* pLoadData);
	void getMeshSkinningInfo(FbxMesh* pMesh, CPGCFBXLoadUnit* pUnit, CPGCFBXLoadData* pLoadData);
	void getMeshRenderData(FbxMesh* pMesh, CPGCFBXLoadUnit* pUnit, CPGCFBXLoadData* pLoadData);

	UINT getSubMaterialIndex(UINT iPolygonIdx, FbxLayerElementMaterial* pMTSet);

	//정점의 속성을 받아오는 형식으로는
	//Control Point에서 직접 받아오는 형식(EMappingMode::eByControlPoint)과
	//폴리곤 단위의 정점에서 받아오는 형식(EMappingMode::eByPolygonVertex)두가지만 고려한다.
	FbxVector2 readTextureCoord(	FbxMesh* pFbxMesh,
									FbxLayerElementUV* pVertexUVSet,
									UINT VTIndex,
									UINT UVIndex);

	FbxColor readColor(				FbxMesh* pFbxMesh,
									FbxLayerElementVertexColor* pVTColorSet,
									UINT VTIndex,
									UINT colorIndex);

	FbxVector4 readNormal(			FbxMesh* pFbxMesh,
									FbxLayerElementNormal* pVTNormalSet,
									UINT VTIndex,
									UINT normalIndex);

public:
	CPGCFBXManager(const CPGCFBXManager& other) = delete;
	CPGCFBXManager& operator=(const CPGCFBXManager& other) = delete;

public:

	bool init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);
	bool release();
	bool loadFBXFile(std::wstring wszFBXName, std::wstring wszFBXFilename);

	CPM_Matrix fbxAffineMatToDXMatrix(const FbxAMatrix& srcMat);

	CPGCFBXLoadData* getLoadData(std::wstring wszFBXMeshName);
	CPRSFBXMesh* getMesh(std::wstring wszFBXMeshName);
	CPRSFBXAnimation* getAnimation(std::wstring wszFBXAnimationName);

	static CPGCFBXManager& getInstance()
	{
		static CPGCFBXManager singleInst;
		return singleInst;
	}
};

#define CPGC_FBX_MGR	CPGCFBXManager::getInstance()
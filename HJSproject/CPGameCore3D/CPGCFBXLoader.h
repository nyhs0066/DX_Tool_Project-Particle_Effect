/////////////////////////////////////////////////////
//
//  CPGCFBXLoader.h
//	Ȯ���ڰ� .FBX�� 3D �� �� �ִϸ��̼� ������ ���� ������ �����ϴ� ���
// 
//	��ǥ�� ���� �� �ý��� �Ÿ� ���� ���� 
//		FbxAxisSystem ConvertScene()
//			��Ʈ FBX node�� ȸ�� ��ĸ� �����ϰ��� �ϴ� ��ǥ���� ������ �ݿ��Ѵ�.
//			LH / RH�� �����ϴ� ���� �ƴԿ� ����
//		FbxAxisSystem DeepConvertScene()
//			���, ���� ��ġ, �ִϸ��̼� Ŀ��� �Ӽ����� ��� ���� ���� �����Ѵ�.
//			LH / RH���� �����Ѵ�.
//		FbxSystemUnit ConvertScene()
//			�⺻���� cm : �� 1 : �ý��� ����(�۷ι�) 1�� ����, �� ��ǥ�迡���� 1������ �۷ι� ��ǥ�� 1������ ����.
//			������ ��Ŀ� ������ ��ģ��.
// 
//	Mesh Deformer : ���� ���� �������� �̷���� ������ �޽ø� �����ϱ� ���� ����ϴ� ��� ����
//		��е��� ������ �޽ø� �ѷ��δ� ������ �޽ø� �����ϴ� ������ �޽ø� ���� ������Ų��.
//		�� ��, �̷��� ������ �޽ø� Deformation Cage��� �Ѵ�.
//		���⼭�� Deformation Cage == Cluster�� �ش��Ѵ�.
// 
//		FbxDeformer�� �ΰ��� Ÿ������ ������. 
//			VertexCache
//			Skin
//				FbxSkin�� �ټ��� FbxCluster�� �����Ǹ� FbxCluster�� ������ �޴� bone�� ���(LinkNode)�� 
//				1:1���� �Ǹ� ������ �޴� ������ ����, Ʈ������, ���� ����ġ�� �Բ� ������.
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
	//�ε� ������

	//���� �ĺ� ����
	std::wstring											m_wszName;
	int														m_idx;

	//�޽� ���ҽ� ����
	std::vector<std::vector<CPRS_SkinningVertex>>			m_vertexGroups;
	std::vector<std::vector<UINT>>							m_IndexGroups;

	//���ҽ� ī��Ʈ
	std::vector<UINT>										m_iVertexCountArray;
	std::vector<UINT>										m_iIndexCountArray;
	UINT													m_iCBufCount;

	//�ؽ�ó �̸� �� ��� ����Ʈ
	bool													m_isExistTexture;	//�ؽ�ó�� �ϳ��� �ε� �ƴ��� ����
	std::vector<std::wstring>								m_wszTexNameList;
	std::vector<std::wstring>								m_wszTexPathList;

	//���� ��Ű�� �ִϸ��̼� ����
	bool													m_bSkinned;
	std::map<int, CPRS_BONEMATRIX>							m_LocalBoneMatMap;

	//���� �ִϸ��̼� ����
	std::vector<CPRS_AnimTrack>								m_animTracks;			//�����Ӵ����� ����Ǵ� ���� Ʈ������ ����Ʈ

	//�ڵ� ������
	
	//DX �������̽�
	ID3D11Device*											m_pDevice;
	ID3D11DeviceContext*									m_pDContext;

	//�������ۿ� ���� �⺻��� ������ ���� �߰� ����	
	UINT													m_iStride;
	UINT													m_iOffset;
	D3D11_PRIMITIVE_TOPOLOGY								m_primitive;

	//�ؽ�ó
	std::vector<CPRS_Texture*>								m_pTextureList;

	//���̴��� �⺻ ���÷� �� ���� ���̾ƿ�
	CPRS_ShaderGroup										m_shaderGroup;
	ID3D11PixelShader*										m_pNoTexPS;
	ID3D11SamplerState*										m_pSampler;
	ID3D11InputLayout*										m_pVLayout;

	//���� ����
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

	//�����Ӱ� ��� ����
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
	//DX �������̽�
	ID3D11Device*											m_pDevice;
	ID3D11DeviceContext*									m_pDContext;

	CPM_Vector3												m_vPos;
	CPM_Vector3												m_vPrevPos;

	CPRS_AXIS												m_sightAxis;

	//mesh & Animation
	CPRSFBXMesh*											m_pFBXMesh;
	CPRSFBXAnimation*										m_pFBXAnimation;

	//���ϸ��̼��� ���� ����
	CPRS_AnimState											m_animState;

	CPM_Matrix												m_matWorld;
	CPM_Matrix												m_matView;
	CPM_Matrix												m_matProj;

	//������� ���� : 0�� ���� ���
	CPRS_CBUF_COORDCONV_MATSET								m_wvpMat;

	//�ִϸ��̼� ��� ��� ���� ���ҽ� : 1�� ����, m_pCAnimBufs ���
	std::vector<CPRS_CBUF_ANIMATION_MATS>					m_animMats;

	//��� ����
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>		m_pCBufs;				//VS�ܰ�� ������� �׷�
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>		m_pCAnimBufs;			//�ִϸ��̼� ����� ���� �κ�

	//���� ������ ����
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

	//�ӽ� ��뺯�� - �ε尡 ������ ������ �ٷ� ������
	std::vector<FbxMesh*>									m_pMeshList;

	CPGCFBXManager();
	~CPGCFBXManager();

	void getAnimInfo(FbxScene* pScene, CPGCFBXLoadData* pLoadData);
	void preProcess(FbxNode* pNode, CPGCFBXLoadUnit* pParentUnit, CPGCFBXLoadData* pLoadData);

	void getAnimTrackInfo(FbxLongLong iFrame, FbxTime& frameTime, CPGCFBXLoadData* pLoadData);
	void getMeshSkinningInfo(FbxMesh* pMesh, CPGCFBXLoadUnit* pUnit, CPGCFBXLoadData* pLoadData);
	void getMeshRenderData(FbxMesh* pMesh, CPGCFBXLoadUnit* pUnit, CPGCFBXLoadData* pLoadData);

	UINT getSubMaterialIndex(UINT iPolygonIdx, FbxLayerElementMaterial* pMTSet);

	//������ �Ӽ��� �޾ƿ��� �������δ�
	//Control Point���� ���� �޾ƿ��� ����(EMappingMode::eByControlPoint)��
	//������ ������ �������� �޾ƿ��� ����(EMappingMode::eByPolygonVertex)�ΰ����� ����Ѵ�.
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
#include "CPGCFBXLoader.h"

CPGCFBXManager::CPGCFBXManager()
{
	std::string temp("temp");

	m_pDevice = nullptr;
	m_pDContext = nullptr;

	m_pFBXMgr = nullptr;
	m_pFBXImporter = nullptr;
}

CPGCFBXManager::~CPGCFBXManager()
{
	m_pFBXMgr = nullptr;
	m_pFBXImporter = nullptr;
}

bool CPGCFBXManager::init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	if (!pDevice || !pDContext) { return false; }
	else
	{
		m_pDevice = pDevice;
		m_pDContext = pDContext;
	}

	//매니저 생성
	m_pFBXMgr = FbxManager::Create();
	if (!m_pFBXMgr) { return false; }

	//Importer생성
	m_pFBXImporter = FbxImporter::Create(m_pFBXMgr, "");	//이름은 별도로 사용하지 않는다.
	if (!m_pFBXImporter) { return false; }

	//로드 속성 변수 세팅
	m_axisSystem = FbxAxisSystem::MayaZUp;	
	m_sysUnit = FbxSystemUnit::m;
	m_timemode = FbxTime::EMode::eFrames30;

	return true;
}

bool CPGCFBXManager::release()
{
	for (auto it : m_FBXDataMap)
	{
		for (auto itUnit : it.second->fbxUnitMap)
		{
			delete itUnit.second;
			itUnit.second = nullptr;
		}

		delete it.second;
		it.second = nullptr;
	}

	for (auto it : m_FBXMeshMap)
	{
		it.second->release();
		delete it.second;
		it.second = nullptr;
	}

	for (auto it : m_FBXAnimationMap)
	{
		delete it.second;
		it.second = nullptr;
	}

	m_FBXMeshMap.clear();
	m_FBXAnimationMap.clear();

	m_FBXDataMap.clear();
	m_FBXFileMap.clear();

	m_pFBXImporter->Destroy();
	m_pFBXMgr->Destroy();

	return true;
}

bool CPGCFBXManager::loadFBXFile(std::wstring wszFBXName, std::wstring wszFBXFilename)
{
	if (m_FBXFileMap.find(wszFBXFilename) == m_FBXFileMap.end() &&
		m_FBXDataMap.find(wszFBXName) == m_FBXDataMap.end())
	{
		CPGCFBXLoadData* pFBXLoadData = new CPGCFBXLoadData;

		//1. Scene 생성 및 임포트
		FbxScene* pFBXScene = FbxScene::Create(m_pFBXMgr, "");
		bool bRet = true;

		std::wstring temp;
		getSplitDir(wszFBXFilename, temp, true);

		bRet = m_pFBXImporter->Initialize(atl_W2M(temp + L".FBX").c_str());
		if (!bRet)
		{
			bRet = m_pFBXImporter->Initialize(atl_W2M(temp + L".fbx").c_str());
			if (!bRet) { return false; }
		}

		bRet = m_pFBXImporter->Import(pFBXScene);
		if (!bRet)
		{
			delete pFBXLoadData;
			pFBXLoadData = nullptr;
			return false;
		}

		//2. 좌표계와 시스템 단위 변경
		m_axisSystem.ConvertScene(pFBXScene);
		m_sysUnit.ConvertScene(pFBXScene);

		//3. 루트 노드 불러오고 파싱을 위해 필요한 구조를 만들기 위한 전처리 수행
		FbxNode* pRootNode = pFBXScene->GetRootNode();
		preProcess(pRootNode, nullptr, pFBXLoadData);

		//4.만들어 놓은 메시 리스트를 이용해서 메시 및 애니메이션 파싱
		//애니메이션 추출을 위해 애니메이션 기본 정보 가져오기
		getAnimInfo(pFBXScene, pFBXLoadData);

		//파싱된 모든 노드에 대한 프레임별 전역 트랜스폼 추출
		FbxTime frameTime;
		for (FbxLongLong t = pFBXLoadData->animInfo.iStFrame; t <= pFBXLoadData->animInfo.iEdFrame; t++)
		{
			frameTime.SetFrame(t, m_timemode);
			getAnimTrackInfo(t, frameTime, pFBXLoadData);
		}

		//메시 및 사용 텍스처 정보 추출
		if (m_pMeshList.size())
		{
			for (auto pMesh : m_pMeshList)
			{
				std::string szMeshNodeName = pMesh->GetNode()->GetName();
				CPGCFBXLoadUnit* pTargetUnit = pFBXLoadData->fbxUnitMap.find(szMeshNodeName)->second;

				//애니메이션의 대상이 되는 메시의 각각의 정점에 적용시킬 
				//영향을 받는 Bone의 Local 트랜스폼 행렬과 인덱스 및 가중치 정보를 가져온다.
				getMeshSkinningInfo(pMesh, pTargetUnit, pFBXLoadData);

				//렌더를 하기 위한 정점 렌더 정보를 가져온다.
				getMeshRenderData(pMesh, pTargetUnit, pFBXLoadData);
			}

			m_FBXFileMap.insert(std::make_pair(wszFBXFilename, wszFBXName));
			m_FBXDataMap.insert(std::make_pair(wszFBXName, pFBXLoadData));

			CPRSFBXMesh* newMesh = new CPRSFBXMesh;
			newMesh->create(m_pDevice, m_pDContext, pFBXLoadData);

			m_FBXMeshMap.insert(std::make_pair(wszFBXName, newMesh));
		}
		else
		{
			//메시 리스트가 비어있는 경우 애니메이션 정보만 추출
			CPRSFBXAnimation* newAnim = new CPRSFBXAnimation;
			newAnim->m_animInfo = pFBXLoadData->animInfo;

			for (auto it : pFBXLoadData->fbxUnitMap)
			{
				newAnim->m_fbxUnitMap.insert(std::make_pair(it.first, it.second->animTracks));
			}
			newAnim->m_fbxUnitIdxMap = pFBXLoadData->fbxUnitIdxMap;

			m_FBXAnimationMap.insert(std::make_pair(wszFBXName, newAnim));
		}

		pFBXScene->Destroy();
	}

	m_pMeshList.clear();

	return true;
}

CPM_Matrix CPGCFBXManager::fbxAffineMatToDXMatrix(const FbxAMatrix& srcMat)
{
	//FbxAMatrix : double => DXSimpleMath : float 변환 과정
	CPM_Matrix ret, temp;

	double* convFrom = (double*)&srcMat;
	float* convTo = (float*)&temp;

	for (int i = 0; i < 16; i++) { convTo[i] = convFrom[i]; }

	//기본 설정을 MayaZUp으로 해 놨으므로 Y/Z축 내용을 변경한다.(2행과 3행 전치, 2열과 3열 전치)
	//열 우선 행렬 구조이므로 참고 할 것
	//트랜스폼 행렬정보만 변환 할 것이므로 아래와 같이 처리한다.
	ret._11 = temp._11, ret._12 = temp._13, ret._13 = temp._12;
	ret._21 = temp._31, ret._22 = temp._33, ret._23 = temp._32;
	ret._31 = temp._21, ret._32 = temp._23, ret._33 = temp._22;
	ret._41 = temp._41, ret._42 = temp._43, ret._43 = temp._42;

	ret._14 = ret._24 = ret._34 = 0.0f;
	ret._44 = 1.0f;

	return ret;
}

CPGCFBXLoadData* CPGCFBXManager::getLoadData(std::wstring wszFBXMeshName)
{
	auto it = m_FBXDataMap.find(wszFBXMeshName);
	if (it != m_FBXDataMap.end()) { return it->second; }

	return nullptr;
}

CPRSFBXMesh* CPGCFBXManager::getMesh(std::wstring wszFBXMeshName)
{
	auto it = m_FBXMeshMap.find(wszFBXMeshName);

	if (it != m_FBXMeshMap.end())
	{
		return it->second;
	}

	return nullptr;
}

CPRSFBXAnimation* CPGCFBXManager::getAnimation(std::wstring wszFBXAnimationName)
{
	auto it = m_FBXAnimationMap.find(wszFBXAnimationName);

	if (it != m_FBXAnimationMap.end())
	{
		return it->second;
	}

	return nullptr;
}

void CPGCFBXManager::getAnimInfo(FbxScene* pScene, CPGCFBXLoadData* pLoadData)
{
	FbxLongLong st, ed;	//시작 프레임, 끝 프레임
	st = ed = 0;

	//1. 애니메이션 프레임 정보를 추출할 FbxAnimStack 가져오기
	FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(0);	//첫번째 꺼

	//2. 전역 시간 단위 설정 - 이 시간 설정을 기준으로 애니메이션의 시간 정보를 재구성한다.
	FbxTime::SetGlobalTimeMode(m_timemode);	//scene의 기준을 바꾸는 부분

	//3. 불러온 AnimStack이 유효하다면 정보 추출 하기
	//애니메이션 정보가 없어도 제대로 동작하게 하기 위해서 조건에 부합하면 적용한다.
	if (pAnimStack)
	{
		//4.1 AnimStack의 이름을 통해 takeInfo를 받아와 시간 정보 구성하기
		FbxString szAnimStackName = pAnimStack->GetName();
		pLoadData->animInfo.wszAnimStackName = atl_M2W(pAnimStack->GetName());

		FbxTakeInfo* pTakeInfo = pScene->GetTakeInfo(szAnimStackName);

		//4.2 애니메이션의 시작과 끝 정보 가져와 재구성 하기
		FbxTimeSpan localTimeSpan = pTakeInfo->mLocalTimeSpan;
		FbxTime stT = localTimeSpan.GetStart();
		FbxTime edT = localTimeSpan.GetStop();

		st = stT.GetFrameCount(m_timemode);	//GetFrameCount(FbxTime::EMode) : 시간 모드가 적용된 프레임 카운트를 반환한다.
		ed = edT.GetFrameCount(m_timemode);
	}

	//4. 오브젝트 애니메이션에 필요한 정보를 AnimScene에 저장해 놓기
	pLoadData->animInfo.iStFrame = st;
	pLoadData->animInfo.iEdFrame = ed;
	pLoadData->animInfo.iTotalFrame = max(1, ed - st);				//같이 불러올 수 있는 정보지만 한 파일에 여러 애니메이션이 구성된 경우 의미가 없기에 직접 넣는다.
	pLoadData->animInfo.fTickPerFrame = CPMAC_FBX_DEFAULT_TICKS;	//기본값으로 사용한다 위와 마찬가지로 필요시 바뀔 수 있다.
	pLoadData->animInfo.fFPS = CPMAC_FBX_DEFAULT_FPS;				//위 글로벌 설정을 따른다. 당연히 필요시 바뀔 수 있다.
}

void CPGCFBXManager::preProcess(FbxNode* pNode, CPGCFBXLoadUnit* pParentUnit, CPGCFBXLoadData* pLoadData)
{
	//1. 카메라 / 조명으로 캐스팅 되는 노드는 메시를 가지고 있어도 파싱 대상에서 제외한다.
	if (!pNode) { return; }
	if (pNode->GetCamera() || pNode->GetLight()) { return; }

	//2. 유닛 기초정보 세팅
	CPGCFBXLoadUnit* pUnit = new CPGCFBXLoadUnit;
	pUnit->szNodename = pNode->GetName();

	//루트 노드인 경우 노드의 속성이 부여되지 않는다.
	FbxNodeAttribute* pNodeAtt = pNode->GetNodeAttribute();
	if (pNodeAtt) { pUnit->nodeType = pNodeAtt->GetAttributeType(); } //노드의 속성이 정해지지 않은 경우도 있다. 보통 루트 노드가 이에 해당한다.
	else { pUnit->nodeType = FbxNodeAttribute::EType::eUnknown; }

	pUnit->pFbxNode = pNode;
	pUnit->pParentFbxNode = pNode->GetParent();
	pUnit->pParentUnit = pParentUnit;
	if (pParentUnit) { pParentUnit->pChilds.push_back(pUnit); }
	pUnit->iIdx = pLoadData->fbxUnitMap.size();			//노드 유닛에 인덱스 부여

	//3. 노드 이름 검색을 위한 전처리
	//노드이름으로 해당 유닛과 인덱스를 찾을 수 있도록 한다.
	pLoadData->fbxUnitMap.insert(std::make_pair(pUnit->szNodename, pUnit));
	pLoadData->fbxUnitIdxMap.insert(std::make_pair(pUnit->szNodename, pUnit->iIdx));

	//4. 노드에 메시가 있는 경우 파싱리스트에 추가 : 루트 부터 전위 순회하게 된다.
	FbxMesh* pTemp = pNode->GetMesh();
	if (pTemp) { m_pMeshList.push_back(pTemp); }

	//5. 재귀 작업 수행
	//루트를 제외한 eNull, eMesh, eSkeleton Node에 해당하는 노드만 전처리하여 오브젝트를 생성한다.
	//Default : GetChildCount(false) : 자식 노드만 카운팅한다.
	//GetChildCount(true) : 모든 자식 및 후손 노드까지 카운팅한다.

	UINT nChild = pNode->GetChildCount();

	for (UINT i = 0; i < nChild; i++)
	{
		//eLOD등의 노드들 까지도 별도의 메시 정보를 포함할 수 있기에 모든 노드를 일단 전처리 한다.
		FbxNode* pChild = pNode->GetChild(i);
		preProcess(pChild, pUnit, pLoadData);
	}
}

void CPGCFBXManager::getAnimTrackInfo(FbxLongLong iFrame, FbxTime& frameTime, CPGCFBXLoadData* pLoadData)
{
	for (auto& itUnit : pLoadData->fbxUnitMap)
	{
		CPRS_AnimTrack newTrack;
		newTrack.iFrame = iFrame;

		//1. 프레임에 해당하는 전역 트랜스폼 행렬을 구하고 사용하는 행렬 형태로 변환한다.
		//FbxAMatrix는 열 우선 행렬이며 트랜스폼을 적용하는 아핀변환 행렬을 담당한다.
		//EvaluateGlobalTransform : 모델에 적용되는 전역 좌표계의 트랜스폼 행렬을 반환한다. 
		//FinalTransform = Local * AllParentLocalMat * globalTransform
		FbxAMatrix globalMat = itUnit.second->pFbxNode->EvaluateGlobalTransform(frameTime);
		newTrack.globalTransform = fbxAffineMatToDXMatrix(globalMat);

		//2. 프레임 보간을 위해서 행렬을 분해해 요소로 보관한다.
		//회전은 쿼터니언으로 저장한다.
		newTrack.globalTransform.Decompose(newTrack.vS, newTrack.qR, newTrack.vT);
		itUnit.second->animTracks.push_back(newTrack);
	}
}

void CPGCFBXManager::getMeshSkinningInfo(FbxMesh* pMesh, CPGCFBXLoadUnit* pUnit, CPGCFBXLoadData* pLoadData)
{
	//1. skinning info 파싱을 위해 필요한 요소 준비
	int nDeformer = pMesh->GetDeformerCount(FbxDeformer::eSkin);	//스킨 정보만 들어있게 해야한다.
	if (!nDeformer) { pUnit->bSkinned = false; return; }

	int nVertex = pMesh->GetControlPointsCount();	
	pUnit->VertexIWs.resize(nVertex);					//메시 제어점 개수 만큼 인덱스 / 가중치 구조체 배열 확장하기

	//3. Deformer 개수만큼 정보 추출하기
	for (int iDeform = 0; iDeform < nDeformer; iDeform++)
	{
		//3-1 Deformer를 FbxSkin으로 캐스팅
		FbxDeformer* pDeformer = pMesh->GetDeformer(iDeform, FbxDeformer::eSkin);
		std::string deformerName = pDeformer->GetName();
		FbxSkin* pSkin = (FbxSkin*)pDeformer;

		//3-2 FbxSkin의 cluster정보를 이용해 정점마다 적용할 boneIndex / Weight 추출하기
		int nCluster = pSkin->GetClusterCount();
		for (int iCluster = 0; iCluster < nCluster; iCluster++)
		{
			//하나의 Cluster내 정점군은 하나의 bone의 영향을 받는다.
			//한 제어점이 여러 cluster에 존재 할 수 있다.
			FbxCluster* pCluster = pSkin->GetCluster(iCluster);

			//3-3-1 cluster에 영향을 주는 bone을 가진 노드 유닛의 인덱스 가져오기
			std::string pBoneName = pCluster->GetLink()->GetName();
			int iBoneIdx = pLoadData->fbxUnitIdxMap.find(pBoneName.c_str())->second;

			//3-3-2 Bone의 Local좌표계로 변환하는 행렬 생성하기 : 열 우선 행렬 연산 순서이므로 유의하자
			FbxAMatrix linkBoneBindPoseMat;						//LinkNode의 BindPose로의 트랜스폼
			FbxAMatrix linkBoneParentMat;						//LinkNode가 biped에 바인딩 되어 위치하기 위한 부모 트랜스폼
			pCluster->GetTransformLinkMatrix(linkBoneBindPoseMat);
			pCluster->GetTransformMatrix(linkBoneParentMat);

			//이 구조는 FBX의 SDK를 이용할 때의 구조에 따른 것으로 다른 3D모델 파일은 이와 다를 수 있음을 유의하자
			FbxAMatrix temp = linkBoneBindPoseMat.Inverse() * linkBoneParentMat;
			CPRS_BONEMATRIX localBoneMat;
			localBoneMat.szBoneName = pBoneName;
			localBoneMat.wszBoneName = atl_M2W(pBoneName);
			localBoneMat.boneMat = fbxAffineMatToDXMatrix(temp);

			pUnit->LocalBoneMatMap.insert(std::make_pair(iBoneIdx, localBoneMat));

			//3-3-3 cluster로 부터 제어점이 영향을 받는 bone frame IW정보 재구성하기
			int			nIW			= pCluster->GetControlPointIndicesCount();
			int*		iIndexs		= pCluster->GetControlPointIndices();
			double*		dWeights	= pCluster->GetControlPointWeights();

			for (int idx = 0; idx < nIW; idx++)
			{
				int		index = iIndexs[idx];
				float	weight = dWeights[idx];

				pUnit->VertexIWs[index].add(iBoneIdx, weight);
			}
		}
	}

	pUnit->bSkinned = true;
}

void CPGCFBXManager::getMeshRenderData(FbxMesh* pMesh, CPGCFBXLoadUnit* pUnit, CPGCFBXLoadData* pLoadData)
{
	//0. 메시 추출정보를 저장할 자료구조 세팅
	pLoadData->renderMeshUnitList.push_back(pUnit);

	//1. 노드 정보 받아오고 로컬 / 월드 행렬 구성하기
	FbxNode* pMeshNode = pUnit->pFbxNode;

	//지오메트리 행렬 구성부 : pivot context를 이동시키는 행렬
	//부모 자식간에 상속되지 않으며 모든 정점에 공통으로 적용되어야 하는 원점 이동 행렬에 해당한다.
	//당연히 단위 행렬일 수 있다.
	FbxAMatrix geometryMat;

	FbxVector4 LocT = pMeshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 LocR = pMeshNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 LocS = pMeshNode->GetGeometricScaling(FbxNode::eSourcePivot);

	//T R S순 : 열 우선 행렬
	geometryMat.SetT(LocT);
	geometryMat.SetR(LocR);
	geometryMat.SetS(LocS);

	//노말은 역행렬의 전치행렬을 곱해야 제대로 계산된다고 한다.
	FbxAMatrix normalLocalMatrix = geometryMat;
	normalLocalMatrix = normalLocalMatrix.Inverse();
	normalLocalMatrix = normalLocalMatrix.Transpose();

	//2. 레이어에서 리소스 정보 읽기

	//개별 오브젝트마다 작업할 리소스 셋 리스트
	std::vector<FbxLayer*>						m_Layers;
	std::vector<FbxLayerElementVertexColor*>	m_LayerVTColorSets;
	std::vector<FbxLayerElementUV*>				m_LayerUVSets;
	std::vector<FbxLayerElementMaterial*>		m_LayerMTSets;
	std::vector<FbxLayerElementNormal*>			m_LayerVTNormalSets;

	UINT nLayer = pMesh->GetLayerCount();

	m_Layers.resize(nLayer);
	m_LayerVTColorSets.resize(nLayer);
	m_LayerUVSets.resize(nLayer);
	m_LayerMTSets.resize(nLayer);
	m_LayerVTNormalSets.resize(nLayer);

	for (int layerIdx = 0; layerIdx < nLayer; layerIdx++)
	{
		m_Layers[layerIdx] = pMesh->GetLayer(layerIdx);

		//Layer에서 리소스 Set가져오기
		m_LayerVTNormalSets[layerIdx] = m_Layers[layerIdx]->GetNormals();		//normals
		m_LayerVTColorSets[layerIdx] = m_Layers[layerIdx]->GetVertexColors();	//vertex colors
		m_LayerUVSets[layerIdx] = m_Layers[layerIdx]->GetUVs();					//UV colors
		m_LayerMTSets[layerIdx] = m_Layers[layerIdx]->GetMaterials();			//materials
	}

	//3. node의 Material을 통해 사용된 텍스처 파일 정보 읽어오기
	UINT nMaterial = pMeshNode->GetMaterialCount();

	//텍스처 파일리스트와 정점 정보와 정점 버퍼 리스트를 이 때 만들어 둔다.
	std::string szFileName;

	if (!nMaterial) 
	{ 
		pUnit->VBDataList.resize(1); 
		pUnit->IBDataList.resize(1);
		pUnit->wszTexNameList.resize(1);
	}
	else 
	{ 
		pUnit->VBDataList.resize(nMaterial); 
		pUnit->IBDataList.resize(nMaterial);
		pUnit->wszTexNameList.resize(nMaterial);
	}

	for (int mtIdx = 0; mtIdx < nMaterial; mtIdx++)
	{
		//Material마다 특정 텍스처 정보를 가져온다.
		//여기에서는 diffuse텍스처 정보를 사용한다.
		FbxSurfaceMaterial* pSurfaceMt = pMeshNode->GetMaterial(mtIdx);

		if (pSurfaceMt)
		{
			FbxProperty sufProp = pSurfaceMt->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (sufProp.IsValid())
			{
				//FbxFileTexture클래스에 해당하는 첫번째 파일의 정보를 불러온다.
				FbxFileTexture* pTex = sufProp.GetSrcObject<FbxFileTexture>(0);

				if (pTex)
				{
					//파일은 절대 경로로 불러와 진다.
					std::wstring wszTemp;
					szFileName = pTex->GetFileName();
					if (szFileName.size())
					{
						getSplitName(szFileName, wszTemp);
						pUnit->wszTexNameList[mtIdx] = wszTemp;
					}
				}
			}
		}
	}

	//4.폴리곤 정보 읽어 정점 구성하기 - 딱 1개의 레이어만 지금은 사용한다.
	UINT nPolygon = pMesh->GetPolygonCount();
	UINT iBaseVTIdx = 0;
	UINT iSubMTIdx = 0;

	//제어점 리스트를 읽어 온다.
	FbxVector4* pVertexPositions = pMesh->GetControlPoints();

	for (int PGIdx = 0; PGIdx < nPolygon; PGIdx++)
	{
		//해당 폴리곤이 정점 몇개로 이루어져 있는지를 반환한다.
		UINT iPGSize = pMesh->GetPolygonSize(PGIdx);

		//4정점 1폴리곤이면 사각형이므로 2면으로 구성
		//3정점 1폴리곤이면 삼각형이므로 그대로 1면
		UINT faceVNum = iPGSize - 2;

		if (m_LayerMTSets.size())
		{
			iSubMTIdx = getSubMaterialIndex(PGIdx, m_LayerMTSets[0]);
		}

		for (int FaceIdx = 0; FaceIdx < faceVNum; FaceIdx++)
		{
			//위치 정점 인덱스 : Max는 반시계 방향으로 구성되어야 앞면이다.
			// a - d
			// | \ |
			// b - c
			//DX는 시계방향으로 구성되어야 하므로 인덱스 순서가 다음과 같이 바뀐다.
			// MAX : a - b - c / a - c - d
			// DX : a - c - b / a - d - c

			//좌표계가 다르므로(y, z switch) 정점의 순서는 이렇게 구성된다.
			UINT iCornerIdx[3];
			iCornerIdx[0] = pMesh->GetPolygonVertex(PGIdx, 0);
			iCornerIdx[1] = pMesh->GetPolygonVertex(PGIdx, FaceIdx + 2);
			iCornerIdx[2] = pMesh->GetPolygonVertex(PGIdx, FaceIdx + 1);

			pUnit->IBDataList[iSubMTIdx].push_back(iCornerIdx[0]);
			pUnit->IBDataList[iSubMTIdx].push_back(iCornerIdx[1]);
			pUnit->IBDataList[iSubMTIdx].push_back(iCornerIdx[2]);

			//정점 텍스처 인덱스
			UINT iUVIdx[3];
			iUVIdx[0] = pMesh->GetTextureUVIndex(PGIdx, 0);
			iUVIdx[1] = pMesh->GetTextureUVIndex(PGIdx, FaceIdx + 2);
			iUVIdx[2] = pMesh->GetTextureUVIndex(PGIdx, FaceIdx + 1);

			//정점 컬러 인덱스
			UINT iVTColorIdx[3] = { 0, FaceIdx + 2 , FaceIdx + 1 };

			//정점 노말 인덱스
			UINT iVTNormalIdx[3] = { 0, FaceIdx + 2 , FaceIdx + 1 };

			//정점 구성부 : 면마다 3개
			for (int VtIdx = 0; VtIdx < 3; VtIdx++)
			{
				CPRS_SkinningVertex	newV;

				//위치
				FbxVector4 v = pVertexPositions[iCornerIdx[VtIdx]];
				v = geometryMat.MultT(v);

				newV.v.p.x = v.mData[0];
				newV.v.p.y = v.mData[2];
				newV.v.p.z = v.mData[1];

				//노말
				newV.v.n = { 0.0f, 0.0f, 0.0f };

				if (m_LayerVTNormalSets[0])
				{
					FbxVector4 n = readNormal(pMesh,
						m_LayerVTNormalSets[0],
						iCornerIdx[VtIdx],
						iBaseVTIdx + iVTNormalIdx[VtIdx]);	//여기부분이 다름 : 폴리곤마다 번호가 누적된다.

					n = normalLocalMatrix.MultT(n);

					newV.v.n.x = n.mData[0];
					newV.v.n.y = n.mData[2];
					newV.v.n.z = n.mData[1];
				}

				//컬러
				newV.v.c = { 1.0f, 1.0f, 1.0f, 1.0f };	//기본 설정값

				if (m_LayerVTColorSets[0])
				{
					FbxColor c = readColor(pMesh,
						m_LayerVTColorSets[0],
						iCornerIdx[VtIdx],
						iBaseVTIdx + iVTColorIdx[VtIdx]);	//여기부분이 다름 : 폴리곤마다 번호가 누적된다.

					newV.v.c.x = c.mRed;
					newV.v.c.y = c.mGreen;
					newV.v.c.z = c.mBlue;
					newV.v.c.w = 1.0f;		//c.mAlpha;
				}

				//텍스처
				if (m_LayerUVSets[0])
				{
					FbxVector2 t = readTextureCoord(pMesh,
						m_LayerUVSets[0],
						iCornerIdx[VtIdx],
						iUVIdx[VtIdx]);

					newV.v.t.x = t.mData[0];
					newV.v.t.y = 1.0f - t.mData[1];
				}

				//행렬 인덱스와 가중치
				if (pUnit->bSkinned)
				{
					CPRS_VertexIWs* pIW = &pUnit->VertexIWs[iCornerIdx[VtIdx]];

					newV.i.x = pIW->boneIndexs[0];
					newV.i.y = pIW->boneIndexs[1];
					newV.i.z = pIW->boneIndexs[2];
					newV.i.w = pIW->boneIndexs[3];

					newV.w.x = pIW->combinedWeights[0];
					newV.w.y = pIW->combinedWeights[1];
					newV.w.z = pIW->combinedWeights[2];
					newV.w.w = pIW->combinedWeights[3];
				}
				else
				{
					//나머지는 생성시 자동 초기화
					newV.i.x = 0.0f;

					//어짜피 가중치가 0.0f인 부분은 인덱스가 어떤 값이어도 상관 없음
					newV.w.x = 1.0f;
				}

				if (pUnit->minXYZ.x > newV.v.p.x) { pUnit->minXYZ.x = newV.v.p.x; }
				if (pUnit->minXYZ.y > newV.v.p.y) { pUnit->minXYZ.y = newV.v.p.y; }
				if (pUnit->minXYZ.z > newV.v.p.z) { pUnit->minXYZ.z = newV.v.p.z; }

				if (pUnit->maxXYZ.x < newV.v.p.x) { pUnit->maxXYZ.x = newV.v.p.x; }
				if (pUnit->maxXYZ.y < newV.v.p.y) { pUnit->maxXYZ.y = newV.v.p.y; }
				if (pUnit->maxXYZ.z < newV.v.p.z) { pUnit->maxXYZ.z = newV.v.p.z; }
				pUnit->VBDataList[iSubMTIdx].push_back(newV);
			}
		}

		iBaseVTIdx += iPGSize;
	}

	pUnit->BVolume.adjustVolume((pUnit->maxXYZ - pUnit->minXYZ) / 2.0f);
}

FbxVector2 CPGCFBXManager::readTextureCoord(FbxMesh* pFbxMesh,
											FbxLayerElementUV* pVertexUVSet,
											UINT VTIndex,
											UINT UVIndex)
{
	//1. 텍스처 매핑 형식에 따라 분기를 나눈다. (EMappingMode)
	//	eByControlPoint와 eByPolygonVertex는 Material을 제외한 요소에 사용된다.
	//	eByPolygon과 eByAllSame은 Material을 위해 사용된다.

	//2. 데이터 주소 접근 방식에 따라 분기를 나눈다. (EReferenceMode)
	//eIndex는 FBX 5.0이후 버전부터 deplecated된 부분에 해당한다.
	//

	FbxVector2 ret;

	FbxLayerElementUV::EMappingMode eMapMode = pVertexUVSet->GetMappingMode();
	FbxLayerElementUV::EReferenceMode eRefMode = pVertexUVSet->GetReferenceMode();

	switch (eMapMode)
	{
	case FbxLayerElement::EMappingMode::eByControlPoint:
	{
		switch (eRefMode)
		{
		case FbxLayerElement::EReferenceMode::eDirect:
		{
			ret = pVertexUVSet->GetDirectArray().GetAt(VTIndex);
		} break;

		case FbxLayerElement::EReferenceMode::eIndexToDirect:
		{
			int idx = pVertexUVSet->GetIndexArray().GetAt(VTIndex);
			ret = pVertexUVSet->GetDirectArray().GetAt(idx);
		} break;
		}

	} break;

	case FbxLayerElement::EMappingMode::eByPolygonVertex:
	{
		switch (eRefMode)
		{
		case FbxLayerElement::EReferenceMode::eDirect:
		case FbxLayerElement::EReferenceMode::eIndexToDirect:
		{
			ret = pVertexUVSet->GetDirectArray().GetAt(UVIndex);
		} break;
		}
	} break;
	}

	return ret;
}

FbxColor CPGCFBXManager::readColor(	FbxMesh* pFbxMesh,
									FbxLayerElementVertexColor* pVTColorSet,
									UINT VTIndex,
									UINT colorIndex)
{
	FbxColor ret(1, 1, 1, 1);

	FbxLayerElement::EMappingMode eMapMode = pVTColorSet->GetMappingMode();
	FbxLayerElement::EReferenceMode eRefMode = pVTColorSet->GetReferenceMode();

	switch (eMapMode)
	{
	case FbxLayerElement::EMappingMode::eByControlPoint:
	{
		switch (eRefMode)
		{
		case FbxLayerElement::EReferenceMode::eDirect:
		{
			ret = pVTColorSet->GetDirectArray().GetAt(VTIndex);
		} break;

		case FbxLayerElement::EReferenceMode::eIndexToDirect:
		{
			int idx = pVTColorSet->GetIndexArray().GetAt(VTIndex);
			ret = pVTColorSet->GetDirectArray().GetAt(idx);
		} break;
		}

	} break;

	case FbxLayerElement::EMappingMode::eByPolygonVertex:
	{
		switch (eRefMode)
		{
		case FbxLayerElement::EReferenceMode::eDirect:
		{
			ret = pVTColorSet->GetDirectArray().GetAt(colorIndex);
		} break;

		case FbxLayerElement::EReferenceMode::eIndexToDirect:
		{
			int idx = pVTColorSet->GetIndexArray().GetAt(colorIndex);
			ret = pVTColorSet->GetDirectArray().GetAt(idx);
		} break;
		}
	} break;
	}

	return ret;
}

FbxVector4 CPGCFBXManager::readNormal(	FbxMesh* pFbxMesh,
										FbxLayerElementNormal* pVTNormalSet,
										UINT VTIndex,
										UINT normalIndex)
{
	FbxVector4 ret(1, 1, 1, 1);

	FbxLayerElement::EMappingMode eMapMode = pVTNormalSet->GetMappingMode();
	FbxLayerElement::EReferenceMode eRefMode;

	switch (eMapMode)
	{
	case FbxLayerElement::EMappingMode::eByControlPoint:
	{
		eRefMode = pVTNormalSet->GetReferenceMode();
		switch (eRefMode)
		{
		case FbxLayerElement::EReferenceMode::eDirect:
		{
			ret = pVTNormalSet->GetDirectArray().GetAt(VTIndex);
		} break;

		case FbxLayerElement::EReferenceMode::eIndexToDirect:
		{
			int idx = pVTNormalSet->GetIndexArray().GetAt(VTIndex);
			ret = pVTNormalSet->GetDirectArray().GetAt(idx);
		} break;
		}

	} break;

	case FbxLayerElement::EMappingMode::eByPolygonVertex:
	{
		eRefMode = pVTNormalSet->GetReferenceMode();
		switch (eRefMode)
		{
		case FbxLayerElement::EReferenceMode::eDirect:
		{
			ret = pVTNormalSet->GetDirectArray().GetAt(normalIndex);
		} break;

		case FbxLayerElement::EReferenceMode::eIndexToDirect:
		{
			int idx = pVTNormalSet->GetIndexArray().GetAt(normalIndex);
			ret = pVTNormalSet->GetDirectArray().GetAt(idx);
		} break;
		}
	} break;
	}

	return ret;
}

UINT CPGCFBXManager::getSubMaterialIndex(UINT iPolygonIdx, FbxLayerElementMaterial* pMTSet)
{
	UINT ret = 0;

	if (pMTSet)
	{
		FbxLayerElement::EMappingMode eMapMode = pMTSet->GetMappingMode();
		FbxLayerElement::EReferenceMode eRefMode;

		switch (eMapMode)
		{
		case FbxLayerElement::eByPolygon:
		{
			eRefMode = pMTSet->GetReferenceMode();

			switch (eRefMode)
			{
			case FbxLayerElement::eIndex:
			{
				ret = iPolygonIdx;
			} break;

			case FbxLayerElement::eIndexToDirect:
			{
				ret = pMTSet->GetIndexArray().GetAt(iPolygonIdx);
			} break;
			}

		} break;
		}
	}

	return ret;
}

CPRSFBXRenderUnit::CPRSFBXRenderUnit()
{
	m_idx = 0;

	m_iCBufCount = 0;

	m_iStride = 0;
	m_iOffset = 0;
	m_primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pDevice = nullptr;
	m_pDContext = nullptr;

	m_pVLayout = nullptr;

	m_bSkinned = false;

	m_isExistTexture = false;
}

CPRSFBXRenderUnit::~CPRSFBXRenderUnit()
{

}

HRESULT CPRSFBXRenderUnit::create(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDContext, 
	CPGCFBXLoadUnit* pLoadUnit,
	CPGCFBXLoadData* pLoadData)
{
	HRESULT hr = S_OK;

	if (!pLoadUnit) { hr = E_POINTER; return hr; }

	if (!pDevice || !pDContext) { hr = E_POINTER; return hr; }
	else { setDevice(pDevice, pDContext); }

	int iMTNum = pLoadUnit->VBDataList.size();

	m_wszName = atl_M2W(pLoadUnit->szNodename);
	m_idx = pLoadUnit->iIdx;
	m_bSkinned = pLoadUnit->bSkinned;

	m_iStride = sizeof(CPRS_SkinningVertex);
	m_iOffset = 0;
	m_primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_vertexGroups.resize(iMTNum);
	m_IndexGroups.resize(iMTNum);
	m_iVertexCountArray.resize(iMTNum);
	m_iIndexCountArray.resize(iMTNum);
	m_wszTexNameList.resize(iMTNum);
	m_pTextureList.resize(iMTNum);

	m_pVBufs.resize(iMTNum);
	m_pIBufs.resize(iMTNum);

	for (int mtIDX = 0; mtIDX < iMTNum; mtIDX++)
	{
		m_vertexGroups[mtIDX].assign(pLoadUnit->VBDataList[mtIDX].begin(),
			pLoadUnit->VBDataList[mtIDX].end());

		m_IndexGroups[mtIDX].assign(pLoadUnit->IBDataList[mtIDX].begin(),
			pLoadUnit->IBDataList[mtIDX].end());

		m_iVertexCountArray[mtIDX] = m_vertexGroups[mtIDX].size();
		m_iIndexCountArray[mtIDX] = m_IndexGroups[mtIDX].size();
		m_wszTexNameList[mtIDX] = pLoadUnit->wszTexNameList[mtIDX];

		CPGC_TEXTURE_MGR.createTexture(pLoadUnit->wszTexNameList[mtIDX], CPMAC_FBX_TEXTURE_FILE_DIRECTORY_PATH + m_wszTexNameList[mtIDX]);
		m_pTextureList[mtIDX] = CPGC_TEXTURE_MGR.getPtr(pLoadUnit->wszTexNameList[mtIDX]);
		if (m_pTextureList[mtIDX])
		{
			m_isExistTexture = true;
		}

		if (m_vertexGroups[mtIDX].size())
		{
			hr = createDXSimpleBuf(m_pDevice,
				m_iStride * m_iVertexCountArray[mtIDX],
				&m_vertexGroups[mtIDX].at(0),
				D3D11_BIND_VERTEX_BUFFER,
				m_pVBufs[mtIDX].GetAddressOf());

			if (FAILED(hr))
			{
				release();
				return hr;
			}

			hr = createDXSimpleBuf(m_pDevice,
				sizeof(UINT) * m_iIndexCountArray[mtIDX],
				&m_IndexGroups[mtIDX].at(0),
				D3D11_BIND_INDEX_BUFFER,
				m_pIBufs[mtIDX].GetAddressOf());

			if (FAILED(hr))
			{
				release();
				return hr;
			}
		}
	}

	if (FAILED(hr))
	{
		release();
		return hr;
	}

	m_pVLayout			= CPGC_VLAYOUT_MGR.getPtr(L"VL_PNCTIW");
	m_shaderGroup.pVS	= CPGC_SHADER_MGR.getVS(L"VS_PNCTIW_Skinning");
	m_shaderGroup.pPS	= CPGC_SHADER_MGR.getPS(L"PS_PNCT_OutlineTexVColor");

	m_pNoTexPS			= CPGC_SHADER_MGR.getPS(L"PS_PNCT_OutlineNoTex");

	m_pSampler			= CPGC_DXSTATE_MGR.getSState(CPGC_VNAME2WSTR(SS_LINEAR_WRAP));

	m_bSkinned = pLoadUnit->bSkinned;
	m_LocalBoneMatMap = pLoadUnit->LocalBoneMatMap;

	m_animTracks.assign(pLoadUnit->animTracks.begin(), pLoadUnit->animTracks.end());

	return hr;
}

bool CPRSFBXRenderUnit::init()
{
	return true;
}

bool CPRSFBXRenderUnit::release()
{
	m_pVBufs.clear();
	m_pIBufs.clear();

	return true;
}

void CPRSFBXRenderUnit::setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	m_pDevice = pDevice;
	m_pDContext = pDContext;
}

void CPRSFBXRenderUnit::draw()
{
	for (int i = 0; i < m_vertexGroups.size(); i++)
	{
		if (m_pVBufs[i])
		{
			m_pDContext->IASetVertexBuffers(0, 1, m_pVBufs[i].GetAddressOf(), &m_iStride, &m_iOffset);
			m_pDContext->IASetInputLayout(m_pVLayout);
			m_pDContext->IASetPrimitiveTopology(m_primitive);

			m_shaderGroup.bindToPipeline(m_pDContext);

			if (m_isExistTexture)
			{
				if (m_pTextureList[i])
				{
					ID3D11ShaderResourceView* temp = m_pTextureList[i]->getSRV();
					m_pDContext->PSSetShaderResources(0, 1, &temp);
				}
			}
			else
			{
				ID3D11ShaderResourceView* temp = nullptr;
				m_pDContext->PSSetShaderResources(0, 1, &temp);
				m_pDContext->PSSetShader(m_pNoTexPS, nullptr, 0);
			}

			m_pDContext->PSSetSamplers(0, 1, &m_pSampler);

			m_pDContext->Draw(m_iVertexCountArray[i], 0);
		}
	}
}

void CPRSFBXRenderUnit::draw(CPRS_ShaderGroup& sGruop, ID3D11PixelShader* pNoTexPS)
{
	for (int i = 0; i < m_vertexGroups.size(); i++)
	{
		if (m_pVBufs[i])
		{
			m_pDContext->IASetVertexBuffers(0, 1, m_pVBufs[i].GetAddressOf(), &m_iStride, &m_iOffset);
			m_pDContext->IASetInputLayout(m_pVLayout);
			m_pDContext->IASetPrimitiveTopology(m_primitive);

			sGruop.bindToPipeline(m_pDContext);

			if (m_isExistTexture)
			{
				if (m_pTextureList[i])
				{
					ID3D11ShaderResourceView* temp = m_pTextureList[i]->getSRV();
					m_pDContext->PSSetShaderResources(0, 1, &temp);
				}
			}
			else
			{
				ID3D11ShaderResourceView* temp = nullptr;
				m_pDContext->PSSetShaderResources(0, 1, &temp);
				m_pDContext->PSSetShader(pNoTexPS, nullptr, 0);
			}

			m_pDContext->PSSetSamplers(0, 1, &m_pSampler);

			m_pDContext->Draw(m_iVertexCountArray[i], 0);
		}
	}
}

void CPRSFBXRenderUnit::instanceDraw()
{
	for (int i = 0; i < m_vertexGroups.size(); i++)
	{
		if (m_pVBufs[i])
		{
			m_pDContext->IASetVertexBuffers(0, 1, m_pVBufs[i].GetAddressOf(), &m_iStride, &m_iOffset);
			m_pDContext->IASetInputLayout(m_pVLayout);
			m_pDContext->IASetPrimitiveTopology(m_primitive);

			m_shaderGroup.bindToPipeline(m_pDContext);

			if (m_isExistTexture)
			{
				if (m_pTextureList[i])
				{
					ID3D11ShaderResourceView* temp = m_pTextureList[i]->getSRV();
					m_pDContext->PSSetShaderResources(0, 1, &temp);
				}
			}
			else
			{
				ID3D11ShaderResourceView* temp = nullptr;
				m_pDContext->PSSetShaderResources(0, 1, &temp);
				m_pDContext->PSSetShader(m_pNoTexPS, nullptr, 0);
			}

			m_pDContext->PSSetSamplers(0, 1, &m_pSampler);

			m_pDContext->DrawInstanced(m_iVertexCountArray[i], 0, 0, 0);
		}
	}
}

CPM_Matrix CPRSFBXRenderUnit::frameInterpolate(FLOAT fFrame, CPRS_AnimInfo& animInfo, bool bLoop)
{
	if (!animInfo.iTotalFrame) { return CPM_Matrix::Identity; }

	if (bLoop)
	{
		if (fFrame > animInfo.iEdFrame) { fFrame = animInfo.iStFrame; }
	}
	else
	{
		if (fFrame > animInfo.iEdFrame) { fFrame = animInfo.iEdFrame; }
	}

	int prevIdx = fFrame - animInfo.iStFrame;
	if (prevIdx < 0) { prevIdx = 0; }

	int nextIdx = fFrame - animInfo.iStFrame + 1;
	if (animInfo.iTotalFrame <= nextIdx) { nextIdx = animInfo.iTotalFrame - 1; }

	CPRS_AnimTrack prev = m_animTracks[prevIdx];
	CPRS_AnimTrack next = m_animTracks[nextIdx];

	UINT iInterval = next.iFrame - prev.iFrame;
	if (iInterval == 0) { return m_animTracks[fFrame].globalTransform; }

	FLOAT t = (fFrame - prev.iFrame) / iInterval;

	CPM_Matrix ret;
	CPM_Matrix matRot;
	CPM_Matrix matScale;

	CPM_Vector3 newPos = CPM_Vector3::Lerp(prev.vT, next.vT, t);
	CPM_Quaternion newRot = CPM_Quaternion::Slerp(prev.qR, next.qR, t);
	CPM_Vector3 newScale = CPM_Vector3::Lerp(prev.vS, next.vS, t);

	matRot = CPM_Matrix::CreateFromQuaternion(newRot);
	matScale = CPM_Matrix::CreateScale(newScale);
	ret = matScale * matRot;
	ret._41 = newPos.x;
	ret._42 = newPos.y;
	ret._43 = newPos.z;

	return ret;
}

CPGCFBXObject::CPGCFBXObject()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;

	m_animState.fCurFrame = 0.0f;
	m_animState.fAnimSpeed = 1.0f;
	m_animState.bLoop = true;

	m_pFBXMesh = nullptr;
	m_pFBXAnimation = nullptr;
}

CPGCFBXObject::~CPGCFBXObject()
{
}

bool CPGCFBXObject::init()
{
	return true;
}

bool CPGCFBXObject::update()
{
	FLOAT dt = CPGC_MAINTIMER.getOneFrameTimeF();
	m_animState.fCurFrame += m_animState.curAnimInfo.fFPS * m_animState.fAnimSpeed * dt;

	if (m_animState.bLoop)
	{
		if (m_animState.fCurFrame < m_animState.fReDefStFrame) { m_animState.fCurFrame = m_animState.fReDefEdFrame; }
		else if (m_animState.fCurFrame > m_animState.fReDefEdFrame) { m_animState.fCurFrame = m_animState.fReDefStFrame; }
	}
	else
	{
		if (m_animState.fCurFrame > m_animState.fReDefEdFrame) { m_animState.fCurFrame = m_animState.fReDefEdFrame; }
	}

	updateAnimMats();

	return true;
}

bool CPGCFBXObject::preRender()
{
	m_pDContext->VSSetConstantBuffers(0, 1, m_pCBufs[0].GetAddressOf());

	return true;
}

bool CPGCFBXObject::render()
{
	int cnt = 0;
	for (auto it : m_pFBXMesh->m_renderMeshUnitList)
	{
		m_pDContext->VSSetConstantBuffers(0, 1, m_pCBufs[0].GetAddressOf());
		m_pDContext->VSSetConstantBuffers(1, 1, m_pCAnimBufs[cnt++].GetAddressOf());

		it->draw();
	}

	return true;
}

bool CPGCFBXObject::render(CPRS_ShaderGroup& sGruop, ID3D11PixelShader* pNoTexPS)
{
	int cnt = 0;
	for (auto it : m_pFBXMesh->m_renderMeshUnitList)
	{
		m_pDContext->VSSetConstantBuffers(0, 1, m_pCBufs[0].GetAddressOf());
		m_pDContext->VSSetConstantBuffers(1, 1, m_pCAnimBufs[cnt++].GetAddressOf());

		it->draw(sGruop, pNoTexPS);
	}

	return true;
}

bool CPGCFBXObject::instanceRender()
{
	int cnt = 0;
	for (auto it : m_pFBXMesh->m_renderMeshUnitList)
	{
		m_pDContext->VSSetConstantBuffers(0, 1, m_pCBufs[0].GetAddressOf());
		m_pDContext->VSSetConstantBuffers(1, 1, m_pCAnimBufs[cnt++].GetAddressOf());

		it->instanceDraw();
	}

	return true;
}

bool CPGCFBXObject::release()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;

	m_pCBufs.clear();

	return true;
}

HRESULT CPGCFBXObject::setDeviceAndMesh(std::wstring wszFBXMeshName,
	ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDContext)
{
	m_pFBXMesh = CPGC_FBX_MGR.getMesh(wszFBXMeshName);
	m_pDevice = pDevice;
	m_pDContext = pDContext;

	if (!pDevice || !pDContext) 
	{
		return E_POINTER;
	}

	HRESULT hr = S_OK;

	if (m_pFBXMesh)
	{
		m_animState.curAnimInfo = m_pFBXMesh->m_animInfo;
		m_animState.fReDefStFrame = m_pFBXMesh->m_animInfo.iStFrame;
		m_animState.fReDefEdFrame = m_pFBXMesh->m_animInfo.iEdFrame;
		m_animState.bLoop = true;
		m_animState.fAnimSpeed = 1.0f;
		m_animState.fCurFrame = 0.0f;

		int cbufNum = m_pFBXMesh->m_renderMeshUnitList.size();
		m_animMats.resize(cbufNum);
		m_pCAnimBufs.resize(cbufNum);

		for (int i = 0; i < cbufNum; i++)
		{
			hr = createDXSimpleBuf(m_pDevice,
				sizeof(CPM_Matrix) * CPMAC_MAX_BONE_NUMBER,
				&m_animMats[i],
				D3D11_BIND_CONSTANT_BUFFER,
				m_pCAnimBufs[i].GetAddressOf());

			if (FAILED(hr))
			{
				printCreateErr(hr); return hr;
			}
		}

		m_pCBufs.resize(1);
		hr = createDXSimpleBuf(m_pDevice,
			sizeof(CPRS_CBUF_COORDCONV_MATSET),
			&m_wvpMat,
			D3D11_BIND_CONSTANT_BUFFER,
			m_pCBufs[0].GetAddressOf());

		if (FAILED(hr))
		{
			printCreateErr(hr); return hr;
		}
	}

	return hr;
}

bool CPGCFBXObject::setAnimation(std::wstring wszFBXAnimationName)
{
	m_pFBXAnimation = CPGC_FBX_MGR.getAnimation(wszFBXAnimationName);

	if (m_pFBXAnimation) 
	{ 
		m_animState.curAnimInfo = m_pFBXAnimation->m_animInfo;
		m_animState.fReDefStFrame = m_pFBXAnimation->m_animInfo.iStFrame;
		m_animState.fReDefEdFrame = m_pFBXAnimation->m_animInfo.iEdFrame;
		m_animState.fCurFrame = 0.0f;
		return true; 
	}
	return false;
}

void CPGCFBXObject::setMatrix(const CPM_Matrix* pWorldM, const CPM_Matrix* pViewM, const CPM_Matrix* pProjM)
{
	if (pWorldM) { m_matWorld = *pWorldM; }
	if (pViewM) { m_matView = *pViewM; }
	if (pProjM) { m_matProj = *pProjM; }

	updateState();

	updateCoordConvMat(m_matWorld, m_matView, m_matProj);
}

void CPGCFBXObject::updateState()
{
	//회전을 정규 기저 벡터의 회전값의 벡터로 표현하기에는 연산이 불필요하게 많다.
	//임의의 축에 대한 회전을 수행 또는 다른 회전 행렬의 복합적인 곱셈 이후에 
	//적용된 최종행렬에서 추출하는 것이 일반적이다.
	//이 함수는 복잡한 스케일 변화를 반영하지는 않는다.

	// 월드 행렬 R*T
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

	//회전 행렬에서 추출한 각 축벡터는 스케일 행렬에 의해 정규화되어있지 않은 상태라고 가정한다.
	m_sightAxis.vRight.Normalize();
	m_sightAxis.vUp.Normalize();
	m_sightAxis.vLook.Normalize();

	//바운딩 볼륨의 재계산을 수행하는 부분이 추가되어야 한다.
}

void CPGCFBXObject::updateCoordConvMat(const CPM_Matrix& matWorld, const CPM_Matrix& matView, const CPM_Matrix& matProj)
{
	matWorld.Transpose(m_wvpMat.matTWorld);

	matWorld.Invert(m_wvpMat.matTInvWorld);
	m_wvpMat.matTInvWorld = m_wvpMat.matTInvWorld.Transpose();

	matView.Transpose(m_wvpMat.matTView);
	matProj.Transpose(m_wvpMat.matTProj);

	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedCbuf;
	ZeroMemory(&mappedCbuf, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = m_pDContext->Map(m_pCBufs[0].Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		NULL,
		&mappedCbuf);

	if (SUCCEEDED(hr))
	{
		//받아온 GPU시작 주소에 업데이트 시킨 상수 버퍼 정보를 복사
		//부분적 수정도 가능
		memcpy(mappedCbuf.pData, &m_wvpMat, sizeof(CPRS_CBUF_COORDCONV_MATSET));

		//이후 Unmap
		m_pDContext->Unmap(m_pCBufs[0].Get(), 0);
	}
}

void CPGCFBXObject::updateAnimMats()
{
	HRESULT hr = S_OK;

	for (int unit = 0; unit < m_pFBXMesh->m_renderMeshUnitList.size(); unit++)
	{
		m_animMats[unit].matTAnimMats[0] = m_pFBXMesh->m_renderMeshUnitList[unit]->frameInterpolate(m_animState.fCurFrame, m_pFBXMesh->m_animInfo, m_animState.bLoop);
		m_animMats[unit].matTAnimMats[0] = m_animMats[unit].matTAnimMats[0].Transpose();

		if (m_pFBXMesh->m_renderMeshUnitList[unit]->m_bSkinned)
		{
			if (m_pFBXAnimation)
			{
				for (auto it : m_pFBXMesh->m_renderMeshUnitList[unit]->m_LocalBoneMatMap)
				{
					int bone = it.first;
					m_animMats[unit].matTAnimMats[bone] = it.second.boneMat * 
						frameInterpolate(m_animState.fCurFrame,
							m_pFBXAnimation->m_fbxUnitMap.find(it.second.szBoneName)->second, m_animState.curAnimInfo);
					m_animMats[unit].matTAnimMats[bone] = m_animMats[unit].matTAnimMats[bone].Transpose();
				}
			}
			else
			{
				for (auto it : m_pFBXMesh->m_renderMeshUnitList[unit]->m_LocalBoneMatMap)
				{
					int bone = it.first;
					m_animMats[unit].matTAnimMats[bone] = it.second.boneMat * 
						frameInterpolate(m_animState.fCurFrame, 
							m_pFBXMesh->m_fbxUnitMap.find(it.second.szBoneName)->second, m_animState.curAnimInfo);
					m_animMats[unit].matTAnimMats[bone] = m_animMats[unit].matTAnimMats[bone].Transpose();
				}
			}
		}

		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

		hr = m_pDContext->Map(m_pCAnimBufs[unit].Get(),
			0, 
			D3D11_MAP_WRITE_DISCARD, 
			NULL, 
			&ms);

		if (SUCCEEDED(hr))
		{
			memcpy(ms.pData, &m_animMats[unit], sizeof(CPM_Matrix) * CPMAC_MAX_BONE_NUMBER);

			m_pDContext->Unmap(m_pCAnimBufs[unit].Get(), 0);
		}
	}
}

CPM_Matrix CPGCFBXObject::frameInterpolate(FLOAT fFrame, std::vector<CPRS_AnimTrack>& animTrack, CPRS_AnimInfo& animInfo)
{
	if (!animInfo.iTotalFrame) { return CPM_Matrix::Identity; }

	CPRS_AnimTrack prev = animTrack[max(0, fFrame - animInfo.iStFrame)];
	CPRS_AnimTrack next = animTrack[min(animInfo.iTotalFrame - 1, fFrame - animInfo.iStFrame + 1)];

	UINT iInterval = next.iFrame - prev.iFrame;
	if (iInterval == 0) { return animTrack[fFrame].globalTransform; }

	FLOAT t = (fFrame - prev.iFrame) / iInterval;

	CPM_Matrix ret;
	CPM_Matrix matRot;
	CPM_Matrix matScale;

	CPM_Vector3 newPos = CPM_Vector3::Lerp(prev.vT, next.vT, t);
	CPM_Quaternion newRot = CPM_Quaternion::Slerp(prev.qR, next.qR, t);
	CPM_Vector3 newScale = CPM_Vector3::Lerp(prev.vS, next.vS, t);

	matRot = CPM_Matrix::CreateFromQuaternion(newRot);
	matScale = CPM_Matrix::CreateScale(newScale);
	ret = matScale * matRot;
	ret._41 = newPos.x;
	ret._42 = newPos.y;
	ret._43 = newPos.z;

	return ret;
}

CPRSFBXMesh::CPRSFBXMesh()
{
}

CPRSFBXMesh::~CPRSFBXMesh()
{
}

HRESULT CPRSFBXMesh::create(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext, CPGCFBXLoadData* pLoadData)
{
	HRESULT hr = S_OK;

	if (!pLoadData) { hr = E_POINTER; return hr; }

	if (!pDevice || !pDContext) { hr = E_POINTER; return hr; }

	m_animInfo = pLoadData->animInfo;

	for (auto it : pLoadData->renderMeshUnitList)
	{
		CPRSFBXRenderUnit* newUnit = new CPRSFBXRenderUnit;

		newUnit->create(pDevice, pDContext, it, pLoadData);

		m_renderMeshUnitList.push_back(newUnit);
	}

	for (auto it : pLoadData->fbxUnitMap)
	{
		m_fbxUnitMap.insert(std::make_pair(it.first, it.second->animTracks));
	}

	m_fbxUnitIdxMap = pLoadData->fbxUnitIdxMap;

	return hr;
}

bool CPRSFBXMesh::init()
{
	return true;
}

bool CPRSFBXMesh::release()
{
	for (auto it : m_renderMeshUnitList)
	{
		it->release();
		delete it;
	}

	return true;
}

void CPRSFBXMesh::draw()
{
	for (auto it : m_renderMeshUnitList)
	{
		it->draw();
	}
}

void CPRSFBXMesh::draw(CPRS_ShaderGroup& sGruop, ID3D11PixelShader* pNoTexPS)
{
	for (auto it : m_renderMeshUnitList)
	{
		it->draw(sGruop, pNoTexPS);
	}
}

void CPRSFBXMesh::instanceDraw()
{
	for (auto it : m_renderMeshUnitList)
	{
		it->instanceDraw();
	}
}

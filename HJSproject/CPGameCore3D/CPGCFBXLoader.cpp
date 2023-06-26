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

	//�Ŵ��� ����
	m_pFBXMgr = FbxManager::Create();
	if (!m_pFBXMgr) { return false; }

	//Importer����
	m_pFBXImporter = FbxImporter::Create(m_pFBXMgr, "");	//�̸��� ������ ������� �ʴ´�.
	if (!m_pFBXImporter) { return false; }

	//�ε� �Ӽ� ���� ����
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

		//1. Scene ���� �� ����Ʈ
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

		//2. ��ǥ��� �ý��� ���� ����
		m_axisSystem.ConvertScene(pFBXScene);
		m_sysUnit.ConvertScene(pFBXScene);

		//3. ��Ʈ ��� �ҷ����� �Ľ��� ���� �ʿ��� ������ ����� ���� ��ó�� ����
		FbxNode* pRootNode = pFBXScene->GetRootNode();
		preProcess(pRootNode, nullptr, pFBXLoadData);

		//4.����� ���� �޽� ����Ʈ�� �̿��ؼ� �޽� �� �ִϸ��̼� �Ľ�
		//�ִϸ��̼� ������ ���� �ִϸ��̼� �⺻ ���� ��������
		getAnimInfo(pFBXScene, pFBXLoadData);

		//�Ľ̵� ��� ��忡 ���� �����Ӻ� ���� Ʈ������ ����
		FbxTime frameTime;
		for (FbxLongLong t = pFBXLoadData->animInfo.iStFrame; t <= pFBXLoadData->animInfo.iEdFrame; t++)
		{
			frameTime.SetFrame(t, m_timemode);
			getAnimTrackInfo(t, frameTime, pFBXLoadData);
		}

		//�޽� �� ��� �ؽ�ó ���� ����
		if (m_pMeshList.size())
		{
			for (auto pMesh : m_pMeshList)
			{
				std::string szMeshNodeName = pMesh->GetNode()->GetName();
				CPGCFBXLoadUnit* pTargetUnit = pFBXLoadData->fbxUnitMap.find(szMeshNodeName)->second;

				//�ִϸ��̼��� ����� �Ǵ� �޽��� ������ ������ �����ų 
				//������ �޴� Bone�� Local Ʈ������ ��İ� �ε��� �� ����ġ ������ �����´�.
				getMeshSkinningInfo(pMesh, pTargetUnit, pFBXLoadData);

				//������ �ϱ� ���� ���� ���� ������ �����´�.
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
			//�޽� ����Ʈ�� ����ִ� ��� �ִϸ��̼� ������ ����
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
	//FbxAMatrix : double => DXSimpleMath : float ��ȯ ����
	CPM_Matrix ret, temp;

	double* convFrom = (double*)&srcMat;
	float* convTo = (float*)&temp;

	for (int i = 0; i < 16; i++) { convTo[i] = convFrom[i]; }

	//�⺻ ������ MayaZUp���� �� �����Ƿ� Y/Z�� ������ �����Ѵ�.(2��� 3�� ��ġ, 2���� 3�� ��ġ)
	//�� �켱 ��� �����̹Ƿ� ���� �� ��
	//Ʈ������ ��������� ��ȯ �� ���̹Ƿ� �Ʒ��� ���� ó���Ѵ�.
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
	FbxLongLong st, ed;	//���� ������, �� ������
	st = ed = 0;

	//1. �ִϸ��̼� ������ ������ ������ FbxAnimStack ��������
	FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(0);	//ù��° ��

	//2. ���� �ð� ���� ���� - �� �ð� ������ �������� �ִϸ��̼��� �ð� ������ �籸���Ѵ�.
	FbxTime::SetGlobalTimeMode(m_timemode);	//scene�� ������ �ٲٴ� �κ�

	//3. �ҷ��� AnimStack�� ��ȿ�ϴٸ� ���� ���� �ϱ�
	//�ִϸ��̼� ������ ��� ����� �����ϰ� �ϱ� ���ؼ� ���ǿ� �����ϸ� �����Ѵ�.
	if (pAnimStack)
	{
		//4.1 AnimStack�� �̸��� ���� takeInfo�� �޾ƿ� �ð� ���� �����ϱ�
		FbxString szAnimStackName = pAnimStack->GetName();
		pLoadData->animInfo.wszAnimStackName = atl_M2W(pAnimStack->GetName());

		FbxTakeInfo* pTakeInfo = pScene->GetTakeInfo(szAnimStackName);

		//4.2 �ִϸ��̼��� ���۰� �� ���� ������ �籸�� �ϱ�
		FbxTimeSpan localTimeSpan = pTakeInfo->mLocalTimeSpan;
		FbxTime stT = localTimeSpan.GetStart();
		FbxTime edT = localTimeSpan.GetStop();

		st = stT.GetFrameCount(m_timemode);	//GetFrameCount(FbxTime::EMode) : �ð� ��尡 ����� ������ ī��Ʈ�� ��ȯ�Ѵ�.
		ed = edT.GetFrameCount(m_timemode);
	}

	//4. ������Ʈ �ִϸ��̼ǿ� �ʿ��� ������ AnimScene�� ������ ����
	pLoadData->animInfo.iStFrame = st;
	pLoadData->animInfo.iEdFrame = ed;
	pLoadData->animInfo.iTotalFrame = max(1, ed - st);				//���� �ҷ��� �� �ִ� �������� �� ���Ͽ� ���� �ִϸ��̼��� ������ ��� �ǹ̰� ���⿡ ���� �ִ´�.
	pLoadData->animInfo.fTickPerFrame = CPMAC_FBX_DEFAULT_TICKS;	//�⺻������ ����Ѵ� ���� ���������� �ʿ�� �ٲ� �� �ִ�.
	pLoadData->animInfo.fFPS = CPMAC_FBX_DEFAULT_FPS;				//�� �۷ι� ������ ������. �翬�� �ʿ�� �ٲ� �� �ִ�.
}

void CPGCFBXManager::preProcess(FbxNode* pNode, CPGCFBXLoadUnit* pParentUnit, CPGCFBXLoadData* pLoadData)
{
	//1. ī�޶� / �������� ĳ���� �Ǵ� ���� �޽ø� ������ �־ �Ľ� ��󿡼� �����Ѵ�.
	if (!pNode) { return; }
	if (pNode->GetCamera() || pNode->GetLight()) { return; }

	//2. ���� �������� ����
	CPGCFBXLoadUnit* pUnit = new CPGCFBXLoadUnit;
	pUnit->szNodename = pNode->GetName();

	//��Ʈ ����� ��� ����� �Ӽ��� �ο����� �ʴ´�.
	FbxNodeAttribute* pNodeAtt = pNode->GetNodeAttribute();
	if (pNodeAtt) { pUnit->nodeType = pNodeAtt->GetAttributeType(); } //����� �Ӽ��� �������� ���� ��쵵 �ִ�. ���� ��Ʈ ��尡 �̿� �ش��Ѵ�.
	else { pUnit->nodeType = FbxNodeAttribute::EType::eUnknown; }

	pUnit->pFbxNode = pNode;
	pUnit->pParentFbxNode = pNode->GetParent();
	pUnit->pParentUnit = pParentUnit;
	if (pParentUnit) { pParentUnit->pChilds.push_back(pUnit); }
	pUnit->iIdx = pLoadData->fbxUnitMap.size();			//��� ���ֿ� �ε��� �ο�

	//3. ��� �̸� �˻��� ���� ��ó��
	//����̸����� �ش� ���ְ� �ε����� ã�� �� �ֵ��� �Ѵ�.
	pLoadData->fbxUnitMap.insert(std::make_pair(pUnit->szNodename, pUnit));
	pLoadData->fbxUnitIdxMap.insert(std::make_pair(pUnit->szNodename, pUnit->iIdx));

	//4. ��忡 �޽ð� �ִ� ��� �Ľ̸���Ʈ�� �߰� : ��Ʈ ���� ���� ��ȸ�ϰ� �ȴ�.
	FbxMesh* pTemp = pNode->GetMesh();
	if (pTemp) { m_pMeshList.push_back(pTemp); }

	//5. ��� �۾� ����
	//��Ʈ�� ������ eNull, eMesh, eSkeleton Node�� �ش��ϴ� ��常 ��ó���Ͽ� ������Ʈ�� �����Ѵ�.
	//Default : GetChildCount(false) : �ڽ� ��常 ī�����Ѵ�.
	//GetChildCount(true) : ��� �ڽ� �� �ļ� ������ ī�����Ѵ�.

	UINT nChild = pNode->GetChildCount();

	for (UINT i = 0; i < nChild; i++)
	{
		//eLOD���� ���� ������ ������ �޽� ������ ������ �� �ֱ⿡ ��� ��带 �ϴ� ��ó�� �Ѵ�.
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

		//1. �����ӿ� �ش��ϴ� ���� Ʈ������ ����� ���ϰ� ����ϴ� ��� ���·� ��ȯ�Ѵ�.
		//FbxAMatrix�� �� �켱 ����̸� Ʈ�������� �����ϴ� ���ɺ�ȯ ����� ����Ѵ�.
		//EvaluateGlobalTransform : �𵨿� ����Ǵ� ���� ��ǥ���� Ʈ������ ����� ��ȯ�Ѵ�. 
		//FinalTransform = Local * AllParentLocalMat * globalTransform
		FbxAMatrix globalMat = itUnit.second->pFbxNode->EvaluateGlobalTransform(frameTime);
		newTrack.globalTransform = fbxAffineMatToDXMatrix(globalMat);

		//2. ������ ������ ���ؼ� ����� ������ ��ҷ� �����Ѵ�.
		//ȸ���� ���ʹϾ����� �����Ѵ�.
		newTrack.globalTransform.Decompose(newTrack.vS, newTrack.qR, newTrack.vT);
		itUnit.second->animTracks.push_back(newTrack);
	}
}

void CPGCFBXManager::getMeshSkinningInfo(FbxMesh* pMesh, CPGCFBXLoadUnit* pUnit, CPGCFBXLoadData* pLoadData)
{
	//1. skinning info �Ľ��� ���� �ʿ��� ��� �غ�
	int nDeformer = pMesh->GetDeformerCount(FbxDeformer::eSkin);	//��Ų ������ ����ְ� �ؾ��Ѵ�.
	if (!nDeformer) { pUnit->bSkinned = false; return; }

	int nVertex = pMesh->GetControlPointsCount();	
	pUnit->VertexIWs.resize(nVertex);					//�޽� ������ ���� ��ŭ �ε��� / ����ġ ����ü �迭 Ȯ���ϱ�

	//3. Deformer ������ŭ ���� �����ϱ�
	for (int iDeform = 0; iDeform < nDeformer; iDeform++)
	{
		//3-1 Deformer�� FbxSkin���� ĳ����
		FbxDeformer* pDeformer = pMesh->GetDeformer(iDeform, FbxDeformer::eSkin);
		std::string deformerName = pDeformer->GetName();
		FbxSkin* pSkin = (FbxSkin*)pDeformer;

		//3-2 FbxSkin�� cluster������ �̿��� �������� ������ boneIndex / Weight �����ϱ�
		int nCluster = pSkin->GetClusterCount();
		for (int iCluster = 0; iCluster < nCluster; iCluster++)
		{
			//�ϳ��� Cluster�� �������� �ϳ��� bone�� ������ �޴´�.
			//�� �������� ���� cluster�� ���� �� �� �ִ�.
			FbxCluster* pCluster = pSkin->GetCluster(iCluster);

			//3-3-1 cluster�� ������ �ִ� bone�� ���� ��� ������ �ε��� ��������
			std::string pBoneName = pCluster->GetLink()->GetName();
			int iBoneIdx = pLoadData->fbxUnitIdxMap.find(pBoneName.c_str())->second;

			//3-3-2 Bone�� Local��ǥ��� ��ȯ�ϴ� ��� �����ϱ� : �� �켱 ��� ���� �����̹Ƿ� ��������
			FbxAMatrix linkBoneBindPoseMat;						//LinkNode�� BindPose���� Ʈ������
			FbxAMatrix linkBoneParentMat;						//LinkNode�� biped�� ���ε� �Ǿ� ��ġ�ϱ� ���� �θ� Ʈ������
			pCluster->GetTransformLinkMatrix(linkBoneBindPoseMat);
			pCluster->GetTransformMatrix(linkBoneParentMat);

			//�� ������ FBX�� SDK�� �̿��� ���� ������ ���� ������ �ٸ� 3D�� ������ �̿� �ٸ� �� ������ ��������
			FbxAMatrix temp = linkBoneBindPoseMat.Inverse() * linkBoneParentMat;
			CPRS_BONEMATRIX localBoneMat;
			localBoneMat.szBoneName = pBoneName;
			localBoneMat.wszBoneName = atl_M2W(pBoneName);
			localBoneMat.boneMat = fbxAffineMatToDXMatrix(temp);

			pUnit->LocalBoneMatMap.insert(std::make_pair(iBoneIdx, localBoneMat));

			//3-3-3 cluster�� ���� �������� ������ �޴� bone frame IW���� �籸���ϱ�
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
	//0. �޽� ���������� ������ �ڷᱸ�� ����
	pLoadData->renderMeshUnitList.push_back(pUnit);

	//1. ��� ���� �޾ƿ��� ���� / ���� ��� �����ϱ�
	FbxNode* pMeshNode = pUnit->pFbxNode;

	//������Ʈ�� ��� ������ : pivot context�� �̵���Ű�� ���
	//�θ� �ڽİ��� ��ӵ��� ������ ��� ������ �������� ����Ǿ�� �ϴ� ���� �̵� ��Ŀ� �ش��Ѵ�.
	//�翬�� ���� ����� �� �ִ�.
	FbxAMatrix geometryMat;

	FbxVector4 LocT = pMeshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 LocR = pMeshNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 LocS = pMeshNode->GetGeometricScaling(FbxNode::eSourcePivot);

	//T R S�� : �� �켱 ���
	geometryMat.SetT(LocT);
	geometryMat.SetR(LocR);
	geometryMat.SetS(LocS);

	//�븻�� ������� ��ġ����� ���ؾ� ����� ���ȴٰ� �Ѵ�.
	FbxAMatrix normalLocalMatrix = geometryMat;
	normalLocalMatrix = normalLocalMatrix.Inverse();
	normalLocalMatrix = normalLocalMatrix.Transpose();

	//2. ���̾�� ���ҽ� ���� �б�

	//���� ������Ʈ���� �۾��� ���ҽ� �� ����Ʈ
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

		//Layer���� ���ҽ� Set��������
		m_LayerVTNormalSets[layerIdx] = m_Layers[layerIdx]->GetNormals();		//normals
		m_LayerVTColorSets[layerIdx] = m_Layers[layerIdx]->GetVertexColors();	//vertex colors
		m_LayerUVSets[layerIdx] = m_Layers[layerIdx]->GetUVs();					//UV colors
		m_LayerMTSets[layerIdx] = m_Layers[layerIdx]->GetMaterials();			//materials
	}

	//3. node�� Material�� ���� ���� �ؽ�ó ���� ���� �о����
	UINT nMaterial = pMeshNode->GetMaterialCount();

	//�ؽ�ó ���ϸ���Ʈ�� ���� ������ ���� ���� ����Ʈ�� �� �� ����� �д�.
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
		//Material���� Ư�� �ؽ�ó ������ �����´�.
		//���⿡���� diffuse�ؽ�ó ������ ����Ѵ�.
		FbxSurfaceMaterial* pSurfaceMt = pMeshNode->GetMaterial(mtIdx);

		if (pSurfaceMt)
		{
			FbxProperty sufProp = pSurfaceMt->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (sufProp.IsValid())
			{
				//FbxFileTextureŬ������ �ش��ϴ� ù��° ������ ������ �ҷ��´�.
				FbxFileTexture* pTex = sufProp.GetSrcObject<FbxFileTexture>(0);

				if (pTex)
				{
					//������ ���� ��η� �ҷ��� ����.
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

	//4.������ ���� �о� ���� �����ϱ� - �� 1���� ���̾ ������ ����Ѵ�.
	UINT nPolygon = pMesh->GetPolygonCount();
	UINT iBaseVTIdx = 0;
	UINT iSubMTIdx = 0;

	//������ ����Ʈ�� �о� �´�.
	FbxVector4* pVertexPositions = pMesh->GetControlPoints();

	for (int PGIdx = 0; PGIdx < nPolygon; PGIdx++)
	{
		//�ش� �������� ���� ��� �̷���� �ִ����� ��ȯ�Ѵ�.
		UINT iPGSize = pMesh->GetPolygonSize(PGIdx);

		//4���� 1�������̸� �簢���̹Ƿ� 2������ ����
		//3���� 1�������̸� �ﰢ���̹Ƿ� �״�� 1��
		UINT faceVNum = iPGSize - 2;

		if (m_LayerMTSets.size())
		{
			iSubMTIdx = getSubMaterialIndex(PGIdx, m_LayerMTSets[0]);
		}

		for (int FaceIdx = 0; FaceIdx < faceVNum; FaceIdx++)
		{
			//��ġ ���� �ε��� : Max�� �ݽð� �������� �����Ǿ�� �ո��̴�.
			// a - d
			// | \ |
			// b - c
			//DX�� �ð�������� �����Ǿ�� �ϹǷ� �ε��� ������ ������ ���� �ٲ��.
			// MAX : a - b - c / a - c - d
			// DX : a - c - b / a - d - c

			//��ǥ�谡 �ٸ��Ƿ�(y, z switch) ������ ������ �̷��� �����ȴ�.
			UINT iCornerIdx[3];
			iCornerIdx[0] = pMesh->GetPolygonVertex(PGIdx, 0);
			iCornerIdx[1] = pMesh->GetPolygonVertex(PGIdx, FaceIdx + 2);
			iCornerIdx[2] = pMesh->GetPolygonVertex(PGIdx, FaceIdx + 1);

			pUnit->IBDataList[iSubMTIdx].push_back(iCornerIdx[0]);
			pUnit->IBDataList[iSubMTIdx].push_back(iCornerIdx[1]);
			pUnit->IBDataList[iSubMTIdx].push_back(iCornerIdx[2]);

			//���� �ؽ�ó �ε���
			UINT iUVIdx[3];
			iUVIdx[0] = pMesh->GetTextureUVIndex(PGIdx, 0);
			iUVIdx[1] = pMesh->GetTextureUVIndex(PGIdx, FaceIdx + 2);
			iUVIdx[2] = pMesh->GetTextureUVIndex(PGIdx, FaceIdx + 1);

			//���� �÷� �ε���
			UINT iVTColorIdx[3] = { 0, FaceIdx + 2 , FaceIdx + 1 };

			//���� �븻 �ε���
			UINT iVTNormalIdx[3] = { 0, FaceIdx + 2 , FaceIdx + 1 };

			//���� ������ : �鸶�� 3��
			for (int VtIdx = 0; VtIdx < 3; VtIdx++)
			{
				CPRS_SkinningVertex	newV;

				//��ġ
				FbxVector4 v = pVertexPositions[iCornerIdx[VtIdx]];
				v = geometryMat.MultT(v);

				newV.v.p.x = v.mData[0];
				newV.v.p.y = v.mData[2];
				newV.v.p.z = v.mData[1];

				//�븻
				newV.v.n = { 0.0f, 0.0f, 0.0f };

				if (m_LayerVTNormalSets[0])
				{
					FbxVector4 n = readNormal(pMesh,
						m_LayerVTNormalSets[0],
						iCornerIdx[VtIdx],
						iBaseVTIdx + iVTNormalIdx[VtIdx]);	//����κ��� �ٸ� : �����︶�� ��ȣ�� �����ȴ�.

					n = normalLocalMatrix.MultT(n);

					newV.v.n.x = n.mData[0];
					newV.v.n.y = n.mData[2];
					newV.v.n.z = n.mData[1];
				}

				//�÷�
				newV.v.c = { 1.0f, 1.0f, 1.0f, 1.0f };	//�⺻ ������

				if (m_LayerVTColorSets[0])
				{
					FbxColor c = readColor(pMesh,
						m_LayerVTColorSets[0],
						iCornerIdx[VtIdx],
						iBaseVTIdx + iVTColorIdx[VtIdx]);	//����κ��� �ٸ� : �����︶�� ��ȣ�� �����ȴ�.

					newV.v.c.x = c.mRed;
					newV.v.c.y = c.mGreen;
					newV.v.c.z = c.mBlue;
					newV.v.c.w = 1.0f;		//c.mAlpha;
				}

				//�ؽ�ó
				if (m_LayerUVSets[0])
				{
					FbxVector2 t = readTextureCoord(pMesh,
						m_LayerUVSets[0],
						iCornerIdx[VtIdx],
						iUVIdx[VtIdx]);

					newV.v.t.x = t.mData[0];
					newV.v.t.y = 1.0f - t.mData[1];
				}

				//��� �ε����� ����ġ
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
					//�������� ������ �ڵ� �ʱ�ȭ
					newV.i.x = 0.0f;

					//��¥�� ����ġ�� 0.0f�� �κ��� �ε����� � ���̾ ��� ����
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
	//1. �ؽ�ó ���� ���Ŀ� ���� �б⸦ ������. (EMappingMode)
	//	eByControlPoint�� eByPolygonVertex�� Material�� ������ ��ҿ� ���ȴ�.
	//	eByPolygon�� eByAllSame�� Material�� ���� ���ȴ�.

	//2. ������ �ּ� ���� ��Ŀ� ���� �б⸦ ������. (EReferenceMode)
	//eIndex�� FBX 5.0���� �������� deplecated�� �κп� �ش��Ѵ�.
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
	//ȸ���� ���� ���� ������ ȸ������ ���ͷ� ǥ���ϱ⿡�� ������ ���ʿ��ϰ� ����.
	//������ �࿡ ���� ȸ���� ���� �Ǵ� �ٸ� ȸ�� ����� �������� ���� ���Ŀ� 
	//����� ������Ŀ��� �����ϴ� ���� �Ϲ����̴�.
	//�� �Լ��� ������ ������ ��ȭ�� �ݿ������� �ʴ´�.

	// ���� ��� R*T
	// Right.x	Right.y	Right.z	0
	// Up.x		Up.y	Up.z	0
	// Look.x	Look.y	Look.z	0
	// Pos.x	Pos.y	Pos.z	1

	//��ġ ������ ��Ŀ��� �����Ѵ�.
	m_vPrevPos = m_vPos;
	m_vPos = { m_matWorld._41, m_matWorld._42, m_matWorld._43 };

	//obj Right-Axis
	m_sightAxis.vRight = { m_matWorld._11, m_matWorld._12, m_matWorld._13 };

	//obj Up-Axis
	m_sightAxis.vUp = { m_matWorld._21, m_matWorld._22, m_matWorld._23 };

	//obj At-Axis
	m_sightAxis.vLook = { m_matWorld._31, m_matWorld._32 , m_matWorld._33 };

	//ȸ�� ��Ŀ��� ������ �� �຤�ʹ� ������ ��Ŀ� ���� ����ȭ�Ǿ����� ���� ���¶�� �����Ѵ�.
	m_sightAxis.vRight.Normalize();
	m_sightAxis.vUp.Normalize();
	m_sightAxis.vLook.Normalize();

	//�ٿ�� ������ ������ �����ϴ� �κ��� �߰��Ǿ�� �Ѵ�.
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
		//�޾ƿ� GPU���� �ּҿ� ������Ʈ ��Ų ��� ���� ������ ����
		//�κ��� ������ ����
		memcpy(mappedCbuf.pData, &m_wvpMat, sizeof(CPRS_CBUF_COORDCONV_MATSET));

		//���� Unmap
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

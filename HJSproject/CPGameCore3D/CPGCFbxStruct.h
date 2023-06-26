#pragma once
#include "CPGCStd.h"
#include "CPRSVertex.h"
#include "CPGCCollision.h"
#include "fbxsdk.h"

#define CPMAC_FBX_DEFAULT_TICKS							160			
#define CPMAC_FBX_DEFAULT_FPS							30			//FbxTime::EMode::eFrames30
#define CPMAC_MAX_IW_SIZE								8			//������ �޴� Ʈ������ �ִ� ���� ����
#define CPMAC_MAX_BONE_NUMBER							256
#define CPMAC_FBX_TEXTURE_FILE_DIRECTORY_PATH			L"../../data/fbx/texture/"

//�����Ӻ� ���� Ʈ������ ���� 
struct CPRS_AnimTrack
{
	int					iFrame;

	CPM_Matrix			globalTransform;
	CPM_Vector3			vS;
	CPM_Quaternion		qR;
	CPM_Vector3			vT;
};

//�ִϸ��̼� ������ ���� ����
struct CPRS_AnimInfo
{
	std::wstring	wszAnimStackName;		//�ش� �ִϸ��̼��� �̸�
	int				iStFrame;				//���� ������
	int				iEdFrame;				//�� ������
	int				iTotalFrame;			//�� ������ ��
	FLOAT			fTickPerFrame;			//�����Ӵ� ƽ
	FLOAT			fFPS;					//�ʴ� ������
};

struct CPRS_CBUF_ANIMATION_MATS
{
	CPM_Matrix	matTAnimMats[CPMAC_MAX_BONE_NUMBER];
};

struct CPRS_BONEMATRIX
{
	std::string		szBoneName;
	std::wstring	wszBoneName;
	CPM_Matrix		boneMat;
};

struct CPRS_AnimState
{
	CPRS_AnimInfo		curAnimInfo;
	float				fReDefStFrame;
	float				fReDefEdFrame;
	float				fCurFrame;
	float				fAnimSpeed;
	bool				bLoop;
};

struct CPRS_VertexIWs
{
	std::vector<int>				boneIndexs;			//������ ��ġ�� Bone�� �ε��� ��ȣ - FbxNodeAttribute::EType::eSkeleton ��尡 ����̴�.
	std::vector<float>				combinedWeights;	//���� ����ġ. �ε�������Ʈ�� 1:1��Ī�ȴ�.

	CPRS_VertexIWs()
	{
		boneIndexs.resize(CPMAC_MAX_IW_SIZE);
		combinedWeights.resize(CPMAC_MAX_IW_SIZE);
	}

	void add(int boneIdx, float weight)
	{
		if (weight <= combinedWeights[CPMAC_MAX_IW_SIZE - 1]) { return; }

		int cnt = CPMAC_MAX_IW_SIZE - 1;

		for (; cnt > 0; cnt--)
		{
			if (weight > combinedWeights[cnt - 1])
			{
				boneIndexs[cnt] = boneIndexs[cnt - 1];
				combinedWeights[cnt] = combinedWeights[cnt - 1];
			}
			else { break; }
		}

		boneIndexs[cnt] = boneIdx;
		combinedWeights[cnt] = weight;
	}
};

struct CPGCFBXLoadUnit
{
	//���� �ĺ� ����
	std::string														szNodename;
	FbxNodeAttribute::EType											nodeType;
	int																iIdx;

	//���� Ʈ�� ����
	FbxNode*														pFbxNode;
	FbxNode*														pParentFbxNode;
	CPGCFBXLoadUnit*												pParentUnit;
	std::vector<CPGCFBXLoadUnit*>									pChilds;

	//���� �ִϸ��̼� ����
	std::vector<CPRS_AnimTrack>										animTracks;			//�����Ӵ����� ����Ǵ� ���� Ʈ������ ����Ʈ

	//���� ��Ű�� �ִϸ��̼� ����
	bool															bSkinned;
	std::vector<CPRS_VertexIWs>										VertexIWs;

	std::vector<std::wstring>										wszTexNameList;
	std::vector<std::vector<CPRS_SkinningVertex>>					VBDataList;
	std::vector<std::vector<UINT>>									IBDataList;

	std::map<int, CPRS_BONEMATRIX>									LocalBoneMatMap;

	CPM_Vector3														minXYZ = {HUGE_VALF, HUGE_VALF, HUGE_VALF };
	CPM_Vector3														maxXYZ = {-HUGE_VALF, -HUGE_VALF, -HUGE_VALF };

	CPGCBoundingVolume												BVolume;
};

struct CPGCFBXLoadData
{
	CPRS_AnimInfo													animInfo;

	std::vector<CPGCFBXLoadUnit*>									renderMeshUnitList;
	std::map<std::string, CPGCFBXLoadUnit*>							fbxUnitMap;
	std::map<std::string, int>										fbxUnitIdxMap;
};
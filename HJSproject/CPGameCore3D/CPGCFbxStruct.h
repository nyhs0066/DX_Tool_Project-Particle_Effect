#pragma once
#include "CPGCStd.h"
#include "CPRSVertex.h"
#include "CPGCCollision.h"
#include "fbxsdk.h"

#define CPMAC_FBX_DEFAULT_TICKS							160			
#define CPMAC_FBX_DEFAULT_FPS							30			//FbxTime::EMode::eFrames30
#define CPMAC_MAX_IW_SIZE								8			//영향을 받는 트랜스폼 최대 적용 개수
#define CPMAC_MAX_BONE_NUMBER							256
#define CPMAC_FBX_TEXTURE_FILE_DIRECTORY_PATH			L"../../data/fbx/texture/"

//프레임별 전역 트랜스폼 정보 
struct CPRS_AnimTrack
{
	int					iFrame;

	CPM_Matrix			globalTransform;
	CPM_Vector3			vS;
	CPM_Quaternion		qR;
	CPM_Vector3			vT;
};

//애니메이션 프레임 구성 정보
struct CPRS_AnimInfo
{
	std::wstring	wszAnimStackName;		//해당 애니메이션의 이름
	int				iStFrame;				//시작 프레임
	int				iEdFrame;				//끝 프레임
	int				iTotalFrame;			//총 프레임 수
	FLOAT			fTickPerFrame;			//프레임당 틱
	FLOAT			fFPS;					//초당 프레임
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
	std::vector<int>				boneIndexs;			//영향을 미치는 Bone의 인덱스 번호 - FbxNodeAttribute::EType::eSkeleton 노드가 대상이다.
	std::vector<float>				combinedWeights;	//결합 가중치. 인덱스리스트와 1:1매칭된다.

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
	//유닛 식별 정보
	std::string														szNodename;
	FbxNodeAttribute::EType											nodeType;
	int																iIdx;

	//유닛 트리 구조
	FbxNode*														pFbxNode;
	FbxNode*														pParentFbxNode;
	CPGCFBXLoadUnit*												pParentUnit;
	std::vector<CPGCFBXLoadUnit*>									pChilds;

	//유닛 애니메이션 정보
	std::vector<CPRS_AnimTrack>										animTracks;			//프레임단위로 적용되는 전역 트랜스폼 리스트

	//유닛 스키닝 애니메이션 정보
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
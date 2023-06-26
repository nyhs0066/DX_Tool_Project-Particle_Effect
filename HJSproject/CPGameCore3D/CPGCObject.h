//////////////////////////////////////////////////
//
// CPGCObject.h
//		렌더작업의 대상 클래스
//		CPGCObject : 렌더 공간에 배치될 수 있는 모든 오브젝트의 원형이되는 기본 오브젝트 클래스
//		CPGCActor : 공간에 직접 배치 가능한 트랜스폼을 가지는 오브젝트
// 
//		다음의 작업이 필요하다.
//			오브젝트의 식별을 위한 고유 식별자 부여
//			오브젝트 풀의 운영방식
//			컴포넌트 구성방식의 도입 시기 결정
// 
//////////////////////////////////////////////////

#pragma once
#include "CPRSMeshManager.h"
#include "CPRSMaterial.h"
#include "CPRSShape.h"
#include "CPGCTimer.h"
#include "CPGCInput.h"
#include "CPGCCollision.h"

//렌더 공간에 배치될 수 있는 모든 오브젝트의 원형이되는 기본 오브젝트 클래스
class CPGCObject
{
protected:
	std::string									m_szName;
	std::wstring								m_wszName;
	ID3D11Device*								m_pDevice;
	ID3D11DeviceContext*						m_pDContext;

public:
	CPGCObject();
	virtual ~CPGCObject();

	//기본 함수
	virtual bool init() { return true; }

	virtual bool update() { return true; }
	virtual bool preUpdate() { return true; }
	virtual bool postUpdate() { return true; }

	virtual bool render() { return true; }
	virtual bool preRender() { return true; }
	virtual bool postRender() { return true; }

	virtual bool release() { return true; }

	const WCHAR* getWszName()
	{
		if (m_wszName.size()) { return m_wszName.c_str(); }
		return nullptr;
	}

	const CHAR* getSzName()
	{
		if (m_szName.size()) { return m_szName.c_str(); }
		return nullptr;
	}

	void setName(std::wstring wszName)
	{
		m_wszName = wszName;
		m_szName = atl_W2M(wszName);
	}

	void setName(std::string szName)
	{
		m_szName = szName;
		m_wszName = atl_M2W(szName);
	}

	virtual void setDevice(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pDContext);
};


class CPGCActor : public CPGCObject
{
protected:
	CPRS_Mesh*									m_pMesh;

	//초기 트랜스폼
	CPRS_TRANSFORM								m_initTransform;

	//위치
	CPM_Vector3									m_vPrevPos;
	CPM_Vector3									m_vPos;

	//오브젝트 기저 축
	CPRS_AXIS									m_sightAxis;

	//월드, 뷰, 투영 행렬
	CPM_Matrix									m_matWorld;
	CPM_Matrix									m_matView;
	CPM_Matrix									m_matProj;

	CPRS_CBUF_COORDCONV_MATSET					m_wvpMat;		//VS에 0번 상수버퍼로 바인딩한다.

	std::vector<ID3D11Buffer*>					m_pVSCBufs;		//VS단계용 상수버퍼 그룹
	UINT										m_iCBufCount;

	//물리 적용 상태 구조체
	CPRS_BASE_PHYSICS_STATE						m_physicsState;

	std::vector<CPGCActor*>						m_pChildList;

public:
	//Shader Group & Samplers
	CPRS_ShaderGroup							m_shaderGroup;
	std::vector<ID3D11SamplerState*>			m_pSamplers;
	UINT										m_iSamplerCount;

public:
	CPGCActor();
	virtual ~CPGCActor();

	//기본 함수
	virtual bool								init();

	virtual bool								update();
	virtual bool								preUpdate();
	virtual bool								postUpdate();

	virtual bool								render();
	virtual bool								preRender();
	virtual bool								postRender();

	virtual bool								release();

	HRESULT										create(std::wstring wszObjName,
														std::wstring wszMeshName);

	void										addSampler(std::wstring wszSamplerName);

	//좌표계 변환 행렬 세팅 함수
	virtual void								setMatrix(const CPM_Matrix* pWorldM,
		const CPM_Matrix* pViewM,
		const CPM_Matrix* pProjM);

	HRESULT										updateCoordConvMat(const CPM_Matrix& matWorld,
		const CPM_Matrix& matView,
		const CPM_Matrix& matProj);

	//변화한 속성값을 갱신하는 함수
	virtual void								updateState();

	//Mesh 세팅 함수
	virtual void								setMesh(std::wstring wszMeshName);

	//파이프 라인 바인딩 함수
	virtual void								bindToPipeline();

	CPM_Vector3									getVRight();
	CPM_Vector3									getVUp();
	CPM_Vector3									getVForward();

};
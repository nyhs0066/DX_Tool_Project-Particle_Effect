//////////////////////////////////////////////////
//
// CPGCObject.h
//		�����۾��� ��� Ŭ����
//		CPGCObject : ���� ������ ��ġ�� �� �ִ� ��� ������Ʈ�� �����̵Ǵ� �⺻ ������Ʈ Ŭ����
//		CPGCActor : ������ ���� ��ġ ������ Ʈ�������� ������ ������Ʈ
// 
//		������ �۾��� �ʿ��ϴ�.
//			������Ʈ�� �ĺ��� ���� ���� �ĺ��� �ο�
//			������Ʈ Ǯ�� ����
//			������Ʈ ��������� ���� �ñ� ����
// 
//////////////////////////////////////////////////

#pragma once
#include "CPRSMeshManager.h"
#include "CPRSMaterial.h"
#include "CPRSShape.h"
#include "CPGCTimer.h"
#include "CPGCInput.h"
#include "CPGCCollision.h"

//���� ������ ��ġ�� �� �ִ� ��� ������Ʈ�� �����̵Ǵ� �⺻ ������Ʈ Ŭ����
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

	//�⺻ �Լ�
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

	//�ʱ� Ʈ������
	CPRS_TRANSFORM								m_initTransform;

	//��ġ
	CPM_Vector3									m_vPrevPos;
	CPM_Vector3									m_vPos;

	//������Ʈ ���� ��
	CPRS_AXIS									m_sightAxis;

	//����, ��, ���� ���
	CPM_Matrix									m_matWorld;
	CPM_Matrix									m_matView;
	CPM_Matrix									m_matProj;

	CPRS_CBUF_COORDCONV_MATSET					m_wvpMat;		//VS�� 0�� ������۷� ���ε��Ѵ�.

	std::vector<ID3D11Buffer*>					m_pVSCBufs;		//VS�ܰ�� ������� �׷�
	UINT										m_iCBufCount;

	//���� ���� ���� ����ü
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

	//�⺻ �Լ�
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

	//��ǥ�� ��ȯ ��� ���� �Լ�
	virtual void								setMatrix(const CPM_Matrix* pWorldM,
		const CPM_Matrix* pViewM,
		const CPM_Matrix* pProjM);

	HRESULT										updateCoordConvMat(const CPM_Matrix& matWorld,
		const CPM_Matrix& matView,
		const CPM_Matrix& matProj);

	//��ȭ�� �Ӽ����� �����ϴ� �Լ�
	virtual void								updateState();

	//Mesh ���� �Լ�
	virtual void								setMesh(std::wstring wszMeshName);

	//������ ���� ���ε� �Լ�
	virtual void								bindToPipeline();

	CPM_Vector3									getVRight();
	CPM_Vector3									getVUp();
	CPM_Vector3									getVForward();

};
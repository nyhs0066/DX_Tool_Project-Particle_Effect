//////////////////////////////////////////////////
//
// CPGCMesh.h
//		���� ����� �޽� ���� ������ �����ϴ� ����ü ���
//		�޽ô� �������� ���� ������ ������ ������Ʈ���� �����Ѵ�.
// 
//		D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT : Shader�ܰ�� ������� �ִ� ���ε� ���� ����
// 
//		�ܰ躰 ���̴� ������۴� ���������� ���� � ���ε� �Ǿ������� �����ϴ� �� ������?
//			�ƴ�. ������ �����ϴ°��� ���� ������ �� ������ �ǰ� ������ �� �򰥸��ž�
// 
//		MeshManager�� �����ؼ� Mesh�� create������ �ѹ��� �Ͼ���� �ϰ� ������� Mesh�� ���� �� �� �ֵ��� �ؾ��Ѵ�.
// 
//////////////////////////////////////////////////

#pragma once
#include "CPRSVertex.h"

#define MAX_NUMBER_OF_CIRCLE_VERTEX 60

enum CPRS_MESH_PRESET
{
	CPRS_SM_RECT,
	CPRS_SM_PLANE,
	CPRS_SM_TEXCUBE,
	NUMBER_OF_MESH_PRESET
};

//�ܺ� ���� ������ ���
HRESULT createDXSimpleBuf(ID3D11Device* pDevice,
	size_t iBufSize,
	void* pResource,
	D3D11_BIND_FLAG bindType,
	ID3D11Buffer** ppOut);

HRESULT createDXBuf(ID3D11Device* pDevice,
	D3D11_BUFFER_DESC* pBd,
	D3D11_SUBRESOURCE_DATA* pSd,
	ID3D11Buffer** ppOut);

struct CPRS_Mesh
{
	//�޽� �̸�
	std::wstring											m_wszName;

	//�޽� ���ҽ� ����
	std::vector<CPRS_BaseVertex>							m_vertices;
	std::vector<UINT>										m_indices;

	//���ҽ� ī��Ʈ
	UINT													m_iVertexCount;
	UINT													m_iIndexCount;

	//�������ۿ� ���� �⺻��� ������ ���� �߰� ����	
	UINT													m_iStride;
	UINT													m_iOffset;
	D3D11_PRIMITIVE_TOPOLOGY								m_primitive;

	Microsoft::WRL::ComPtr<ID3D11Buffer>					m_pVBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>					m_pIBuf;
	ID3D11InputLayout*										m_pVLayout;

public:
	CPRS_Mesh();
	virtual ~CPRS_Mesh();

	virtual bool				init();
	virtual bool				release();

	virtual void				bindToPipeline(ID3D11DeviceContext* pDContext);
};

class CPGCMeshManager
{
private:
	ID3D11Device*							m_pDevice;
	ID3D11DeviceContext*					m_pDContext;

	std::set<std::wstring>					m_scriptFilenameSet;

	std::map<std::wstring, std::wstring>	m_fbxFilenameMap;
	std::map<std::wstring, CPRS_Mesh*>		m_meshMap;

	CPGCMeshManager();
	~CPGCMeshManager();

	//Mesh �����Լ�
	virtual HRESULT				createMeshBuf(CPRS_Mesh* pMesh, UINT iVFormatSize);

	virtual bool				setVertices(CPRS_BaseVertex* vertexArr, UINT iCnt, CPRS_Mesh* pMesh);
	virtual bool				setIndices(UINT* indexArr, UINT iCnt, CPRS_Mesh* pMesh);

	//Mesh DX �������̽� �����Լ�
	virtual HRESULT				createVBuf(CPRS_Mesh* pMesh, UINT iFormatSize);
	virtual HRESULT				createIBuf(CPRS_Mesh* pMesh);

public:
	CPGCMeshManager(const CPGCMeshManager& other) = delete;
	CPGCMeshManager& operator=(const CPGCMeshManager& other) = delete;

public:

	bool				init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);
	bool				release();

	HRESULT				createMesh(	std::wstring wszMeshName,
									CPRS_BaseVertex* vertexArr,
									UINT iVCnt,
									UINT* indexArr,
									UINT iICnt,
									std::wstring wszLayoutName,
									D3D11_PRIMITIVE_TOPOLOGY primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void				createDefaultMeshPreset();

	CPRS_Mesh*			getPtr(std::wstring wszMeshName);

	static CPGCMeshManager& getInstance()
	{
		static CPGCMeshManager singleInst;
		return singleInst;
	}
};

#define CPGC_MESH_MGR CPGCMeshManager::getInstance()
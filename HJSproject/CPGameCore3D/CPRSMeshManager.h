//////////////////////////////////////////////////
//
// CPGCMesh.h
//		정점 덩어리인 메시 대한 정보를 보관하는 구조체 헤더
//		메시는 정점군에 대한 구조만 렌더는 오브젝트에서 수행한다.
// 
//		D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT : Shader단계당 상수버퍼 최대 바인딩 가능 개수
// 
//		단계별 쓰이는 상수버퍼는 전역변수로 현재 몇개 바인딩 되었는지를 관리하는 게 좋을까?
//			아니. 쓰임을 고정하는것이 유지 관리에 더 도움이 되고 지금은 안 헷갈릴거야
// 
//		MeshManager를 도입해서 Mesh의 create과정이 한번만 일어나도록 하고 만들어진 Mesh가 관리 될 수 있도록 해야한다.
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

//외부 버퍼 생성시 사용
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
	//메시 이름
	std::wstring											m_wszName;

	//메시 리소스 변수
	std::vector<CPRS_BaseVertex>							m_vertices;
	std::vector<UINT>										m_indices;

	//리소스 카운트
	UINT													m_iVertexCount;
	UINT													m_iIndexCount;

	//정점버퍼와 기하 기본요소 세팅을 위한 추가 변수	
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

	//Mesh 생성함수
	virtual HRESULT				createMeshBuf(CPRS_Mesh* pMesh, UINT iVFormatSize);

	virtual bool				setVertices(CPRS_BaseVertex* vertexArr, UINT iCnt, CPRS_Mesh* pMesh);
	virtual bool				setIndices(UINT* indexArr, UINT iCnt, CPRS_Mesh* pMesh);

	//Mesh DX 인터페이스 생성함수
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
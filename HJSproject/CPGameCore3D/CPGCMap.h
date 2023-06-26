#pragma once
#include "CPGCObject.h"
#include "CPGCCollision.h"

struct VTNormalLookupInfo
{
	std::vector<UINT> adjFaceIdxList;
	CPM_Vector3 vNormal;
};

class CPGCMap : public CPGCObject
{
public:
	//위치
	CPM_Vector3									m_vPrevPos;
	CPM_Vector3									m_vPos;

	//초기 트랜스폼
	CPRS_TRANSFORM								m_initTransform;

	//오브젝트 기저 축
	CPRS_AXIS									m_sightAxis;

	//월드, 뷰, 투영 행렬
	CPM_Matrix									m_matWorld;
	CPM_Matrix									m_matView;
	CPM_Matrix									m_matProj;

	//물리 적용 상태 구조체
	CPRS_BASE_PHYSICS_STATE						m_physicsState;

	//맵 속성
	FLOAT										m_fCellSize;
	FLOAT										m_fHeightScale;
	UINT										m_iRowTexTileNum;

	//row와 col의 셀 개수는 각각 width -1, height -1이다.
	UINT										m_iWidth;
	UINT										m_iHeight;
	UINT										m_nFace;

	std::vector<VTNormalLookupInfo>				m_VTNLookupTable;

	UINT										m_iHeightMapWidth;
	UINT										m_iHeightMapHeight;
	std::vector<FLOAT>							m_fHeightList;

	//렌더 정점 정보
	std::vector<CPRS_BaseVertex>				m_vertices;
	std::vector<UINT>							m_indices;
	CPRS_CBUF_COORDCONV_MATSET					m_wvpMat;

	UINT										m_iVertexCount;
	UINT										m_iIndexCount;

	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pVBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pIBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pCBuf;

	//렌더 속성
	CPRS_ShaderGroup							m_shaderGroup;
	CPRS_Texture*								m_pTexture;
	ID3D11InputLayout*							m_pVLayout;

	std::vector<ID3D11SamplerState*>			m_pSamplers;

	CPGCMap();
	~CPGCMap();

	bool init();
	bool update();
	bool render();
	bool release();

	//공간 분할 적용시에 용이성을 생각하면 만들때 치수를 2^n + 1로 만드는 것이 권장된다.
	//build먼저(정점 정보 생성) create나중에(정점 버퍼 생성)
	bool build(CPM_Vector3 vCPos, int iCol, int iRow, float fCellSize);
	HRESULT create(	ID3D11Device* pDevice,
					ID3D11DeviceContext* pDContext,
					std::wstring wszTextureName,
					CPM_Vector3 vCPos,
					int iCol, int iRow, float fCellSize,
					std::wstring wszHeightMapName = std::wstring(),
					std::wstring wszHeightMapFileName = std::wstring());


	void setMatrix(const CPM_Matrix* matWorld, const CPM_Matrix* matView, const CPM_Matrix* matProj);

	void updateState();
	HRESULT updateCoordConvMat(const CPM_Matrix& matWorld, const CPM_Matrix& matView, const CPM_Matrix& matProj);

	void bindToPipeline();

	//Height Texture Map으로 부터 높이 값을 추출하는 함수
	HRESULT loadHeightMap(std::wstring wszHeightMapName, std::wstring wszTexFlieName);

	//정점 노말을 구하는 함수
	void generateVTNLookupTable();

	//face의 노말을 구하는 함수 : 3정점을 이용해 구한다.
	CPM_Vector3 calcFaceNormal(UINT i0, UINT i1, UINT i2);
	void calcVertexNormal(UINT iVertex);

	//맵에서 해당위치의 높이값 가져오기
	FLOAT getHeight(CPM_Vector3 vPos);
	FLOAT lerp(FLOAT opd1, FLOAT opd2, FLOAT fRatio);	//높이값 보간함수
};

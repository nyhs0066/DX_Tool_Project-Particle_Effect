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
	//��ġ
	CPM_Vector3									m_vPrevPos;
	CPM_Vector3									m_vPos;

	//�ʱ� Ʈ������
	CPRS_TRANSFORM								m_initTransform;

	//������Ʈ ���� ��
	CPRS_AXIS									m_sightAxis;

	//����, ��, ���� ���
	CPM_Matrix									m_matWorld;
	CPM_Matrix									m_matView;
	CPM_Matrix									m_matProj;

	//���� ���� ���� ����ü
	CPRS_BASE_PHYSICS_STATE						m_physicsState;

	//�� �Ӽ�
	FLOAT										m_fCellSize;
	FLOAT										m_fHeightScale;
	UINT										m_iRowTexTileNum;

	//row�� col�� �� ������ ���� width -1, height -1�̴�.
	UINT										m_iWidth;
	UINT										m_iHeight;
	UINT										m_nFace;

	std::vector<VTNormalLookupInfo>				m_VTNLookupTable;

	UINT										m_iHeightMapWidth;
	UINT										m_iHeightMapHeight;
	std::vector<FLOAT>							m_fHeightList;

	//���� ���� ����
	std::vector<CPRS_BaseVertex>				m_vertices;
	std::vector<UINT>							m_indices;
	CPRS_CBUF_COORDCONV_MATSET					m_wvpMat;

	UINT										m_iVertexCount;
	UINT										m_iIndexCount;

	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pVBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pIBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pCBuf;

	//���� �Ӽ�
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

	//���� ���� ����ÿ� ���̼��� �����ϸ� ���鶧 ġ���� 2^n + 1�� ����� ���� ����ȴ�.
	//build����(���� ���� ����) create���߿�(���� ���� ����)
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

	//Height Texture Map���� ���� ���� ���� �����ϴ� �Լ�
	HRESULT loadHeightMap(std::wstring wszHeightMapName, std::wstring wszTexFlieName);

	//���� �븻�� ���ϴ� �Լ�
	void generateVTNLookupTable();

	//face�� �븻�� ���ϴ� �Լ� : 3������ �̿��� ���Ѵ�.
	CPM_Vector3 calcFaceNormal(UINT i0, UINT i1, UINT i2);
	void calcVertexNormal(UINT iVertex);

	//�ʿ��� �ش���ġ�� ���̰� ��������
	FLOAT getHeight(CPM_Vector3 vPos);
	FLOAT lerp(FLOAT opd1, FLOAT opd2, FLOAT fRatio);	//���̰� �����Լ�
};

//////////////////////////////////////////////////
//
// CPGCVertex.h
//		Vertex구조체와 Vertex Input Layout을 관리하는 헤더
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"
#include "CPRSBaseStruct.h"

#define CPRS_LAYOUT_SHADER_VERSION						CPGC_SHADER_LANG_VERSION

#define CPRS_VSLAYOUT_ENTRYPOINT						"VSLayout"
#define CPRS_VSLAYOUT_TARGET							"vs"

#define MAX_NUMBER_OF_SKINNING_MATRIX 255

struct CPRS_LineVertex
{
	CPM_Vector3 p;
	CPM_Vector4 c;
};

struct CPRS_BaseVertex
{
	CPM_Vector3 p;
	CPM_Vector3 n;
	CPM_Vector4 c;
	CPM_Vector2 t;
};

struct CPRS_SkinningVertex
{
	CPRS_BaseVertex		v;
	CPM_Vector4			i;
	CPM_Vector4			w;
};

struct CPRS_ParticleVertex
{
	CPM_Vector3 p;
	CPM_Vector3 n;
	CPM_Vector4 c;
	CPM_Vector2 t;
	CPM_Vector4 spriteRect;
	CPM_Matrix	rot;
	CPM_Vector3 scale;
};

struct CPRS_BaseCBuf
{
	CPM_Matrix			WVPMat;
	CPM_Vector4			vLight;
	FLOAT				timer;
	FLOAT				y;
	FLOAT				z;
	FLOAT				w;
};

struct CPRS_CBuf_SkinningMat
{
	CPM_Matrix	matList[MAX_NUMBER_OF_SKINNING_MATRIX];
};

class CPGCVertexLayoutManager
{
private:
	ID3D11Device*															m_pDevice;

	std::map<std::wstring, ID3D11InputLayout*>								m_VertexLayoutMap;
	std::set<std::wstring>													m_vsFilenameSet;

	CPGCVertexLayoutManager();
	~CPGCVertexLayoutManager();

public:
	CPGCVertexLayoutManager(const CPGCVertexLayoutManager& other) = delete;
	CPGCVertexLayoutManager& operator=(const CPGCVertexLayoutManager& other) = delete;

public:
	bool					init(ID3D11Device* pDevice);
	bool					release();

	HRESULT					createVLayout(std::wstring wszLayoutName, D3D11_INPUT_ELEMENT_DESC* iedArr, UINT iCnt, std::wstring wszVSFilename);

	ID3D11InputLayout*		getPtr(std::wstring wszLayoutName);

	//전역 인스턴스 반환
	static CPGCVertexLayoutManager& getInstance()
	{
		static CPGCVertexLayoutManager singleInst;
		return singleInst;
	}
};

#define CPGC_VLAYOUT_MGR CPGCVertexLayoutManager::getInstance()

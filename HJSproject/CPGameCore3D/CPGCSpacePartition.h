#pragma once
#include "CPGCMap.h"
#include "CPGCCamera.h"

class CPGCNode
{
public:
	//순서는 LT, RT, RB, LB
	// LT RT
	// LB RB
	std::vector<UINT>	m_cornerV;		//코너 정점 정보
	CPRS_BOUNDING_BOX	m_boundingBox;	//노드의 바운딩 박스

	//노드의 부모 자식 정보
	CPGCNode* m_pParent;
	std::vector<CPGCNode*> m_pChild;

	//노드의 깊이와 리프 여부
	UINT m_iDepth;
	BOOL m_bLeaf;
	UINT m_NodeIdx;

	//오브젝트 리스트
	//std::vector<CPGCObject*> m_staticObjList;
	//std::vector<CPGCObject*> m_dynamicObjList;

	//노드의 인덱스 리스트와 인덱스 버퍼
	std::vector<UINT> m_idxList;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuf;

	CPGCNode();
	~CPGCNode();

	CPGCNode(UINT LT, UINT RT, UINT RB, UINT LB,
		UINT iDepth, CPGCNode* m_pParent = nullptr);

	void set(UINT LT, UINT RT, UINT RB, UINT LB,
		UINT iDepth, CPGCNode* m_pParent = nullptr);

	void setIndices(CPGCMap* pMap);

	void updateBoundingBox(CPGCMap* pMap);
};



class CPGCQuadTree
{
public:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDContext;

	CPGCNode*				m_pRoot;
	CPGCMap*				m_pMap;
	UINT					m_iMaxDepth;
	CPGCCamera*				m_pCam;

	std::vector<CPGCNode*>		m_leafNodeList;
	std::vector<CPGCNode*>		m_drawLeafNodeList;	//매 프레임마다 갱신

public:
	CPGCQuadTree();
	~CPGCQuadTree();

	void create(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDContext, 
		CPGCMap* pMap, 
		UINT iMaxDepth = 3);

	bool buildTree(CPGCNode* pTarget);

	bool isAbleDivide(CPGCNode* pTarget);

	void clear();
	void clearSub(CPGCNode* pTarget);

	HRESULT createIndexBuffer(CPGCMap* pMap, CPGCNode* pTarget);

	//그릴 리프노드 판별 함수
	void checkNodeToDraw(CPGCNode* pTarget);

	bool update();
	bool preRender();
	bool render();

	/*CPGCNode* addObject(MyObject3D* pObj);
	CPGCNode* addObjectSub(CPGCNode* pTarget, MyObject3D* pObj);*/

	/*
	void addStaticObject(MyObject3D* pBObj);
	void addDynamicObject(MyObject3D* pBObj);
	void resetAllDynamicList();
	void getCollisionList(const MyObject3D* pDObj, std::vector<MyObject3D*>& objList);
	void getCollisionListSub(const MyObject3D* pDObj, const CPGCNode* pTarget,
		std::vector<MyObject3D*>& objList);


	void resetSubTreeDList(CPGCNode* pTarget);
	*/
};

#pragma once
#include "CPGCMap.h"
#include "CPGCCamera.h"

class CPGCNode
{
public:
	//������ LT, RT, RB, LB
	// LT RT
	// LB RB
	std::vector<UINT>	m_cornerV;		//�ڳ� ���� ����
	CPRS_BOUNDING_BOX	m_boundingBox;	//����� �ٿ�� �ڽ�

	//����� �θ� �ڽ� ����
	CPGCNode* m_pParent;
	std::vector<CPGCNode*> m_pChild;

	//����� ���̿� ���� ����
	UINT m_iDepth;
	BOOL m_bLeaf;
	UINT m_NodeIdx;

	//������Ʈ ����Ʈ
	//std::vector<CPGCObject*> m_staticObjList;
	//std::vector<CPGCObject*> m_dynamicObjList;

	//����� �ε��� ����Ʈ�� �ε��� ����
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
	std::vector<CPGCNode*>		m_drawLeafNodeList;	//�� �����Ӹ��� ����

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

	//�׸� ������� �Ǻ� �Լ�
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

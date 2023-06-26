#include "CPGCSpacePartition.h"

CPGCNode::CPGCNode()
{
	m_bLeaf = FALSE;
	set(0, 0, 0, 0, 0, nullptr);
	m_NodeIdx = 0;
}

CPGCNode::~CPGCNode()
{
}

CPGCNode::CPGCNode(UINT LT, UINT RT, UINT RB, UINT LB,
	UINT iDepth, CPGCNode* pParent)
{
	set(LT, RT, RB, LB, iDepth, pParent);
}

void CPGCNode::set(UINT LT, UINT RT, UINT RB, UINT LB,
	UINT iDepth, CPGCNode* pParent)
{
	m_cornerV.resize(4);

	m_cornerV[0] = LT;
	m_cornerV[1] = RT;
	m_cornerV[2] = RB;
	m_cornerV[3] = LB;

	m_iDepth = iDepth;
	m_pParent = pParent;
}

void CPGCNode::setIndices(CPGCMap* pMap)
{
	//코너 정점 정보를 이용해 인덱스 리스트를 구성한다.

	// LT ... RT
	//
	// LB ... RB

	UINT nRowCell = m_cornerV[1] - m_cornerV[0];	//RT - LT : 가로 정점 개수 -1
	UINT nColCell = (m_cornerV[3] - m_cornerV[0]) / pMap->m_iWidth; //(LB - LT) / nColNum : 세로 정점 개수 - 1

	UINT nIdxSize = (nRowCell * nColCell) * 2 * 3;

	m_idxList.resize(nIdxSize);
	int cnt = 0;

	//인덱스 리스트 & 바운딩 박스 생성부
	//1. AABB
	m_boundingBox.m_vMin.x = pMap->m_vertices[m_cornerV[0]].p.x;
	m_boundingBox.m_vMin.z = pMap->m_vertices[m_cornerV[2]].p.z;
	m_boundingBox.m_vMax.x = pMap->m_vertices[m_cornerV[2]].p.x;
	m_boundingBox.m_vMax.z = pMap->m_vertices[m_cornerV[0]].p.z;

	m_boundingBox.m_vMin.y = +INFINITY;
	m_boundingBox.m_vMax.y = -INFINITY;

	//정점 개수만큼 돌아 MaxY를 찾는다.
	for (UINT i = 0; i < nColCell + 1; i++)
	{
		for (UINT j = 0; j < nRowCell + 1; j++)
		{
			m_boundingBox.m_vMin.y = min(pMap->m_vertices[m_cornerV[0] + i * pMap->m_iWidth + j].p.y, m_boundingBox.m_vMin.y);
			m_boundingBox.m_vMax.y = max(pMap->m_vertices[m_cornerV[0] + i * pMap->m_iWidth + j].p.y, m_boundingBox.m_vMax.y);

			if (i != nColCell && j != nRowCell)
			{
				UINT LT = m_cornerV[0] + i * pMap->m_iWidth + j;
				UINT RT = LT + 1;
				UINT RB = RT + pMap->m_iWidth;
				UINT LB = LT + pMap->m_iWidth;

				m_idxList[cnt + 0] = LT;
				m_idxList[cnt + 1] = RT;
				m_idxList[cnt + 2] = LB;

				m_idxList[cnt + 3] = LB;
				m_idxList[cnt + 4] = RT;
				m_idxList[cnt + 5] = RB;

				cnt += 6;
			}
		}
	}

	//3.OBB
	m_boundingBox.m_vCPos = (m_boundingBox.m_vMin + m_boundingBox.m_vMax) * 0.5f;
	m_boundingBox.m_vAxis[0] = { 1.0f, 0.0f, 0.0f };
	m_boundingBox.m_vAxis[1] = { 0.0f, 1.0f, 0.0f };
	m_boundingBox.m_vAxis[2] = { 0.0f, 0.0f, 1.0f };
	m_boundingBox.m_vExt.x = m_boundingBox.m_vMax.x - m_boundingBox.m_vCPos.x;
	m_boundingBox.m_vExt.y = m_boundingBox.m_vMax.y - m_boundingBox.m_vCPos.y;
	m_boundingBox.m_vExt.z = m_boundingBox.m_vMax.z - m_boundingBox.m_vCPos.z;
}

void CPGCNode::updateBoundingBox(CPGCMap* pMap)
{
	//코너 정점 정보를 이용해 바운딩 박스 갱신 범위를 정한다.

	// LT ... RT
	//
	// LB ... RB

	UINT nRowCell = m_cornerV[1] - m_cornerV[0];	//RT - LT : 가로 정점 개수 -1
	UINT nColCell = (m_cornerV[3] - m_cornerV[0]) / pMap->m_iWidth; //(LB - LT) / nColNum : 세로 정점 개수 - 1

	//인덱스 리스트 & 바운딩 박스 생성부
	m_boundingBox.m_vMin.x = pMap->m_vertices[m_cornerV[0]].p.x;
	m_boundingBox.m_vMin.z = pMap->m_vertices[m_cornerV[2]].p.z;
	m_boundingBox.m_vMax.x = pMap->m_vertices[m_cornerV[2]].p.x;
	m_boundingBox.m_vMax.z = pMap->m_vertices[m_cornerV[0]].p.z;

	m_boundingBox.m_vMin.y = +INFINITY;
	m_boundingBox.m_vMax.y = -INFINITY;

	//정점 개수만큼 돌아 MaxY를 찾는다.
	for (UINT i = 0; i < nColCell + 1; i++)
	{
		for (UINT j = 0; j < nRowCell + 1; j++)
		{
			m_boundingBox.m_vMin.y = min(pMap->m_vertices[m_cornerV[0] + i * pMap->m_iWidth + j].p.y, m_boundingBox.m_vMin.y);
			m_boundingBox.m_vMax.y = max(pMap->m_vertices[m_cornerV[0] + i * pMap->m_iWidth + j].p.y, m_boundingBox.m_vMax.y);
		}
	}

	//3.OBB
	m_boundingBox.m_vCPos = (m_boundingBox.m_vMin + m_boundingBox.m_vMax) * 0.5f;
	m_boundingBox.m_vAxis[0] = { 1.0f, 0.0f, 0.0f };
	m_boundingBox.m_vAxis[1] = { 0.0f, 1.0f, 0.0f };
	m_boundingBox.m_vAxis[2] = { 0.0f, 0.0f, 1.0f };
	m_boundingBox.m_vExt.x = m_boundingBox.m_vMax.x - m_boundingBox.m_vCPos.x;
	m_boundingBox.m_vExt.y = m_boundingBox.m_vMax.y - m_boundingBox.m_vCPos.y;
	m_boundingBox.m_vExt.z = m_boundingBox.m_vMax.z - m_boundingBox.m_vCPos.z;
}

CPGCQuadTree::CPGCQuadTree()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;

	m_pRoot = nullptr;
	m_pMap = nullptr;
	m_iMaxDepth = 0;
}

CPGCQuadTree::~CPGCQuadTree()
{
	clear();
}

void CPGCQuadTree::create(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext, CPGCMap* pMap, UINT iMaxDepth)
{
	if (!m_pRoot)
	{
		m_pDevice = pDevice;
		m_pDContext = pDContext;
		m_iMaxDepth = iMaxDepth;
		m_pMap = pMap;

		CPGCNode* newE = new CPGCNode(0, pMap->m_iWidth - 1, pMap->m_iWidth * pMap->m_iHeight - 1, pMap->m_iWidth * (pMap->m_iHeight - 1), 0);
		assert(newE);

		m_pRoot = newE;

		buildTree(m_pRoot);
	}
}

bool CPGCQuadTree::buildTree(CPGCNode* pTarget)
{
	if (pTarget)
	{
		if (isAbleDivide(pTarget))
		{
			//m_cornerV => LT : 0 / RT : 1 / RB : 2 / LB : 3
			UINT LT = pTarget->m_cornerV[0];
			UINT RT = pTarget->m_cornerV[1];
			UINT RB = pTarget->m_cornerV[2];
			UINT LB = pTarget->m_cornerV[3];
			UINT LC = (LT + LB) / 2;
			UINT RC = (RT + RB) / 2;
			UINT TC = (LT + RT) / 2;
			UINT BC = (LB + RB) / 2;
			UINT center = (LT + LB + RT + RB) / 4;

			pTarget->m_pChild.resize(4);

			//자식 노드의 사분면 순서
			// 0  1
			// 2  3
			pTarget->m_pChild[0] = new CPGCNode(LT, TC, center, LC, pTarget->m_iDepth + 1, pTarget);
			pTarget->m_pChild[1] = new CPGCNode(TC, RT, RC, center, pTarget->m_iDepth + 1, pTarget);
			pTarget->m_pChild[2] = new CPGCNode(LC, center, BC, LB, pTarget->m_iDepth + 1, pTarget);
			pTarget->m_pChild[3] = new CPGCNode(center, RC, RB, BC, pTarget->m_iDepth + 1, pTarget);

			for (int i = 0; i < pTarget->m_pChild.size(); i++)
			{
				if (!buildTree(pTarget->m_pChild[i])) { return false; }
			}
		}
		else
		{
			pTarget->m_bLeaf = TRUE;
			pTarget->m_NodeIdx = m_leafNodeList.size();
			m_leafNodeList.push_back(pTarget);
			createIndexBuffer(m_pMap, pTarget);
		}
		
		return true;
	}

	return false;
}

bool CPGCQuadTree::isAbleDivide(CPGCNode* pTarget)
{
	if (pTarget->m_cornerV[1] - pTarget->m_cornerV[0] <= 1) { return false; }
	if (pTarget->m_iDepth >= m_iMaxDepth) { return false; }

	return true;
}

void CPGCQuadTree::clear()
{
	if (m_pRoot)
	{
		clearSub(m_pRoot);
		m_pRoot = nullptr;
	}
}

void CPGCQuadTree::clearSub(CPGCNode* pTarget)
{
	if (pTarget->m_pChild.size())
	{
		for (int i = 0; i < 4; i++) { clearSub(pTarget->m_pChild[i]); }
	}
	delete pTarget;
	pTarget = nullptr;
}

HRESULT CPGCQuadTree::createIndexBuffer(CPGCMap* pMap, CPGCNode* pTarget)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	pTarget->setIndices(pMap);

	bd.ByteWidth = pTarget->m_idxList.size() * sizeof(UINT);
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
	sd.pSysMem = &pTarget->m_idxList.at(0);

	hr = m_pDevice->CreateBuffer(&bd, &sd, pTarget->m_pIndexBuf.GetAddressOf());

	return hr;
}

void CPGCQuadTree::checkNodeToDraw(CPGCNode* pTarget)
{
	if (pTarget)
	{
		UINT temp = m_pCam->m_frustum.classifyBox(pTarget->m_boundingBox);

		if (temp == CPRS_FCULL_FRONT || temp == CPRS_FCULL_SPAN)
		{
			m_drawLeafNodeList.push_back(pTarget);
		}
	}
}

bool CPGCQuadTree::update()
{
	m_drawLeafNodeList.clear();

	for (auto it : m_leafNodeList)
	{
		checkNodeToDraw(it);
	}

	return true;
}

bool CPGCQuadTree::preRender()
{
	m_pMap->setMatrix(nullptr, m_pCam->getViewMatPtr(), m_pCam->getProjMatPtr());

	return true;
}

bool CPGCQuadTree::render()
{
	m_pMap->bindToPipeline();

	for (auto& it : m_drawLeafNodeList)
	{
		m_pDContext->IASetIndexBuffer(it->m_pIndexBuf.Get(), DXGI_FORMAT_R32_UINT, 0);

		m_pDContext->DrawIndexed(it->m_idxList.size(), 0, 0);
	}

	return true;
}


//CPGCNode* CPGCQuadTree::addObject(MyObject3D* pBObj)
//{
//	CPGCNode* pFind = nullptr;
//
//	if (m_pRoot && pBObj) { pFind = addObjectSub(m_pRoot, pBObj); }
//	return pFind;
//}
//
//CPGCNode* CPGCQuadTree::addObjectSub(CPGCNode* pTarget, MyObject3D* pBObj)
//{
//	CPGCNode* pFind = nullptr;
//	CPGCNode* pTemp = nullptr;
//
//	/*if (TestCollision::rectToRect(pTarget->nPos, pObj->posRect) == CT_IN)
//	{
//		pFind = pTarget;
//
//		if (pTarget->pChild.size())
//		{
//			for (int i = 0; i < 4; i++)
//			{
//				pTemp = addObjectSub(pTarget->pChild[i], pObj);
//				if (pTemp) { return pTemp; }
//			}
//		}
//	}*/
//
//	return pFind;
//}
//
//
//
//
//
//void CPGCQuadTree::addStaticObject(MyObject3D* pBObj)
//{
//	MyObject3D* pSObj = dynamic_cast<MyObject3D*>(pBObj);
//	if (pSObj)
//	{
//		CPGCNode* pFind = addObject(pSObj);
//		if (pFind) { pFind->staticObjList.push_back(pSObj); }
//	}
//}
//
//void CPGCQuadTree::addDynamicObject(MyObject3D* pBObj)
//{
//	MyObject3D* pDObj = dynamic_cast<MyObject3D*>(pBObj);
//	if (pDObj)
//	{
//		CPGCNode* pFind = addObject(pDObj);
//		if (pFind) { pFind->dynamicObjList.push_back(pDObj); }
//	}
//}
//
//void CPGCQuadTree::resetAllDynamicList()
//{
//	if (m_pRoot) { resetSubTreeDList(m_pRoot); }
//}
//
//void CPGCQuadTree::getCollisionList(const MyObject3D* pBObj, std::vector<MyObject3D*>& objList)
//{
//	const MyObject3D* pDObj = dynamic_cast<const MyObject3D*>(pBObj);
//
//	if (pDObj) { getCollisionListSub(pDObj, m_pRoot, objList); }
//}
//
//void CPGCQuadTree::getCollisionListSub(const MyObject3D* pDObj, const CPGCNode* pTarget,
//	std::vector<MyObject3D*>& objList)
//{
//	for (int i = 0; i < pTarget->staticObjList.size(); i++)
//	{
//		if (TestCollision::circleToCircle(pDObj->posCircle, pTarget->staticObjList[i]->posCircle))
//		{
//			if (TestCollision::rectToRect(pDObj->posRect, pTarget->staticObjList[i]->posRect))
//			{
//				objList.push_back(pTarget->staticObjList[i]);
//			}
//		}
//	}
//
//	for (int i = 0; i < pTarget->dynamicObjList.size(); i++)
//	{
//		if (TestCollision::circleToCircle(pDObj->posCircle, pTarget->dynamicObjList[i]->posCircle))
//		{
//			if (TestCollision::rectToRect(pDObj->posRect, pTarget->dynamicObjList[i]->posRect))
//			{
//				objList.push_back(pTarget->dynamicObjList[i]);
//			}
//		}
//	}
//
//	for (int i = 0; i < pTarget->pChild.size(); i++)
//	{
//		if (TestCollision::rectToRect(pTarget->pChild[i]->nPos, pDObj->posRect))
//		{
//			getCollisionListSub(pDObj, pTarget->pChild[i], objList);
//		}
//	}
//}
//
//void CPGCQuadTree::resetSubTreeDList(CPGCNode* pTarget)
//{
//	pTarget->dynamicObjList.clear();
//
//	if (pTarget->pChild.size())
//	{
//		for (int i = 0; i < 4; i++) { resetSubTreeDList(pTarget->pChild[i]); }
//	}
//}

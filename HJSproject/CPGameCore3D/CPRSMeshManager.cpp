#include "CPRSMeshManager.h"

HRESULT createDXSimpleBuf(ID3D11Device* pDevice,
	size_t iBufSize,
	void* pResource,
	D3D11_BIND_FLAG bindType,
	ID3D11Buffer** ppOut)
{
	HRESULT hr = S_OK;

	if (!pDevice || !pResource || !ppOut) { hr = E_POINTER; return hr; }

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = iBufSize;
	bd.Usage = D3D11_USAGE_DYNAMIC;						//GPU는 읽기전용, CPU는 쓰기 전용으로 액세스 지정
	bd.BindFlags = bindType;							//정점 버퍼로 사용 - DX Map함수 사용을 위한 설정
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			//CPU 쓰기 가능 - DX Map함수 사용을 위한 설정

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
	sd.pSysMem = pResource;

	hr = pDevice->CreateBuffer(&bd, &sd, ppOut);

	return hr;
}

HRESULT createDXBuf(ID3D11Device* pDevice,
	D3D11_BUFFER_DESC* pBd,
	D3D11_SUBRESOURCE_DATA* pSd,
	ID3D11Buffer** ppOut)
{
	HRESULT hr = S_OK;

	if (!pDevice || !pBd || !pSd || !ppOut) { hr = E_POINTER; return hr; }
	hr = pDevice->CreateBuffer(pBd, pSd, ppOut);

	return hr;
}

CPRS_Mesh::CPRS_Mesh()
{
	m_iVertexCount = 0;
	m_iIndexCount = 0;
	m_iStride = 0;
	m_iOffset = 0;
	m_primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pVLayout = nullptr;
}

CPRS_Mesh::~CPRS_Mesh()
{
}

bool CPRS_Mesh::init()
{
	return true;
}

bool CPRS_Mesh::release()
{
	m_pVLayout = nullptr;

	return true;
}

void CPRS_Mesh::bindToPipeline(ID3D11DeviceContext* pDContext)
{
	if (pDContext)
	{
		pDContext->IASetVertexBuffers(0, 1, m_pVBuf.GetAddressOf(), &m_iStride, &m_iOffset);

		//인덱스 버퍼가 바인딩되지 않으면 작동하지 않는다.
		pDContext->IASetIndexBuffer(m_pIBuf.Get(), DXGI_FORMAT_R32_UINT, 0);

		pDContext->IASetInputLayout(m_pVLayout);
		pDContext->IASetPrimitiveTopology(m_primitive);
	}
}

CPGCMeshManager::CPGCMeshManager()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;
}

CPGCMeshManager::~CPGCMeshManager()
{
}

HRESULT CPGCMeshManager::createMeshBuf(CPRS_Mesh* pMesh, UINT iVFormatSize)
{
	HRESULT hr = S_OK;

	//정점 버퍼 생성
	hr = createVBuf(pMesh, iVFormatSize);
	if (FAILED(hr)) { return hr; }

	//인덱스 버퍼 생성
	if (pMesh->m_indices.size())
	{
		hr = createIBuf(pMesh);
		if (FAILED(hr)) { return hr; }
	}

	return hr;
}

HRESULT CPGCMeshManager::createVBuf(CPRS_Mesh* pMesh, UINT iFormatSize)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = pMesh->m_vertices.size() * iFormatSize;
	bd.Usage = D3D11_USAGE_DYNAMIC;						//GPU는 읽기전용, CPU는 쓰기 전용으로 액세스 지정
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//정점 버퍼로 사용 - DX Map함수 사용을 위한 설정
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			//CPU 쓰기 가능 - DX Map함수 사용을 위한 설정

	pMesh->m_iStride = iFormatSize;
	pMesh->m_iOffset = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));

	sd.pSysMem = pMesh->m_vertices.data();

	hr = m_pDevice->CreateBuffer(&bd, &sd, pMesh->m_pVBuf.GetAddressOf());

	return hr;
}

HRESULT CPGCMeshManager::createIBuf(CPRS_Mesh* pMesh)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = pMesh->m_indices.size() * sizeof(UINT);
	bd.Usage = D3D11_USAGE_DYNAMIC;						//GPU는 읽기전용, CPU는 쓰기 전용으로 액세스 지정
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;			//정점 버퍼로 사용 - DX Map함수 사용을 위한 설정
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			//CPU 쓰기 가능 - DX Map함수 사용을 위한 설정

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));

	sd.pSysMem = pMesh->m_indices.data();

	hr = m_pDevice->CreateBuffer(&bd, &sd, pMesh->m_pIBuf.GetAddressOf());

	return hr;
}

bool CPGCMeshManager::setVertices(CPRS_BaseVertex* vertexArr, UINT iCnt, CPRS_Mesh* pMesh)
{
	if (!vertexArr || !iCnt) { return false; }

	pMesh->m_vertices.clear();

	//resize는 원소의 개수를 받음을 유의하자
	pMesh->m_vertices.resize(iCnt);

	//배열에서도 Last는 마지막 원소의 다음 원소임을 유의하자
	std::copy(vertexArr, vertexArr + iCnt, pMesh->m_vertices.begin());

	pMesh->m_iVertexCount = iCnt;

	return true;
}

bool CPGCMeshManager::setIndices(UINT* indexArr, UINT iCnt, CPRS_Mesh* pMesh)
{
	if (!indexArr || !iCnt) { return false; }

	pMesh->m_indices.clear();

	//resize는 원소의 개수를 받음을 유의하자
	pMesh->m_indices.resize(iCnt);

	//배열에서도 Last는 마지막 원소의 다음 원소임을 유의하자
	std::copy(indexArr, indexArr + iCnt, pMesh->m_indices.begin());

	pMesh->m_iIndexCount = iCnt;

	return true;
}

bool CPGCMeshManager::init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	if (!pDevice || !pDContext) { return false; }
	else
	{
		m_pDevice = pDevice;
		m_pDContext = pDContext;
	}
	createDefaultMeshPreset();

	return true;
}

bool CPGCMeshManager::release()
{
	for (auto it : m_meshMap)
	{
		it.second->release();
		delete it.second;
		it.second = nullptr;
	}

	m_scriptFilenameSet.clear();
	m_fbxFilenameMap.clear();

	m_pDevice = nullptr;
	m_pDContext = nullptr;

	return true;
}

HRESULT CPGCMeshManager::createMesh(std::wstring wszMeshName,
	CPRS_BaseVertex* vertexArr,
	UINT iVCnt,
	UINT* indexArr,
	UINT iICnt,
	std::wstring wszLayoutName,
	D3D11_PRIMITIVE_TOPOLOGY primitive)
{
	HRESULT hr = S_OK;

	if (m_meshMap.find(wszMeshName) == m_meshMap.end())
	{
		CPRS_Mesh* newE = nullptr;

		newE = new CPRS_Mesh;

		bool bRet = true;

		//정점 세팅
		bRet = setVertices(vertexArr, iVCnt, newE);
		if (!bRet) { newE->release(); delete newE; newE = nullptr; return E_FAIL; }

		//인덱스 세팅
		bRet = setIndices(indexArr, iICnt, newE);

		//버퍼 생성
		hr = createMeshBuf(newE, sizeof(CPRS_BaseVertex));
		if (FAILED(hr))
		{
			newE->release();
			delete newE;
			newE = nullptr;
			printCreateErr(hr);
			return hr;
		}

		//레이아웃 세팅
		newE->m_pVLayout = CPGC_VLAYOUT_MGR.getPtr(wszLayoutName);
		if (!newE->m_pVLayout)
		{
			newE->release();
			delete newE;
			newE = nullptr;
			hr = E_FAIL;
			return hr;
		}

		newE->m_primitive = primitive;
		newE->m_wszName = wszMeshName;

		m_meshMap.insert(std::make_pair(wszMeshName, newE));
	}
	else { hr = S_FALSE; }

	return hr;
}

void CPGCMeshManager::createDefaultMeshPreset()
{
	HRESULT hr = S_OK;

	UINT iVertexCount = 0;
	UINT iIdxCount = 0;

	//0. Rect
	CPRS_BaseVertex BaseRectV[] =
	{
		{ {-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },
		{ {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
		{ {1.0f, -1.0f, -0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
		{ {-1.0f, -1.0f, -0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} }
	};
	iVertexCount = CPGC_ARRAY_ELEM_NUM(BaseRectV);

	UINT BaseRectI[] = { 0, 1, 2, 0, 2, 3 };
	iIdxCount = CPGC_ARRAY_ELEM_NUM(BaseRectI);

	hr = createMesh(L"CPRS_SM_RECT",
		BaseRectV, iVertexCount,
		BaseRectI, iIdxCount,
		L"VL_PNCT");

	//1. BG Plane
	CPRS_BaseVertex PlaneV[] =
	{
		{ {-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },
		{ {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
		{ {1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
		{ {-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} }
	};
	iVertexCount = CPGC_ARRAY_ELEM_NUM(PlaneV);

	UINT PlaneI[] = { 0, 1, 2, 0, 2, 3 };
	iIdxCount = CPGC_ARRAY_ELEM_NUM(PlaneI);

	hr = createMesh(L"CPRS_SM_PLANE",
		PlaneV, iVertexCount,
		PlaneI, iIdxCount,
		L"VL_PNCT");

	//2. Texture Cube
	CPRS_BaseVertex TexCubeV[24];
	iVertexCount = CPGC_ARRAY_ELEM_NUM(TexCubeV);
	for (int i = 0; i < 24; i++)
	{
		TexCubeV[i].n = { 1.0f, 1.0f, 1.0f };
		TexCubeV[i].p = TexCubeV[i].n;

		TexCubeV[i].c = { (FLOAT)(((i / 16) + 1) % 2), (FLOAT)(((i / 8) + 1) % 2), (FLOAT)(((i / 4) + 1) % 2), 1.0f };
	}

	for (int i = 0; i < 6; i++)
	{
		TexCubeV[i * 4].t = { 0.0f, 0.0f };
		TexCubeV[i * 4 + 1].t = { 1.0f, 0.0f };
		TexCubeV[i * 4 + 2].t = { 1.0f, 1.0f };
		TexCubeV[i * 4 + 3].t = { 0.0f, 1.0f };
	}

	//pt[0] = LTF
	TexCubeV[3].p = TexCubeV[4].p = TexCubeV[17].p = { -1.0f, 1.0f, -1.0f };

	//pt[1] = LTB
	TexCubeV[0].p = TexCubeV[13].p = TexCubeV[16].p = { -1.0f, 1.0f, 1.0f };

	//pt[2] = RTB
	TexCubeV[1].p = TexCubeV[9].p = TexCubeV[12].p = { 1.0f, 1.0f, 1.0f };

	//pt[3] = RTF
	TexCubeV[2].p = TexCubeV[5].p = TexCubeV[8].p = { 1.0f, 1.0f, -1.0f };

	//pt[4] = LBF
	TexCubeV[7].p = TexCubeV[18].p = TexCubeV[20].p = { -1.0f, -1.0f, -1.0f };

	//pt[5] = LBB
	TexCubeV[14].p = TexCubeV[19].p = TexCubeV[23].p = { -1.0f, -1.0f, 1.0f };

	//pt[6] = RBB
	TexCubeV[10].p = TexCubeV[15].p = TexCubeV[22].p = { 1.0f, -1.0f, 1.0f };

	//pt[7] = RBF
	TexCubeV[6].p = TexCubeV[11].p = TexCubeV[21].p = { 1.0f, -1.0f, -1.0f };

	UINT TexCubeI[] =
	{
		//top - 0 1 2 3
		0, 1, 3,
		3, 1, 2,
		//front - 4 5 6 7
		4, 5, 7,
		7, 5, 6,
		//right - 8 9 10 11
		8, 9, 11,
		11, 9, 10,
		//back - 12 13 14 15
		12, 13, 15,
		15, 13, 14,
		//left - 16 17 18 19
		16, 17, 19,
		19, 17, 18,
		//bottom - 20 21 22 23
		20, 21, 23,
		23, 21, 22
	};

	iIdxCount = CPGC_ARRAY_ELEM_NUM(TexCubeI);

	hr = createMesh(L"CPRS_SM_TEXCUBE",
		TexCubeV, iVertexCount,
		TexCubeI, iIdxCount,
		L"VL_PNCT");

	//3. Line Cube
	CPM_Color vColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	CPM_Vector3 Axis[3] =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};

	CPRS_BaseVertex LineCubeV[] =
	{
		//0 LBB -> 1 LBF -> 2 RBF -> 3 RBB // 아랫면
		{{ - Axis[0] - Axis[1] + Axis[2] }, CPM_Vector3(), vColor, CPM_Vector2() },	//LBB
		{{ - Axis[0] - Axis[1] - Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//LBF
		{{ + Axis[0] - Axis[1] - Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//RBF
		{{ + Axis[0] - Axis[1] + Axis[2] }, CPM_Vector3(), vColor, CPM_Vector2() },	//RBB

		//0 LBB -> 4 LTB -> 7 RTB -> 3 RBB // 뒷면
		{{ - Axis[0] - Axis[1] + Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//LBB
		{{ - Axis[0] + Axis[1] + Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//LTB
		{{ + Axis[0] + Axis[1] + Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//RTB
		{{ + Axis[0] - Axis[1] + Axis[2] }, CPM_Vector3(), vColor, CPM_Vector2() },	//RBB

		//7 RTB -> 6 RTF -> 5 LTF -> 4 LTB // 윗면 
		{{ + Axis[0] + Axis[1] + Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//RTB
		{{ + Axis[0] + Axis[1] - Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//RTF
		{{ - Axis[0] + Axis[1] - Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//LTF
		{{ - Axis[0] + Axis[1] + Axis[2] }, CPM_Vector3(), vColor, CPM_Vector2() },	//LTB

		//5 LTF -> 1 LBF -> 2 RBF -> 6 RTF // 앞면
		{{ - Axis[0] + Axis[1] - Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//LTF	
		{{ - Axis[0] - Axis[1] - Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//LBF
		{{ + Axis[0] - Axis[1] - Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//RBF
		{{ + Axis[0] + Axis[1] - Axis[2] },	CPM_Vector3(), vColor, CPM_Vector2() },	//RTF
	};

	iVertexCount = CPGC_ARRAY_ELEM_NUM(LineCubeV);
	iIdxCount = 0;

	hr = createMesh(L"CPRS_SM_LINEBOX",
		LineCubeV, iVertexCount,
		nullptr, iIdxCount,
		L"VL_PNCT",
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//4. Line Sphere
	vColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	CPM_Vector3 unit = { 1.0f, 0.0f, 0.0f };

	CPRS_BaseVertex verticesX[MAX_NUMBER_OF_CIRCLE_VERTEX];
	CPRS_BaseVertex verticesZ[MAX_NUMBER_OF_CIRCLE_VERTEX];

	float rotAngle = CPGC_TAU / (float)MAX_NUMBER_OF_CIRCLE_VERTEX;

	CPM_Matrix rotX = DirectX::XMMatrixRotationRollPitchYaw(rotAngle, 0.0f, 0.0f);
	CPM_Matrix rotZ = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rotAngle);

	verticesX[0] = { { unit.x, unit.y, unit.z }, CPM_Vector3(), vColor, CPM_Vector2() };
	verticesZ[0] = { { unit.x, unit.y, unit.z }, CPM_Vector3(), vColor, CPM_Vector2() };

	for (int i = 1; i < MAX_NUMBER_OF_CIRCLE_VERTEX; i++)
	{
		CPM_Vector3 P0 = CPM_Vector3::Transform(verticesX[i - 1].p, rotX);
		CPM_Vector3 P2 = CPM_Vector3::Transform(verticesZ[i - 1].p, rotZ);

		verticesX[i] = { { P0.x, P0.y, P0.z }, CPM_Vector3(), vColor, CPM_Vector2() };
		verticesZ[i] = { { P2.x, P2.y, P2.z }, CPM_Vector3(), vColor, CPM_Vector2() };
	}

	std::vector<CPRS_BaseVertex> LineSphereV;

	for (auto& it : verticesX) { LineSphereV.push_back(it); }
	LineSphereV.push_back(verticesX[0]);
	for (auto& it : verticesZ) { LineSphereV.push_back(it); }
	LineSphereV.push_back(verticesX[0]);

	iVertexCount = LineSphereV.size();
	iIdxCount = 0;

	hr = createMesh(L"CPRS_SM_LINESPHERE",
		LineSphereV.data(), iVertexCount,
		nullptr, iIdxCount,
		L"VL_PNCT",
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

CPRS_Mesh* CPGCMeshManager::getPtr(std::wstring wszMeshName)
{
	auto it = m_meshMap.find(wszMeshName);
	if (it != m_meshMap.end()) { return it->second; }

	return nullptr;
}

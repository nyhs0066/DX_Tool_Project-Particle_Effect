#include "CPGCMap.h"

CPGCMap::CPGCMap()
{
	m_fCellSize = 16.0f;
	m_iRowTexTileNum = 1;
	m_fHeightScale = 1.0f;

	m_iWidth = 0;
	m_iHeight = 0;
	m_iVertexCount = 0;
	m_iIndexCount = 0;
	m_nFace = 0;

	m_iHeightMapWidth = 0;
	m_iHeightMapHeight = 0;

	m_pTexture = nullptr;
	m_pVLayout = nullptr;
}

CPGCMap::~CPGCMap()
{

}

bool CPGCMap::init()
{
	return true;
}

bool CPGCMap::update()
{
	return true;
}

bool CPGCMap::render()
{
	return true;
}

bool CPGCMap::release()
{
	m_pVBuf.ReleaseAndGetAddressOf();
	m_pIBuf.ReleaseAndGetAddressOf();
	m_pCBuf.ReleaseAndGetAddressOf();
	return true;
}

bool CPGCMap::build(CPM_Vector3 vCPos, int iCol, int iRow, float fCellSize)
{
	//맵의 정점 사각형은 다음과 같이 구성된다.
	//	v(0) - v(1) - v(2) - ... - v(C-1)
	//  v(C) - v(C+1) - v(C+2) - ... - v(2C-1)
	//					...
	//	v((R-1)*C) - v((R-1)*C+1) - ... - v(R*C-1)
	//
	// 각 사각형은 다음과 같이 구성된다.
	// v0 - v1
	//  | \ |
	// v3 - v2

	m_iWidth = iCol;
	m_iHeight = iRow;
	m_fCellSize = fCellSize;

	//정점 개수 / 면 개수
	m_iVertexCount = m_iWidth * m_iHeight;
	m_nFace = ((m_iWidth - 1) * (m_iHeight - 1)) * 2;
	m_iIndexCount = m_nFace * 3;

	//정점 - 인덱스 리스트 구성
	m_vertices.resize(m_iVertexCount);
	m_indices.resize(m_iIndexCount);
	m_vPos = vCPos;

	m_iRowTexTileNum = (m_iWidth - 1);

	//원점이 가운데에 있는 RECT와 같게 만들 것임
	int halfW = m_iWidth / 2;
	int halfH = m_iHeight / 2;

	//정점 초기 구성 : m_fHeightList가 있는 경우
	//만들고자 하는 맵의 사이즈가 지정한 Height맵의 해상도를 벗어날 수 있으므로 인덱스에 주의하자
	if (m_fHeightList.size())
	{
		UINT idxWidthOffset;
		UINT idxHeightOffset;
		float heightMapY;

		for (int row = 0; row < m_iHeight; row++)
		{
			for (int col = 0; col < m_iWidth; col++)
			{
				//col - halfW + m_vPos.x : 중앙 위치로부터 반을 -X방향으로 이동후 열의 위치만큼 이동
				//halfH - row + m_vPos.z : 좌상단을 기준으로 함을 유의하자.
				idxWidthOffset = (col * ((FLOAT)m_iHeightMapWidth / (FLOAT)m_iWidth));
				idxHeightOffset = (row * ((FLOAT)m_iHeightMapHeight / (FLOAT)m_iHeight));
				heightMapY = m_fHeightList[idxHeightOffset * m_iHeightMapWidth + idxWidthOffset];

				m_vertices[row * m_iWidth + col].p = { (FLOAT)(col - halfW + m_vPos.x) * m_fCellSize, heightMapY * m_fHeightScale, (FLOAT)(halfH - row + m_vPos.z) * m_fCellSize };
				m_vertices[row * m_iWidth + col].c = { 1.0f, 1.0f, 1.0f, 1.0f };
				m_vertices[row * m_iWidth + col].t = { m_iRowTexTileNum * (FLOAT)col / (FLOAT)(m_iWidth - 1), m_iRowTexTileNum * (FLOAT)row / (FLOAT)(m_iHeight - 1) };
			}
		}
	}
	else //사전 구성된 높이 값이 없는 경우
	{
		for (int row = 0; row < m_iHeight; row++)
		{
			for (int col = 0; col < m_iWidth; col++)
			{
				m_vertices[row * m_iWidth + col].p = { (FLOAT)(col - halfW + m_vPos.x) * m_fCellSize, 0.0f, (FLOAT)(halfH - row + m_vPos.z) * m_fCellSize };
				m_vertices[row * m_iWidth + col].c = { 1.0f, 1.0f, 1.0f, 1.0f };
				m_vertices[row * m_iWidth + col].t = { m_iRowTexTileNum * (FLOAT)col / (FLOAT)(m_iWidth - 1), m_iRowTexTileNum * (FLOAT)row / (FLOAT)(m_iHeight - 1) };
			}
		}
	}

	//인덱스 초기 구성
	int cnt = 0;
	for (int row = 0; row < m_iHeight - 1; row++)
	{
		for (int col = 0; col < m_iWidth - 1; col++)
		{
			UINT U = row * m_iWidth;
			UINT D = U + m_iWidth;
			UINT L = col;
			UINT R = col + 1;

			//UL - UR
			//   \ |
			//     DR
			m_indices[cnt++] = U + L;
			m_indices[cnt++] = U + R;
			m_indices[cnt++] = D + R;

			//UL
			// | \ 
			//DL - DR
			m_indices[cnt++] = U + L;
			m_indices[cnt++] = D + R;
			m_indices[cnt++] = D + L;
		}
	}

	//정점 노말을 만들기 위한 인덱스 리스트
	//m_idxList.assign(m_indices.begin(), m_indices.end());

	//정점의 인덱스 리스트가 만들어지면 각 정점에 대한 룩업테이블을 구성한다.
	generateVTNLookupTable();

	//룩업 테이블을 토대로 전체 지형에 대한 정점 노말을 계산한다.
	for (int iVertex = 0; iVertex < m_iVertexCount; iVertex++) { calcVertexNormal(iVertex); }

	return true;
}

HRESULT CPGCMap::create(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDContext,
	std::wstring wszTextureName,
	CPM_Vector3 vCPos,
	int iCol, int iRow, float fCellSize,
	std::wstring wszHeightMapName,
	std::wstring wszHeightMapFileName)
{
	HRESULT hr = S_OK;

	if (iCol == 0 || iRow == 0) { hr = E_FAIL; return hr; }

	if (!pDevice || !pDContext) { hr = E_POINTER; return hr; }
	else
	{
		m_pDevice = pDevice;
		m_pDContext = pDContext;
	}

	m_pTexture = CPGC_TEXTURE_MGR.getPtr(wszTextureName);
	if (!m_pTexture) { hr = E_POINTER; return hr; }

	loadHeightMap(wszHeightMapName, wszHeightMapFileName);

	build(vCPos, iCol, iRow, fCellSize);

	hr = createDXSimpleBuf(m_pDevice,
		sizeof(CPRS_BaseVertex) * m_vertices.size(),
		&m_vertices.at(0),
		D3D11_BIND_VERTEX_BUFFER,
		m_pVBuf.GetAddressOf());
	if (FAILED(hr)) { printCreateErr(hr); return hr; }

	hr = createDXSimpleBuf(m_pDevice,
		sizeof(UINT) * m_indices.size(),
		&m_indices.at(0),
		D3D11_BIND_INDEX_BUFFER,
		m_pIBuf.GetAddressOf());
	if (FAILED(hr)) { printCreateErr(hr); return hr; }

	hr = createDXSimpleBuf(m_pDevice,
		sizeof(CPRS_CBUF_COORDCONV_MATSET),
		&m_wvpMat,
		D3D11_BIND_CONSTANT_BUFFER,
		m_pCBuf.GetAddressOf());
	if (FAILED(hr)) { printCreateErr(hr); return hr; }

	m_pVLayout = CPGC_VLAYOUT_MGR.getPtr(L"VL_PNCT");
	if (!m_pVLayout) { hr = E_POINTER; return hr; }

	return hr;
}

void CPGCMap::setMatrix(const CPM_Matrix* pWorldM,
	const CPM_Matrix* pViewM,
	const CPM_Matrix* pProjM)
{
	if (pWorldM) { m_matWorld = *pWorldM; }
	if (pViewM) { m_matView = *pViewM; }
	if (pProjM) { m_matProj = *pProjM; }

	updateState();
	updateCoordConvMat(m_matWorld, m_matView, m_matProj);
}

void CPGCMap::updateState()
{
	//회전을 정규 기저 벡터의 회전값의 벡터로 표현하기에는 연산이 불필요하게 많다.
	//임의의 축에 대한 회전을 수행 또는 다른 회전 행렬의 복합적인 곱셈 이후에 
	//적용된 최종행렬에서 추출하는 것이 일반적이다.
	//이 함수는 복잡한 스케일 변화를 반영하지는 않는다.

	// 월드 행렬 R*T
	// Right.x	Right.y	Right.z	0
	// Up.x		Up.y	Up.z	0
	// Look.x	Look.y	Look.z	0
	// Pos.x	Pos.y	Pos.z	1

	//전치 이전의 행렬에서 추출한다.
	m_vPrevPos = m_vPos;
	m_vPos = { m_matWorld._41, m_matWorld._42, m_matWorld._43 };

	//obj Right-Axis
	m_sightAxis.vRight = { m_matWorld._11, m_matWorld._12, m_matWorld._13 };

	//obj Up-Axis
	m_sightAxis.vUp = { m_matWorld._21, m_matWorld._22, m_matWorld._23 };

	//obj At-Axis
	m_sightAxis.vLook = { m_matWorld._31, m_matWorld._32 , m_matWorld._33 };

	//회전 행렬에서 추출한 각 축벡터는 스케일 행렬에 의해 정규화되어있지 않은 상태라고 가정한다.
	m_sightAxis.vRight.Normalize();
	m_sightAxis.vUp.Normalize();
	m_sightAxis.vLook.Normalize();

	//바운딩 볼륨의 재계산을 수행하는 부분이 추가되어야 한다.
}

HRESULT CPGCMap::updateCoordConvMat(const CPM_Matrix& matWorld, const CPM_Matrix& matView, const CPM_Matrix& matProj)
{
	matWorld.Transpose(m_wvpMat.matTWorld);

	matWorld.Invert(m_wvpMat.matTInvWorld);
	m_wvpMat.matTInvWorld = m_wvpMat.matTInvWorld.Transpose();

	matView.Transpose(m_wvpMat.matTView);
	matProj.Transpose(m_wvpMat.matTProj);

	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedCbuf;
	ZeroMemory(&mappedCbuf, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = m_pDContext->Map(m_pCBuf.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		NULL,
		&mappedCbuf);

	if (SUCCEEDED(hr))
	{
		//받아온 GPU시작 주소에 업데이트 시킨 상수 버퍼 정보를 복사
		//부분적 수정도 가능
		memcpy(mappedCbuf.pData, &m_wvpMat, sizeof(CPRS_CBUF_COORDCONV_MATSET));

		//이후 Unmap
		m_pDContext->Unmap(m_pCBuf.Get(), 0);
	}

	return hr;
}

void CPGCMap::bindToPipeline()
{
	UINT strides = sizeof(CPRS_BaseVertex);
	UINT offset = 0;

	m_pDContext->IASetVertexBuffers(0, 1, m_pVBuf.GetAddressOf(), &strides, &offset);
	m_pDContext->IASetInputLayout(m_pVLayout);
	m_pDContext->IASetIndexBuffer(m_pIBuf.Get() , DXGI_FORMAT_R32_UINT, 0);

	m_pDContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDContext->VSSetConstantBuffers(0, 1, m_pCBuf.GetAddressOf());

	m_shaderGroup.bindToPipeline(m_pDContext);

	m_pDContext->PSSetSamplers(0, m_pSamplers.size(), m_pSamplers.data());

	if (m_pTexture) 
	{ 
		ID3D11ShaderResourceView* pTemp = m_pTexture->getSRV();
		m_pDContext->PSSetShaderResources(0, 1, &pTemp);
	}

	m_pDContext->OMSetBlendState(CPGC_DXSTATE_MGR.getBState(L"BS_ALPHABLEND"), NULL, -1);
	m_pDContext->OMSetDepthStencilState(CPGC_DXSTATE_MGR.getDSState(L"DSS_D_COMPLESS"), 0xff);
}

HRESULT CPGCMap::loadHeightMap(std::wstring wszHeightMapName, std::wstring wszTexFlieName)
{
	HRESULT hr = S_OK;

	//1. 높이맵 텍스처 불러오기
	CPRS_Texture* pHeightMapTexture = nullptr;

	CPGC_TEXTURE_MGR.createTextureEx(wszHeightMapName,
		wszTexFlieName,
		D3D11_USAGE_STAGING,
		(D3D11_CPU_ACCESS_FLAG)(D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE));

	pHeightMapTexture = CPGC_TEXTURE_MGR.getPtr(wszHeightMapName);

	if (!pHeightMapTexture) { return E_FAIL; }

	//2. 불러온 Texture에서 Height Map 정보를 읽어 저장
	D3D11_TEXTURE2D_DESC td = pHeightMapTexture->getTexInfo();

	UINT tW = td.Width;
	UINT tH = td.Height;

	m_iHeightMapWidth = tW;
	m_iHeightMapHeight = tH;
	m_fHeightList.resize(tW * tH);

	D3D11_MAPPED_SUBRESOURCE texSubRes;

	//가장 작업 속도가 느린 D3D11_MAP_READ옵션이다 CPU에서 직접 접근 하는 것이므로 반드시
	//텍스처 로딩시 D3D11_USAGE_STAGING으로 해주어야한다.
	if (SUCCEEDED(m_pDContext->Map(pHeightMapTexture->getTexBuf(), 0, D3D11_MAP_READ, 0, &texSubRes)))
	{
		//1바이트 크기의 텍셀을 받을 unsigned char형의 배열
		//파일 포맷에 따라 다르겠지만
		//명도만 반영하는 GrayScale텍스처 파일에서 뽑아온다. Red만 추출해도 되므로 1바이트 크기사용
		BYTE* texelArray = (BYTE*)texSubRes.pData;

		for (int row = 0; row < tH; row++)
		{
			//반드시 RowPitch크기가 행의 열 개수에 해당하므로 이 크기를 기준으로 작업할 것
			UINT rowAddr = row * texSubRes.RowPitch;

			for (int col = 0; col < tW; col++)
			{
				//RGBA는 각각 0~255사이의 값으로 구성되며 
				//이 중 Red만 추출해 0~1값을 갖도록 정규화 한다.
				//정규화하면 사용이 편리해진다.
				UINT colAddr = col * 4;
				m_fHeightList[row * tW + col] = texelArray[rowAddr + colAddr + 0] / 255.0f;
			}
		}

		m_pDContext->Unmap(pHeightMapTexture->getTexBuf(), 0);
	}

	return hr;
}

void CPGCMap::generateVTNLookupTable()
{
	//하나의 정점에 인접한 정점과 면은 최대 6개가 있을 수 있다.
	//	    v2 -  v3
	//	   /  \  /  \ 
	//	 v1  - v0  - v4  
	//	   \  /  \  /
	//	    v6  - v5

	//1. 정점의 노말을 구하기 위해서 정점의 인접 면정보를 들고있는 룩업테이블 만들기
	m_VTNLookupTable.resize(m_iVertexCount);
	for (int iFace = 0; iFace < m_nFace; iFace++)
	{
		UINT i0 = m_indices[iFace * 3 + 0];
		UINT i1 = m_indices[iFace * 3 + 1];
		UINT i2 = m_indices[iFace * 3 + 2];

		m_VTNLookupTable[i0].adjFaceIdxList.push_back(iFace);
		m_VTNLookupTable[i1].adjFaceIdxList.push_back(iFace);
		m_VTNLookupTable[i2].adjFaceIdxList.push_back(iFace);
	}
}

CPM_Vector3 CPGCMap::calcFaceNormal(UINT i0, UINT i1, UINT i2)
{
	//인덱스 리스트에서 3정점 = 1면 이므로 3개씩 끊어서 순차적으로 구한다.
	//인덱스에 해당하는 정점의 위치로부터 두개의 방향벡터를 만들고 외적한다.
	//외적 순서에 유의하자.
	//페이스당 정점의 순서는 다음과 같다.
	//i0 - i1
	//|  /  
	//i2

	CPM_Vector3 e0 = m_vertices[i1].p - m_vertices[i0].p;
	CPM_Vector3 e1 = m_vertices[i2].p - m_vertices[i0].p;

	CPM_Vector3 vFaceNormal = e0.Cross(e1);

	return vFaceNormal;
}

void CPGCMap::calcVertexNormal(UINT iVertex)
{
	//룩업 테이블에서 구성된 인접 면 리스트를 통해서 정점 노말을 계산한다.
	for (auto it : m_VTNLookupTable[iVertex].adjFaceIdxList)
	{
		UINT i0 = m_indices[it * 3 + 0];
		UINT i1 = m_indices[it * 3 + 1];
		UINT i2 = m_indices[it * 3 + 2];

		//인접 면의 노말을 계산하고 나서 합산한다.
		m_VTNLookupTable[iVertex].vNormal += calcFaceNormal(i0, i1, i2);
	}

	//합산한 노말을 정규화 하여 정점노말로 사용한다.
	m_VTNLookupTable[iVertex].vNormal.Normalize();
	m_vertices[iVertex].n = m_VTNLookupTable[iVertex].vNormal;
}

FLOAT CPGCMap::getHeight(CPM_Vector3 vPos)
{
	//1. 인수로 주어진 위치에 해당하는 맵의 셀의 행, 열 번호 찾기
	//각 셀의 크기와 중앙 위치에 따라 다르겠지만 중앙이 원점이라고 가정하면 다음과 같다.
	int nRowCellCnt = m_iWidth - 1;
	int nColCellCnt = m_iHeight - 1;

	//셀 번호를 화면 좌표계 처럼 계산하려면 z값을 부호 반전시킨다.
	FLOAT fRowLine = (-vPos.z + nColCellCnt * m_fCellSize / 2) / m_fCellSize;
	FLOAT fColLine = (vPos.x + nRowCellCnt * m_fCellSize / 2) / m_fCellSize;

	//크지 않은 가장 가까운 양의 정수가 셀 번호가 된다.
	//floor로 소수부를 잘라내는것이 좋지만 그냥 캐스팅해도 버려지므로 이를 이용한다.
	int iRowLine = min(fRowLine, nRowCellCnt - 1);
	iRowLine = max(iRowLine, 0);

	int iColLine = min(fColLine, nColCellCnt - 1);
	iColLine = max(iColLine, 0);

	//2. 찾은 셀의 행, 열번호를 이용해 셀에 해당하는 4정점의 높이 값을 지형의 정점에서 구한다.
	//0 - 1
	//| / |
	//3 - 2
	FLOAT vH[4];
	vH[0] = m_vertices[iRowLine * m_iWidth + iColLine].p.y;
	vH[1] = m_vertices[iRowLine * m_iWidth + iColLine + 1].p.y;
	vH[2] = m_vertices[(iRowLine + 1) * m_iWidth + iColLine + 1].p.y;
	vH[3] = m_vertices[(iRowLine + 1) * m_iWidth + iColLine].p.y;

	//3. 보간을 위한 기준 face를 찾는다.
	FLOAT res, t1, t2;

	FLOAT offZ = fRowLine - iRowLine;
	FLOAT offX = fColLine - iColLine;

	//X축의 슬로프 변화량으로 더해 보간한다.
	if (offX + offZ < 1.0f)
	{
		//왼쪽 상단 면 보간
		//0 -> 1 : offX
		t1 = lerp(0.0f, vH[1] - vH[0], offX);
		//0 -> 3 : offZ
		t2 = lerp(vH[0], vH[3], offZ);

		res = t2 + t1;
	}
	else
	{
		FLOAT invOffX = 1 - offX;
		FLOAT invOffZ = 1 - offZ;
		//오른쪽 하단 면 보간
		//2 -> 3 : invOffX
		t1 = lerp(0.0f, vH[3] - vH[2], invOffX);
		//2 -> 1 : invOffZ
		t2 = lerp(vH[2], vH[1], invOffZ);

		res = t2 + t1;
	}

	return res;
}

//X축의 슬로프 변화량으로 더해 보간한다.
FLOAT CPGCMap::lerp(FLOAT opd1, FLOAT opd2, FLOAT fRatio)
{
	//fRatio값이 0 ~ 1사이라고 가정한다.
	return opd2 * fRatio + opd1 * (1.0f - fRatio);
}

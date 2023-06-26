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
	//���� ���� �簢���� ������ ���� �����ȴ�.
	//	v(0) - v(1) - v(2) - ... - v(C-1)
	//  v(C) - v(C+1) - v(C+2) - ... - v(2C-1)
	//					...
	//	v((R-1)*C) - v((R-1)*C+1) - ... - v(R*C-1)
	//
	// �� �簢���� ������ ���� �����ȴ�.
	// v0 - v1
	//  | \ |
	// v3 - v2

	m_iWidth = iCol;
	m_iHeight = iRow;
	m_fCellSize = fCellSize;

	//���� ���� / �� ����
	m_iVertexCount = m_iWidth * m_iHeight;
	m_nFace = ((m_iWidth - 1) * (m_iHeight - 1)) * 2;
	m_iIndexCount = m_nFace * 3;

	//���� - �ε��� ����Ʈ ����
	m_vertices.resize(m_iVertexCount);
	m_indices.resize(m_iIndexCount);
	m_vPos = vCPos;

	m_iRowTexTileNum = (m_iWidth - 1);

	//������ ����� �ִ� RECT�� ���� ���� ����
	int halfW = m_iWidth / 2;
	int halfH = m_iHeight / 2;

	//���� �ʱ� ���� : m_fHeightList�� �ִ� ���
	//������� �ϴ� ���� ����� ������ Height���� �ػ󵵸� ��� �� �����Ƿ� �ε����� ��������
	if (m_fHeightList.size())
	{
		UINT idxWidthOffset;
		UINT idxHeightOffset;
		float heightMapY;

		for (int row = 0; row < m_iHeight; row++)
		{
			for (int col = 0; col < m_iWidth; col++)
			{
				//col - halfW + m_vPos.x : �߾� ��ġ�κ��� ���� -X�������� �̵��� ���� ��ġ��ŭ �̵�
				//halfH - row + m_vPos.z : �»���� �������� ���� ��������.
				idxWidthOffset = (col * ((FLOAT)m_iHeightMapWidth / (FLOAT)m_iWidth));
				idxHeightOffset = (row * ((FLOAT)m_iHeightMapHeight / (FLOAT)m_iHeight));
				heightMapY = m_fHeightList[idxHeightOffset * m_iHeightMapWidth + idxWidthOffset];

				m_vertices[row * m_iWidth + col].p = { (FLOAT)(col - halfW + m_vPos.x) * m_fCellSize, heightMapY * m_fHeightScale, (FLOAT)(halfH - row + m_vPos.z) * m_fCellSize };
				m_vertices[row * m_iWidth + col].c = { 1.0f, 1.0f, 1.0f, 1.0f };
				m_vertices[row * m_iWidth + col].t = { m_iRowTexTileNum * (FLOAT)col / (FLOAT)(m_iWidth - 1), m_iRowTexTileNum * (FLOAT)row / (FLOAT)(m_iHeight - 1) };
			}
		}
	}
	else //���� ������ ���� ���� ���� ���
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

	//�ε��� �ʱ� ����
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

	//���� �븻�� ����� ���� �ε��� ����Ʈ
	//m_idxList.assign(m_indices.begin(), m_indices.end());

	//������ �ε��� ����Ʈ�� ��������� �� ������ ���� ������̺��� �����Ѵ�.
	generateVTNLookupTable();

	//��� ���̺��� ���� ��ü ������ ���� ���� �븻�� ����Ѵ�.
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
	//ȸ���� ���� ���� ������ ȸ������ ���ͷ� ǥ���ϱ⿡�� ������ ���ʿ��ϰ� ����.
	//������ �࿡ ���� ȸ���� ���� �Ǵ� �ٸ� ȸ�� ����� �������� ���� ���Ŀ� 
	//����� ������Ŀ��� �����ϴ� ���� �Ϲ����̴�.
	//�� �Լ��� ������ ������ ��ȭ�� �ݿ������� �ʴ´�.

	// ���� ��� R*T
	// Right.x	Right.y	Right.z	0
	// Up.x		Up.y	Up.z	0
	// Look.x	Look.y	Look.z	0
	// Pos.x	Pos.y	Pos.z	1

	//��ġ ������ ��Ŀ��� �����Ѵ�.
	m_vPrevPos = m_vPos;
	m_vPos = { m_matWorld._41, m_matWorld._42, m_matWorld._43 };

	//obj Right-Axis
	m_sightAxis.vRight = { m_matWorld._11, m_matWorld._12, m_matWorld._13 };

	//obj Up-Axis
	m_sightAxis.vUp = { m_matWorld._21, m_matWorld._22, m_matWorld._23 };

	//obj At-Axis
	m_sightAxis.vLook = { m_matWorld._31, m_matWorld._32 , m_matWorld._33 };

	//ȸ�� ��Ŀ��� ������ �� �຤�ʹ� ������ ��Ŀ� ���� ����ȭ�Ǿ����� ���� ���¶�� �����Ѵ�.
	m_sightAxis.vRight.Normalize();
	m_sightAxis.vUp.Normalize();
	m_sightAxis.vLook.Normalize();

	//�ٿ�� ������ ������ �����ϴ� �κ��� �߰��Ǿ�� �Ѵ�.
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
		//�޾ƿ� GPU���� �ּҿ� ������Ʈ ��Ų ��� ���� ������ ����
		//�κ��� ������ ����
		memcpy(mappedCbuf.pData, &m_wvpMat, sizeof(CPRS_CBUF_COORDCONV_MATSET));

		//���� Unmap
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

	//1. ���̸� �ؽ�ó �ҷ�����
	CPRS_Texture* pHeightMapTexture = nullptr;

	CPGC_TEXTURE_MGR.createTextureEx(wszHeightMapName,
		wszTexFlieName,
		D3D11_USAGE_STAGING,
		(D3D11_CPU_ACCESS_FLAG)(D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE));

	pHeightMapTexture = CPGC_TEXTURE_MGR.getPtr(wszHeightMapName);

	if (!pHeightMapTexture) { return E_FAIL; }

	//2. �ҷ��� Texture���� Height Map ������ �о� ����
	D3D11_TEXTURE2D_DESC td = pHeightMapTexture->getTexInfo();

	UINT tW = td.Width;
	UINT tH = td.Height;

	m_iHeightMapWidth = tW;
	m_iHeightMapHeight = tH;
	m_fHeightList.resize(tW * tH);

	D3D11_MAPPED_SUBRESOURCE texSubRes;

	//���� �۾� �ӵ��� ���� D3D11_MAP_READ�ɼ��̴� CPU���� ���� ���� �ϴ� ���̹Ƿ� �ݵ��
	//�ؽ�ó �ε��� D3D11_USAGE_STAGING���� ���־���Ѵ�.
	if (SUCCEEDED(m_pDContext->Map(pHeightMapTexture->getTexBuf(), 0, D3D11_MAP_READ, 0, &texSubRes)))
	{
		//1����Ʈ ũ���� �ؼ��� ���� unsigned char���� �迭
		//���� ���˿� ���� �ٸ�������
		//���� �ݿ��ϴ� GrayScale�ؽ�ó ���Ͽ��� �̾ƿ´�. Red�� �����ص� �ǹǷ� 1����Ʈ ũ����
		BYTE* texelArray = (BYTE*)texSubRes.pData;

		for (int row = 0; row < tH; row++)
		{
			//�ݵ�� RowPitchũ�Ⱑ ���� �� ������ �ش��ϹǷ� �� ũ�⸦ �������� �۾��� ��
			UINT rowAddr = row * texSubRes.RowPitch;

			for (int col = 0; col < tW; col++)
			{
				//RGBA�� ���� 0~255������ ������ �����Ǹ� 
				//�� �� Red�� ������ 0~1���� ������ ����ȭ �Ѵ�.
				//����ȭ�ϸ� ����� ��������.
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
	//�ϳ��� ������ ������ ������ ���� �ִ� 6���� ���� �� �ִ�.
	//	    v2 -  v3
	//	   /  \  /  \ 
	//	 v1  - v0  - v4  
	//	   \  /  \  /
	//	    v6  - v5

	//1. ������ �븻�� ���ϱ� ���ؼ� ������ ���� �������� ����ִ� ������̺� �����
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
	//�ε��� ����Ʈ���� 3���� = 1�� �̹Ƿ� 3���� ��� ���������� ���Ѵ�.
	//�ε����� �ش��ϴ� ������ ��ġ�κ��� �ΰ��� ���⺤�͸� ����� �����Ѵ�.
	//���� ������ ��������.
	//���̽��� ������ ������ ������ ����.
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
	//��� ���̺��� ������ ���� �� ����Ʈ�� ���ؼ� ���� �븻�� ����Ѵ�.
	for (auto it : m_VTNLookupTable[iVertex].adjFaceIdxList)
	{
		UINT i0 = m_indices[it * 3 + 0];
		UINT i1 = m_indices[it * 3 + 1];
		UINT i2 = m_indices[it * 3 + 2];

		//���� ���� �븻�� ����ϰ� ���� �ջ��Ѵ�.
		m_VTNLookupTable[iVertex].vNormal += calcFaceNormal(i0, i1, i2);
	}

	//�ջ��� �븻�� ����ȭ �Ͽ� �����븻�� ����Ѵ�.
	m_VTNLookupTable[iVertex].vNormal.Normalize();
	m_vertices[iVertex].n = m_VTNLookupTable[iVertex].vNormal;
}

FLOAT CPGCMap::getHeight(CPM_Vector3 vPos)
{
	//1. �μ��� �־��� ��ġ�� �ش��ϴ� ���� ���� ��, �� ��ȣ ã��
	//�� ���� ũ��� �߾� ��ġ�� ���� �ٸ������� �߾��� �����̶�� �����ϸ� ������ ����.
	int nRowCellCnt = m_iWidth - 1;
	int nColCellCnt = m_iHeight - 1;

	//�� ��ȣ�� ȭ�� ��ǥ�� ó�� ����Ϸ��� z���� ��ȣ ������Ų��.
	FLOAT fRowLine = (-vPos.z + nColCellCnt * m_fCellSize / 2) / m_fCellSize;
	FLOAT fColLine = (vPos.x + nRowCellCnt * m_fCellSize / 2) / m_fCellSize;

	//ũ�� ���� ���� ����� ���� ������ �� ��ȣ�� �ȴ�.
	//floor�� �Ҽ��θ� �߶󳻴°��� ������ �׳� ĳ�����ص� �������Ƿ� �̸� �̿��Ѵ�.
	int iRowLine = min(fRowLine, nRowCellCnt - 1);
	iRowLine = max(iRowLine, 0);

	int iColLine = min(fColLine, nColCellCnt - 1);
	iColLine = max(iColLine, 0);

	//2. ã�� ���� ��, ����ȣ�� �̿��� ���� �ش��ϴ� 4������ ���� ���� ������ �������� ���Ѵ�.
	//0 - 1
	//| / |
	//3 - 2
	FLOAT vH[4];
	vH[0] = m_vertices[iRowLine * m_iWidth + iColLine].p.y;
	vH[1] = m_vertices[iRowLine * m_iWidth + iColLine + 1].p.y;
	vH[2] = m_vertices[(iRowLine + 1) * m_iWidth + iColLine + 1].p.y;
	vH[3] = m_vertices[(iRowLine + 1) * m_iWidth + iColLine].p.y;

	//3. ������ ���� ���� face�� ã�´�.
	FLOAT res, t1, t2;

	FLOAT offZ = fRowLine - iRowLine;
	FLOAT offX = fColLine - iColLine;

	//X���� ������ ��ȭ������ ���� �����Ѵ�.
	if (offX + offZ < 1.0f)
	{
		//���� ��� �� ����
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
		//������ �ϴ� �� ����
		//2 -> 3 : invOffX
		t1 = lerp(0.0f, vH[3] - vH[2], invOffX);
		//2 -> 1 : invOffZ
		t2 = lerp(vH[2], vH[1], invOffZ);

		res = t2 + t1;
	}

	return res;
}

//X���� ������ ��ȭ������ ���� �����Ѵ�.
FLOAT CPGCMap::lerp(FLOAT opd1, FLOAT opd2, FLOAT fRatio)
{
	//fRatio���� 0 ~ 1���̶�� �����Ѵ�.
	return opd2 * fRatio + opd1 * (1.0f - fRatio);
}

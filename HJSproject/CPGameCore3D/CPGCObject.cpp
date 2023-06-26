#include "CPGCObject.h"

CPGCObject::CPGCObject()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;
}

CPGCObject::~CPGCObject()
{
}

void CPGCObject::setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	if (pDevice && pDContext)
	{
		m_pDevice = pDevice;
		m_pDContext = pDContext;
	}
}

CPGCActor::CPGCActor()
{
	m_pMesh = nullptr;
	m_iCBufCount = 0;
	m_iSamplerCount = 0;
}

CPGCActor::~CPGCActor()
{
}

bool CPGCActor::init()
{
	return true;
}

bool CPGCActor::update()
{
	return true;
}

bool CPGCActor::preUpdate()
{
	return true;
}

bool CPGCActor::postUpdate()
{
	return true;
}

bool CPGCActor::render()
{
	preRender();

	if (m_pMesh) 
	{ 
		m_pMesh->bindToPipeline(m_pDContext);
		if (m_pMesh->m_iIndexCount)
		{
			m_pDContext->DrawIndexed(m_pMesh->m_iIndexCount, 0, 0);
		}
		else
		{
			m_pDContext->Draw(m_pMesh->m_iVertexCount, 0);
		}
	}

	//postRender();
	return true;
}

bool CPGCActor::preRender()
{
	bindToPipeline();

	return true;
}

bool CPGCActor::postRender()
{
	return true;
}

bool CPGCActor::release()
{
	for (auto it : m_pChildList)
	{
		it->release();
		delete it;
		it = nullptr;
	}

	m_pChildList.clear();

	for (auto it : m_pVSCBufs)
	{
		it->Release();
		it = nullptr;
	}

	m_pMesh = nullptr;

	m_pVSCBufs.clear();
	m_pSamplers.clear();

	return true;
}

HRESULT CPGCActor::create(std::wstring wszObjName, std::wstring wszMeshName)
{
	HRESULT hr = S_OK;

	m_pMesh = CPGC_MESH_MGR.getPtr(wszMeshName);
	if (!m_pMesh) { return E_INVALIDARG; }

	m_pVSCBufs.resize(1);
	m_iCBufCount++;

	hr = createDXSimpleBuf(m_pDevice,
		sizeof(CPRS_CBUF_COORDCONV_MATSET),
		&m_wvpMat,
		D3D11_BIND_CONSTANT_BUFFER,
		&m_pVSCBufs[0]);

	if (FAILED(hr)) { return hr; }

	return E_NOTIMPL;
}

void CPGCActor::addSampler(std::wstring wszSamplerName)
{
	m_pSamplers.push_back(CPGC_DXSTATE_MGR.getSState(wszSamplerName));
	m_iSamplerCount = m_pSamplers.size();
}

void CPGCActor::setMatrix(	const CPM_Matrix* pWorldM,
							const CPM_Matrix* pViewM,
							const CPM_Matrix* pProjM)
{
	if (pWorldM) { m_matWorld = *pWorldM; }
	if (pViewM) { m_matView = *pViewM; }
	if (pProjM) { m_matProj = *pProjM; }

	updateState();
	updateCoordConvMat(m_matWorld, m_matView, m_matProj);
}

HRESULT CPGCActor::updateCoordConvMat(const CPM_Matrix& matWorld, const CPM_Matrix& matView, const CPM_Matrix& matProj)
{
	matWorld.Transpose(m_wvpMat.matTWorld);

	matWorld.Invert(m_wvpMat.matTInvWorld);
	m_wvpMat.matTInvWorld = m_wvpMat.matTInvWorld.Transpose();

	matView.Transpose(m_wvpMat.matTView);
	matProj.Transpose(m_wvpMat.matTProj);

	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedCbuf;
	ZeroMemory(&mappedCbuf, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = m_pDContext->Map(m_pVSCBufs[0],
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
		m_pDContext->Unmap(m_pVSCBufs[0], 0);
	}

	ZeroMemory(&mappedCbuf, sizeof(D3D11_MAPPED_SUBRESOURCE));

	for (auto it : m_pChildList)
	{
		it->updateCoordConvMat(it->m_wvpMat.matTWorld * matWorld,
			matView, matProj);
	}

	return hr;
}

void CPGCActor::updateState()
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

void CPGCActor::setMesh(std::wstring wszMeshName)
{
	m_pMesh = CPGC_MESH_MGR.getPtr(wszMeshName);
}

void CPGCActor::bindToPipeline()
{
	m_shaderGroup.bindToPipeline(m_pDContext);

	if (m_iCBufCount)
	{
		m_pDContext->VSSetConstantBuffers(0, m_iCBufCount, m_pVSCBufs.data());
	}

	if (m_iSamplerCount)
	{
		m_pDContext->PSSetSamplers(0, m_iSamplerCount, m_pSamplers.data());
	}
}

CPM_Vector3 CPGCActor::getVRight()
{
	return m_sightAxis.vRight;
}

CPM_Vector3 CPGCActor::getVUp()
{
	return m_sightAxis.vUp;
}

CPM_Vector3 CPGCActor::getVForward()
{
	return m_sightAxis.vLook;
}

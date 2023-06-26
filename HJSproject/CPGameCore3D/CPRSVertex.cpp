#include "CPRSVertex.h"

CPGCVertexLayoutManager::CPGCVertexLayoutManager()
{
	m_pDevice = nullptr;
}

CPGCVertexLayoutManager::~CPGCVertexLayoutManager()
{
}

bool CPGCVertexLayoutManager::init(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	if (!pDevice) { return false; }
	else { m_pDevice = pDevice; }

	//D3D11_INPUT_ELEMENT_DESC 유의사항
	//InputSlotClass가 D3D11_INPUT_PER_VERTEX_DATA인 경우 반드시 마지막 인수는 NULL(0)이어야 한다.

	D3D11_INPUT_ELEMENT_DESC pcIed[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, NULL},
	};

	D3D11_INPUT_ELEMENT_DESC pnctIed[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, NULL},
	};

	D3D11_INPUT_ELEMENT_DESC pnctiwIed[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "INDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, NULL},
	};

	D3D11_INPUT_ELEMENT_DESC pointParticleLayoutIed[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "SPRITERT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "ROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "ROTATION", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "ROTATION", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 96, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "ROTATION", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 112, D3D11_INPUT_PER_VERTEX_DATA, NULL},
		{ "SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 128, D3D11_INPUT_PER_VERTEX_DATA, NULL}
	};

	hr = createVLayout(L"VL_PC", pcIed, CPGC_ARRAY_ELEM_NUM(pcIed), L"../../data/shader/layout/VL_PC.hlsl");
	hr = createVLayout(L"VL_PNCT", pnctIed, CPGC_ARRAY_ELEM_NUM(pnctIed), L"../../data/shader/layout/VL_PNCT.hlsl");
	hr = createVLayout(L"VL_PNCTIW", pnctiwIed, CPGC_ARRAY_ELEM_NUM(pnctiwIed), L"../../data/shader/layout/VL_PNCTIW.hlsl");
	hr = createVLayout(L"VL_POINTPS", pointParticleLayoutIed, CPGC_ARRAY_ELEM_NUM(pointParticleLayoutIed), L"../../data/shader/layout/VL_POINTPS.hlsl");

	return true;
}

bool CPGCVertexLayoutManager::release()
{
	m_pDevice = nullptr;
	for (auto it : m_VertexLayoutMap)
	{
		it.second->Release();
		it.second = nullptr;
	}

	m_VertexLayoutMap.clear();
	m_vsFilenameSet.clear();

	return true;
}

HRESULT CPGCVertexLayoutManager::createVLayout(std::wstring wszLayoutName, D3D11_INPUT_ELEMENT_DESC* iedArr, UINT iCnt, std::wstring wszVSFilename)
{
	HRESULT hr = S_OK;

	//같은 이름이 없는경우 새로 생성
	if (m_VertexLayoutMap.find(wszLayoutName) == m_VertexLayoutMap.end() &&
		m_vsFilenameSet.find(wszVSFilename) == m_vsFilenameSet.end())
	{
		//D3D11_INPUT_ELEMENT_DESC 배열이 미지정시 함수 수행 종료
		if (!iedArr) { hr = E_POINTER; return hr; }

		//기본 레이아웃용 Shader파일을 이용해 레이아웃 생성
		Microsoft::WRL::ComPtr<ID3DBlob>	pVsCode;
		Microsoft::WRL::ComPtr<ID3DBlob>	pErrCode;

		ID3D11InputLayout* pVLayout;

		std::string vsTarget = CPRS_VSLAYOUT_TARGET;
		vsTarget += CPRS_LAYOUT_SHADER_VERSION;

		hr = D3DCompileFromFile(	wszVSFilename.c_str(),
									NULL,
									NULL,
									CPRS_VSLAYOUT_ENTRYPOINT,
									vsTarget.c_str(),
									0, 0,
									pVsCode.GetAddressOf(),
									pErrCode.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			//pShaderBytecodeWithInputSignature : Shader와 구조가 동일한 구조 레이아웃인지 판단하기 위한 컴파일된 shader 바이트 코드
			//BytecodeLength : 위 바이트 코드의 길이
			hr = m_pDevice->CreateInputLayout(iedArr, 
												iCnt, 
												pVsCode->GetBufferPointer(), 
												pVsCode->GetBufferSize(), 
												&pVLayout);

			if (SUCCEEDED(hr)) 
			{ 
				//성공시 이름 매핑
				m_vsFilenameSet.insert(wszVSFilename);
				m_VertexLayoutMap.insert(std::make_pair(wszLayoutName, pVLayout)); 
			}
		}
		else 
		{ 
			//실패시 오류 메시지 출력
			if (pErrCode.Get())
			{
				OutputDebugStringA((char*)pErrCode->GetBufferPointer());
			}
		}
	}
	else { hr = S_FALSE; }

	return hr;
}

ID3D11InputLayout* CPGCVertexLayoutManager::getPtr(std::wstring wszLayoutName)
{
	auto it = m_VertexLayoutMap.find(wszLayoutName);
	if (it != m_VertexLayoutMap.end()) { return it->second; }

	return nullptr;
}

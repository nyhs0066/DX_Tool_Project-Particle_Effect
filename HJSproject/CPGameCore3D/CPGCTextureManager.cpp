#include "CPGCTextureManager.h"

CPRS_Texture::CPRS_Texture()
{
	m_pDContext = nullptr;
	ZeroMemory(&m_texDesc, sizeof(D3D11_TEXTURE2D_DESC));
}

CPRS_Texture::~CPRS_Texture()
{
}

D3D11_TEXTURE2D_DESC CPRS_Texture::getTexInfo()
{
	return m_texDesc;
}

ID3D11Texture2D* CPRS_Texture::getTexBuf()
{
	return m_texBuf.Get();
}

ID3D11ShaderResourceView* CPRS_Texture::getSRV()
{
	return m_pSRView.Get();
}

std::wstring CPRS_Texture::getName()
{
	return m_wszName;
}

std::wstring CPRS_Texture::getFilePath()
{
	return m_wszFilePath;
}

CPGCTextureManager::CPGCTextureManager()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;
}

CPGCTextureManager::~CPGCTextureManager()
{
}

bool CPGCTextureManager::init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	if (!pDevice || !pDContext) { return false; }
	else
	{
		m_pDevice = pDevice;
		m_pDContext = pDContext;
	}

	return true;
}

bool CPGCTextureManager::release()
{
	m_scriptFilenameSet.clear();
	m_filenameMap.clear();

	for (auto it : m_textureMap)
	{
		delete it.second;
		it.second = nullptr;
	}

	m_textureMap.clear();

	return true;
}

HRESULT CPGCTextureManager::createTexture(std::wstring wszTexureName, std::wstring wszTexFilename)
{
	HRESULT hr = S_OK;

	//등록된 이름이 없으면 생성
	if (m_textureMap.find(wszTexureName) == m_textureMap.end() &&
		m_filenameMap.find(wszTexFilename) == m_filenameMap.end())
	{
		CPRS_Texture* newE = new CPRS_Texture;
		newE->m_wszName = wszTexureName;
		newE->m_wszFilePath = wszTexFilename;
		newE->m_pDContext = m_pDContext;

		//WIC(Windows Imaging Component) 텍스처 리소스 생성함수
		//BMP, JPEG, PNG, TIFF, GIF, HD Photo, or other WIC supported file container 포맷을 처리한다.
		hr = DirectX::CreateWICTextureFromFile(	m_pDevice,
												m_pDContext,
												wszTexFilename.c_str(),
												(ID3D11Resource**)newE->m_texBuf.GetAddressOf(),
												newE->m_pSRView.GetAddressOf());

		if (FAILED(hr))
		{
			//DDS(DirectDraw Surface) 텍스처 리소스 생성함수
			//dds 포맷을 처리한다.
			hr = DirectX::CreateDDSTextureFromFile(	m_pDevice,
													m_pDContext,
													wszTexFilename.c_str(),
													(ID3D11Resource**)newE->m_texBuf.GetAddressOf(),
													newE->m_pSRView.GetAddressOf());

			if (FAILED(hr)) 
			{ 
				printCreateErr(hr); 
				delete newE;
				return hr; 
			}
		}

		//생성된 텍스처 리소스의 속성과 이름을 저장한다.
		//속성값에는 텍스처의 해상도도 포함되어 있다.
		newE->m_texBuf->GetDesc(&newE->m_texDesc);
		newE->m_wszName = wszTexureName;

		m_filenameMap.insert(std::make_pair(wszTexFilename, wszTexureName));
		m_textureMap.insert(std::make_pair(wszTexureName, newE));
	}
	else { hr = S_FALSE; return hr; }

	return hr;
}

HRESULT CPGCTextureManager::createTextureEx(std::wstring wszTexureName, 
	std::wstring wszTexFilename, 
	D3D11_USAGE resourceUsage, 
	D3D11_CPU_ACCESS_FLAG CPUFlag)
{
	HRESULT hr = S_OK;

	//등록된 이름이 없으면 생성
	if (m_textureMap.find(wszTexureName) == m_textureMap.end())
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> texRes;
		CPRS_Texture* newE = new CPRS_Texture;
		newE->m_wszName = wszTexureName;
		newE->m_wszFilePath = wszTexFilename;
		newE->m_pDContext = m_pDContext;

		//WIC(Windows Imaging Component) 텍스처 리소스 생성함수
		//BMP, JPEG, PNG, TIFF, GIF, HD Photo, or other WIC supported file container 포맷을 처리한다.
		hr = DirectX::CreateWICTextureFromFileEx(m_pDevice,
			m_pDContext,
			wszTexFilename.c_str(),
			0,
			resourceUsage,
			0,
			CPUFlag,
			0,
			DirectX::WIC_LOADER_DEFAULT,
			texRes.GetAddressOf(),
			nullptr);

		if (FAILED(hr))
		{
			//DDS(DirectDraw Surface) 텍스처 리소스 생성함수
			//dds 포맷을 처리한다.
			hr = DirectX::CreateDDSTextureFromFileEx(m_pDevice,
				m_pDContext,
				wszTexFilename.c_str(),
				0,
				resourceUsage,
				0,
				CPUFlag,
				0,
				DirectX::DDS_LOADER_DEFAULT,
				texRes.GetAddressOf(),
				nullptr);

			if (FAILED(hr))
			{
				printCreateErr(hr);
				delete newE;
				return hr;
			}
		}

		if (SUCCEEDED(hr))
		{
			texRes.Get()->QueryInterface<ID3D11Texture2D>(newE->m_texBuf.GetAddressOf());
		}

		//생성된 텍스처 리소스의 속성과 이름을 저장한다.
		//속성값에는 텍스처의 해상도도 포함되어 있다.
		newE->m_texBuf->GetDesc(&newE->m_texDesc);
		newE->m_wszName = wszTexureName;

		m_textureMap.insert(std::make_pair(wszTexureName, newE));
	}
	else { hr = S_FALSE; }

	return hr;
}

HRESULT CPGCTextureManager::createAlphaMaskTexture(UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> retTexBuf;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> retTexSRV;

	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));

	td.Width = iWidth;
	td.Height = iHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	BYTE* m_fAlphaData = new BYTE[iWidth * iHeight * 4];
	for (UINT y = 0; y < iHeight; y++)
	{
		for (UINT x = 0; x < iWidth; x++)
		{
			BYTE* pixel = &m_fAlphaData[iWidth * y * 4 + x * 4];
			pixel[0] = 0;
			pixel[1] = 0;
			pixel[2] = 0;
			pixel[3] = 0;
		}
	}

	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = m_fAlphaData;
	sd.SysMemPitch = sizeof(BYTE) * 4 * iWidth;
	sd.SysMemSlicePitch = 0;

	if (SUCCEEDED(m_pDevice->CreateTexture2D(&td, &sd, retTexBuf.GetAddressOf())))
	{
		m_pDevice->CreateShaderResourceView(retTexBuf.Get(), NULL, retTexSRV.GetAddressOf());
	}

	return hr;
}

CPRS_Texture* CPGCTextureManager::getPtr(std::wstring wszTexureName)
{
	auto it = m_textureMap.find(wszTexureName);
	if (it != m_textureMap.end()) { return it->second; }

	return nullptr;
}

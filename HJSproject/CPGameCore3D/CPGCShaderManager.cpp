#include "CPGCShaderManager.h"

HRESULT CPGCShaderManager::compileShader(std::wstring wszFileName, 
	CPRS_SHADER_COMPILE_TYPE compileType, 
	void* pShader,
	CPRS_SO_OPTION* pSOOpt)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob>		m_pCompiledShaderCode;
	Microsoft::WRL::ComPtr<ID3DBlob>		m_pErrCode;

	std::string shaderType = m_compSet[compileType].second;
	std::string shaderLangVer = CPRS_DEFAULT_SHADER_LANG_VERSION;

	std::string compileTarget = shaderType + shaderLangVer;

#ifdef _DEBUG
	UINT shaderFlag = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PREFER_FLOW_CONTROL;
#endif //_DEBUG

	hr = D3DCompileFromFile(wszFileName.c_str(),
		nullptr,												//shader가 사용할 매크로 정의 구조체
		nullptr,												//shader가 사용할 Include파일 도입을 위한 인터페이스
		m_compSet[compileType].first.c_str(),					//shader 진입점
		compileTarget.c_str(),									//컴파일 shader 종류와 버전 문자열

#ifdef _DEBUG
		shaderFlag, 0,											//컴파일 옵션
#else
		0, 0,													//컴파일 옵션
#endif //_DEBUG
		m_pCompiledShaderCode.GetAddressOf(),
		m_pErrCode.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		hr = createShader_Internal(m_pCompiledShaderCode->GetBufferPointer(),
			m_pCompiledShaderCode->GetBufferSize(),
			compileType,
			pShader,
			pSOOpt);
	}
	else
	{
		if (m_pErrCode.Get())
		{
			OutputDebugStringA((char*)m_pErrCode->GetBufferPointer());
		}
	}

	return hr;
}

HRESULT CPGCShaderManager::createShader_Internal(void* pData, 
	size_t iSize, 
	CPRS_SHADER_COMPILE_TYPE compileType,
	void* pShader,
	CPRS_SO_OPTION* pSOOpt)
{
	HRESULT hr = S_OK;

	switch (compileType)
	{
		case CPRS_SCT_VS:
		{
			hr = m_pDevice->CreateVertexShader(pData,
				iSize,
				nullptr,
				(ID3D11VertexShader**)pShader);
		} break;

		case CPRS_SCT_HS:
		{
			hr = m_pDevice->CreateHullShader(pData,
				iSize,
				nullptr,
				(ID3D11HullShader**)pShader);
		} break;

		case CPRS_SCT_DS:
		{
			hr = m_pDevice->CreateDomainShader(pData,
				iSize,
				nullptr,
				(ID3D11DomainShader**)pShader);
		} break;

		case CPRS_SCT_GS:
		{
			hr = m_pDevice->CreateGeometryShader(pData,
				iSize,
				nullptr,
				(ID3D11GeometryShader**)pShader);
		} break;

		case CPRS_SCT_GS_SO:
		{
			if (pSOOpt)
			{
				hr = m_pDevice->CreateGeometryShaderWithStreamOutput(pData,
					iSize,
					pSOOpt->pSOEntry,
					pSOOpt->iNumEntry,
					pSOOpt->pStrides,
					pSOOpt->iNumStride,
					pSOOpt->iRasterizedStream,
					pSOOpt->pClassLinkage,
					(ID3D11GeometryShader**)pShader);
			}
			else
			{
				hr = E_INVALIDARG;

				return hr;
			}
		} break;

		case CPRS_SCT_PS:
		{
			hr = m_pDevice->CreatePixelShader(pData,
				iSize,
				nullptr,
				(ID3D11PixelShader**)pShader);
		} break;

		case CPRS_SCT_CS:
		{
			hr = m_pDevice->CreateComputeShader(pData,
				iSize,
				nullptr,
				(ID3D11ComputeShader**)pShader);
		} break;
	}

	return hr;
}

std::vector<BYTE> CPGCShaderManager::loadCsoFile(std::wstring loadFilepath)
{
	std::vector<BYTE> ret;
	LARGE_INTEGER iSize;

	HANDLE hRead = CreateFile(loadFilepath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hRead)
	{
		::GetFileSizeEx(hRead, &iSize);			//파일 크기 불러오기
		CloseHandle(hRead);

		ret.resize(iSize.QuadPart);
		if (!CPGC_FILEIO_MGR.loadFile(loadFilepath, ret.data())) { ret.clear(); }
	}

	return ret;
}

CPGCShaderManager::CPGCShaderManager()
{
	m_pDevice = nullptr;
}

CPGCShaderManager::~CPGCShaderManager()
{
}

bool CPGCShaderManager::init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	HRESULT hr = S_OK;

	m_pDevice = pDevice;

	m_compSet.push_back(std::make_pair(CPRS_VS_ENTRYPOINT, CPRS_VS_TARGET));
	m_compSet.push_back(std::make_pair(CPRS_HS_ENTRYPOINT, CPRS_HS_TARGET));
	m_compSet.push_back(std::make_pair(CPRS_DS_ENTRYPOINT, CPRS_DS_TARGET));
	m_compSet.push_back(std::make_pair(CPRS_GS_ENTRYPOINT, CPRS_GS_TARGET));
	m_compSet.push_back(std::make_pair(CPRS_GS_ENTRYPOINT, CPRS_GS_TARGET));
	m_compSet.push_back(std::make_pair(CPRS_PS_ENTRYPOINT, CPRS_PS_TARGET));
	m_compSet.push_back(std::make_pair(CPRS_CS_ENTRYPOINT, CPRS_CS_TARGET));

	if (!pDevice || !pDContext) { return false; }

	createPresetShaderGroups();

	return true;
}

bool CPGCShaderManager::release()
{
	m_compSet.clear();

	m_VSMap.clear();
	m_HSMap.clear();
	m_DSMap.clear();
	m_GSMap.clear();
	m_PSMap.clear();
	m_CSMap.clear();

	return true;
}

HRESULT CPGCShaderManager::createShader(std::wstring wszShaderName, 
	CPRS_SHADER_COMPILE_TYPE shaderType, 
	std::wstring wszFilepath,
	CPRS_SO_OPTION* pSOOpt)
{
	HRESULT hr = S_OK;

	switch (shaderType)
	{
		case CPRS_SCT_VS:
		{
			if (m_VSMap.find(wszShaderName) == m_VSMap.end())
			{
				auto it = m_VSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = compileShader(wszFilepath, shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_VSMap.erase(it.first);
				}
				else
				{
					m_VSHlslMap.insert(std::make_pair(wszShaderName, wszFilepath));
				}
			}
			else { hr = S_FALSE; }

		} break;

		case CPRS_SCT_HS:
		{
			if (m_HSMap.find(wszShaderName) == m_HSMap.end())
			{
				auto it = m_HSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = compileShader(wszFilepath, shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_HSMap.erase(it.first);
				}
				else
				{
					m_HSHlslMap.insert(std::make_pair(wszShaderName, wszFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_DS:
		{
			if (m_DSMap.find(wszShaderName) == m_DSMap.end())
			{
				auto it = m_DSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = compileShader(wszFilepath, shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_DSMap.erase(it.first);
				}
				else
				{
					m_DSHlslMap.insert(std::make_pair(wszShaderName, wszFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_GS:
		{
			if (m_GSMap.find(wszShaderName) == m_GSMap.end())
			{
				auto it = m_GSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = compileShader(wszFilepath, shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_GSMap.erase(it.first);
				}
				else
				{
					m_GSHlslMap.insert(std::make_pair(wszShaderName, wszFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_GS_SO:
		{
			if (m_GS_SOMap.find(wszShaderName) == m_GS_SOMap.end())
			{
				auto it = m_GS_SOMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = compileShader(wszFilepath, shaderType, it.first->second.GetAddressOf(), pSOOpt);

				if (FAILED(hr))
				{
					m_GS_SOMap.erase(it.first);
				}
				else
				{
					m_GS_SOHlslMap.insert(std::make_pair(wszShaderName, wszFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_PS:
		{
			if (m_PSMap.find(wszShaderName) == m_PSMap.end())
			{
				auto it = m_PSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = compileShader(wszFilepath, shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_PSMap.erase(it.first);
				}
				else
				{
					m_PSHlslMap.insert(std::make_pair(wszShaderName, wszFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_CS:
		{
			if (m_CSMap.find(wszShaderName) == m_CSMap.end())
			{
				auto it = m_CSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = compileShader(wszFilepath, shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_CSMap.erase(it.first);
				}
				else
				{
					m_CSHlslMap.insert(std::make_pair(wszShaderName, wszFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;
	}

	return hr;
}

HRESULT CPGCShaderManager::createByteCodeShader(std::wstring wszShaderName, CPRS_SHADER_COMPILE_TYPE shaderType, std::wstring wszByteCodeFilepath, CPRS_SO_OPTION* pSOOpt)
{
	HRESULT hr = S_OK;

	//std::vector<BYTE>의 복사본으로 받을 예정
	auto blob = loadCsoFile(wszByteCodeFilepath);
	if (!blob.size()) { return E_FAIL; }

	switch (shaderType)
	{
		case CPRS_SCT_VS:
		{
			if (m_VSMap.find(wszShaderName) == m_VSMap.end())
			{
				auto it = m_VSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = createShader_Internal(blob.data(), blob.size(), shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_VSMap.erase(it.first);
				}
				else
				{
					m_VSHlslMap.insert(std::make_pair(wszShaderName, wszByteCodeFilepath));
				}
			}
			else { hr = S_FALSE; }

		} break;

		case CPRS_SCT_HS:
		{
			if (m_HSMap.find(wszShaderName) == m_HSMap.end())
			{
				auto it = m_HSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = createShader_Internal(blob.data(), blob.size(), shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_HSMap.erase(it.first);
				}
				else
				{
					m_HSHlslMap.insert(std::make_pair(wszShaderName, wszByteCodeFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_DS:
		{
			if (m_DSMap.find(wszShaderName) == m_DSMap.end())
			{
				auto it = m_DSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = createShader_Internal(blob.data(), blob.size(), shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_DSMap.erase(it.first);
				}
				else
				{
					m_DSHlslMap.insert(std::make_pair(wszShaderName, wszByteCodeFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_GS:
		{
			if (m_GSMap.find(wszShaderName) == m_GSMap.end())
			{
				auto it = m_GSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = createShader_Internal(blob.data(), blob.size(), shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_GSMap.erase(it.first);
				}
				else
				{
					m_GSHlslMap.insert(std::make_pair(wszShaderName, wszByteCodeFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_GS_SO:
		{
			if (m_GS_SOMap.find(wszShaderName) == m_GS_SOMap.end())
			{
				auto it = m_GS_SOMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = createShader_Internal(blob.data(), blob.size(), shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_GS_SOMap.erase(it.first);
				}
				else
				{
					m_GS_SOCsoMap.insert(std::make_pair(wszShaderName, wszByteCodeFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_PS:
		{
			if (m_PSMap.find(wszShaderName) == m_PSMap.end())
			{
				auto it = m_PSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = createShader_Internal(blob.data(), blob.size(), shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_PSMap.erase(it.first);
				}
				else
				{
					m_PSHlslMap.insert(std::make_pair(wszShaderName, wszByteCodeFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;

		case CPRS_SCT_CS:
		{
			if (m_CSMap.find(wszShaderName) == m_CSMap.end())
			{
				auto it = m_CSMap.insert(std::make_pair(wszShaderName, nullptr));

				hr = createShader_Internal(blob.data(), blob.size(), shaderType, it.first->second.GetAddressOf());

				if (FAILED(hr))
				{
					m_CSMap.erase(it.first);
				}
				else
				{
					m_CSHlslMap.insert(std::make_pair(wszShaderName, wszByteCodeFilepath));
				}
			}
			else { hr = S_FALSE; }
		} break;
	}

	return hr;
}

ID3D11VertexShader* CPGCShaderManager::getVS(std::wstring wszShaderName)
{
	auto it = m_VSMap.find(wszShaderName);
	if(it != m_VSMap.end())
	{
		return it->second.Get();
	}

	return nullptr;
}

ID3D11HullShader* CPGCShaderManager::getHS(std::wstring wszShaderName)
{
	auto it = m_HSMap.find(wszShaderName);
	if (it != m_HSMap.end())
	{
		return it->second.Get();
	}

	return nullptr;
}

ID3D11DomainShader* CPGCShaderManager::getDS(std::wstring wszShaderName)
{
	auto it = m_DSMap.find(wszShaderName);
	if (it != m_DSMap.end())
	{
		return it->second.Get();
	}

	return nullptr;
}

ID3D11GeometryShader* CPGCShaderManager::getGS(std::wstring wszShaderName)
{
	auto it = m_GSMap.find(wszShaderName);
	if (it != m_GSMap.end())
	{
		return it->second.Get();
	}

	return nullptr;
}

ID3D11GeometryShader* CPGCShaderManager::getGS_SO(std::wstring wszShaderName)
{
	auto it = m_GS_SOMap.find(wszShaderName);
	if (it != m_GS_SOMap.end())
	{
		return it->second.Get();
	}

	return nullptr;
}

ID3D11PixelShader* CPGCShaderManager::getPS(std::wstring wszShaderName)
{
	auto it = m_PSMap.find(wszShaderName);
	if (it != m_PSMap.end())
	{
		return it->second.Get();
	}

	return nullptr;
}

ID3D11ComputeShader* CPGCShaderManager::getCS(std::wstring wszShaderName)
{
	auto it = m_CSMap.find(wszShaderName);
	if (it != m_CSMap.end())
	{
		return it->second.Get();
	}

	return nullptr;
}

bool CPGCShaderManager::getShaderPreset(std::wstring wsaShaderGroupName, CPRS_ShaderGroup& shaderGroup)
{
	auto it = m_shaderPresetMap.find(wsaShaderGroupName);
	if (it != m_shaderPresetMap.end())
	{
		shaderGroup.pVS = it->second.pVS;
		shaderGroup.pHS = it->second.pHS;
		shaderGroup.pDS = it->second.pDS;
		shaderGroup.pGS = it->second.pGS;
		shaderGroup.pPS = it->second.pPS;
		shaderGroup.pCS = it->second.pCS;

		return true;
	}

	return false;
}

void CPGCShaderManager::makeShaderGroup(std::wstring VS,
										std::wstring HS,
										std::wstring DS,
										std::wstring GS,
										std::wstring PS,
										std::wstring CS,
										CPRS_ShaderGroup& shaderGroup)
{
	shaderGroup.pVS = getVS(VS);
	shaderGroup.pHS = getHS(HS);
	shaderGroup.pDS = getDS(DS);
	shaderGroup.pGS = getGS(GS);
	shaderGroup.pPS = getPS(PS);
	shaderGroup.pCS = getCS(CS);
}

bool CPGCShaderManager::loadHLSLScript(std::wstring wszLoadFilepath)
{
	std::wifstream wifp;

	wifp.open(wszLoadFilepath.c_str(), std::ios::in);
	if (!wifp) { return false; }

	std::wstring temp;
	std::wstring type;
	std::wstring shadername;
	std::wstring filepath;

	CPRS_SHADER_COMPILE_TYPE shaderType;

	HRESULT hr = S_OK;

	wifp >> temp;

	if (temp != L"#END")
	{
		while (true)
		{
			wifp >> type;
			if (type == L"#END") { break; }
			else
			{
				if (type == L"VS") { shaderType = CPRS_SCT_VS; }
				if (type == L"HS") { shaderType = CPRS_SCT_HS; }
				if (type == L"DS") { shaderType = CPRS_SCT_DS; }
				if (type == L"GS") { shaderType = CPRS_SCT_GS; }
				if (type == L"PS") { shaderType = CPRS_SCT_PS; }
				if (type == L"CS") { shaderType = CPRS_SCT_CS; }

				wifp >> shadername;
				wifp >> filepath;

				switch (shaderType)
				{
					case CPRS_SCT_VS :
					{
						hr = createShader(shadername, CPRS_SCT_VS, filepath);
						if (SUCCEEDED(hr))
						{
							m_VSHlslMap.insert(std::make_pair(shadername, filepath));
						}
					
					} break;

					case CPRS_SCT_HS:
					{
						hr = createShader(shadername, CPRS_SCT_HS, filepath);
						if (SUCCEEDED(hr))
						{
							m_HSHlslMap.insert(std::make_pair(shadername, filepath));
						}

					} break;

					case CPRS_SCT_DS:
					{
						hr = createShader(shadername, CPRS_SCT_DS, filepath);
						if (SUCCEEDED(hr))
						{
							m_DSHlslMap.insert(std::make_pair(shadername, filepath));
						}

					} break;

					case CPRS_SCT_GS:
					{
						hr = createShader(shadername, CPRS_SCT_GS, filepath);
						if (SUCCEEDED(hr))
						{
							m_GSHlslMap.insert(std::make_pair(shadername, filepath));
						}

					} break;

					case CPRS_SCT_PS:
					{
						hr = createShader(shadername, CPRS_SCT_PS, filepath);
						if (SUCCEEDED(hr))
						{
							m_PSHlslMap.insert(std::make_pair(shadername, filepath));
						}

					} break;

					case CPRS_SCT_CS:
					{
						hr = createShader(shadername, CPRS_SCT_CS, filepath);
						if (SUCCEEDED(hr))
						{
							m_CSHlslMap.insert(std::make_pair(shadername, filepath));
						}

					} break;
				}
			}
		}
	}

	return true;
}

bool CPGCShaderManager::loadHLSLDirectory(std::wstring wszDirPath)
{
	std::vector<std::wstring> InExtArr;
	std::vector<std::wstring> OutPath;
	std::vector<std::wstring> OutName;

	InExtArr.push_back(L".HLSL");
	InExtArr.push_back(L".hlsl");

	bool ret = loadDir(CPMAC_DEFAULT_CSO_SHADER_DIRPATH, &InExtArr,
		&OutPath,
		&OutName);

	if (!ret) { return false; }

	CPRS_SHADER_COMPILE_TYPE shaderType;

	HRESULT hr = S_OK;

	std::wstring wszType;
	std::wstring wszName;
	std::wstring wszPath;

	for (int i = 0; i < OutPath.size(); i++)
	{
		wszType.assign(wszName.begin(), wszName.begin() + 1);

		if (wszType == L"VS") { shaderType = CPRS_SCT_VS; }
		if (wszType == L"HS") { shaderType = CPRS_SCT_HS; }
		if (wszType == L"DS") { shaderType = CPRS_SCT_DS; }
		if (wszType == L"GS") { shaderType = CPRS_SCT_GS; }
		if (wszType == L"PS") { shaderType = CPRS_SCT_PS; }
		if (wszType == L"CS") { shaderType = CPRS_SCT_CS; }

		switch (shaderType)
		{
		case CPRS_SCT_VS:
		{
			hr = createShader(wszName, CPRS_SCT_VS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_VSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_HS:
		{
			hr = createShader(wszName, CPRS_SCT_HS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_HSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_DS:
		{
			hr = createShader(wszName, CPRS_SCT_DS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_DSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_GS:
		{
			hr = createShader(wszName, CPRS_SCT_GS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_GSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_PS:
		{
			hr = createShader(wszName, CPRS_SCT_PS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_PSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_CS:
		{
			hr = createShader(wszName, CPRS_SCT_CS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_CSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;
		}
	}

	return true;
}

bool CPGCShaderManager::loadCSOScript(std::wstring wszLoadFilepath)
{
	std::wifstream wifp;

	wifp.open(wszLoadFilepath.c_str(), std::ios::in);
	if (!wifp) { return false; }

	std::wstring temp;
	std::wstring type;
	std::wstring shadername;
	std::wstring filepath;

	CPRS_SHADER_COMPILE_TYPE shaderType;

	HRESULT hr = S_OK;

	wifp >> temp;

	if(temp != L"#END")
	{
		while (true)
		{
			wifp >> type;
			if (type == L"#END") { break; }
			else
			{
				if (type == L"VS") { shaderType = CPRS_SCT_VS; }
				if (type == L"HS") { shaderType = CPRS_SCT_HS; }
				if (type == L"DS") { shaderType = CPRS_SCT_DS; }
				if (type == L"GS") { shaderType = CPRS_SCT_GS; }
				if (type == L"PS") { shaderType = CPRS_SCT_PS; }
				if (type == L"CS") { shaderType = CPRS_SCT_CS; }

				wifp >> shadername;
				wifp >> filepath;

				switch (shaderType)
				{
				case CPRS_SCT_VS:
				{
					hr = createByteCodeShader(shadername, CPRS_SCT_VS, filepath);
					if (SUCCEEDED(hr))
					{
						m_VSCsoMap.insert(std::make_pair(shadername, filepath));
					}

				} break;

				case CPRS_SCT_HS:
				{
					hr = createByteCodeShader(shadername, CPRS_SCT_HS, filepath);
					if (SUCCEEDED(hr))
					{
						m_HSCsoMap.insert(std::make_pair(shadername, filepath));
					}

				} break;

				case CPRS_SCT_DS:
				{
					hr = createByteCodeShader(shadername, CPRS_SCT_DS, filepath);
					if (SUCCEEDED(hr))
					{
						m_DSCsoMap.insert(std::make_pair(shadername, filepath));
					}

				} break;

				case CPRS_SCT_GS:
				{
					hr = createByteCodeShader(shadername, CPRS_SCT_GS, filepath);
					if (SUCCEEDED(hr))
					{
						m_GSCsoMap.insert(std::make_pair(shadername, filepath));
					}

				} break;

				case CPRS_SCT_PS:
				{
					hr = createByteCodeShader(shadername, CPRS_SCT_PS, filepath);
					if (SUCCEEDED(hr))
					{
						m_PSCsoMap.insert(std::make_pair(shadername, filepath));
					}

				} break;

				case CPRS_SCT_CS:
				{
					hr = createByteCodeShader(shadername, CPRS_SCT_CS, filepath);
					if (SUCCEEDED(hr))
					{
						m_CSCsoMap.insert(std::make_pair(shadername, filepath));
					}

				} break;
				}
			}
		}
	}

	return true;
}

bool CPGCShaderManager::loadCSODirectory(std::wstring wszDirPath)
{
	std::vector<std::wstring> InExtArr;
	std::vector<std::wstring> OutPath;
	std::vector<std::wstring> OutName;

	InExtArr.push_back(L".CSO");
	InExtArr.push_back(L".cso");

	bool ret = loadDir(CPMAC_DEFAULT_CSO_SHADER_DIRPATH, &InExtArr,
		&OutPath,
		&OutName);

	if (!ret) { return false; }

	CPRS_SHADER_COMPILE_TYPE shaderType;

	HRESULT hr = S_OK;

	std::wstring wszType;
	std::wstring wszName;
	std::wstring wszPath;

	for (int i = 0; i < OutPath.size(); i++)
	{
		wszType.assign(OutName[i].begin(), OutName[i].begin() + 2);

		if (wszType == L"VS") { shaderType = CPRS_SCT_VS; }
		if (wszType == L"HS") { shaderType = CPRS_SCT_HS; }
		if (wszType == L"DS") { shaderType = CPRS_SCT_DS; }
		if (wszType == L"GS") { shaderType = CPRS_SCT_GS; }
		if (wszType == L"PS") { shaderType = CPRS_SCT_PS; }
		if (wszType == L"CS") { shaderType = CPRS_SCT_CS; }

		wszName.assign(OutName[i].begin(), OutName[i].end()-4);

		switch (shaderType)
		{
		case CPRS_SCT_VS:
		{
			hr = createByteCodeShader(wszName, CPRS_SCT_VS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_VSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_HS:
		{
			hr = createByteCodeShader(wszName, CPRS_SCT_HS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_HSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_DS:
		{
			hr = createByteCodeShader(wszName, CPRS_SCT_DS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_DSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_GS:
		{
			hr = createByteCodeShader(wszName, CPRS_SCT_GS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_GSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_PS:
		{
			hr = createByteCodeShader(wszName, CPRS_SCT_PS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_PSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;

		case CPRS_SCT_CS:
		{
			hr = createByteCodeShader(wszName, CPRS_SCT_CS, OutPath[i]);
			if (SUCCEEDED(hr))
			{
				m_CSCsoMap.insert(std::make_pair(wszName, OutPath[i]));
			}

		} break;
		}
	}

	return true;
}

bool CPGCShaderManager::saveHLSLScript(std::wstring wszSaveFilepath)
{
	std::wofstream wofp;

	wofp.open(wszSaveFilepath.c_str(), std::ios::out);
	if (!wofp) { return false; }

	wofp << L"##HLSL_File_List\n";

	int cnt = 0;
	for (auto it : m_VSHlslMap)
	{
		wofp << L"VS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_HSHlslMap)
	{
		wofp << L"HS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_DSHlslMap)
	{
		wofp << L"DS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_GSHlslMap)
	{
		wofp << L"GS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_PSHlslMap)
	{
		wofp << L"PS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_CSHlslMap)
	{
		wofp << L"CS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	wofp << L"#END";

	return true;
}

bool CPGCShaderManager::saveCSOScript(std::wstring wszSaveFilepath)
{
	std::wofstream wofp;

	wofp.open(wszSaveFilepath.c_str(), std::ios::out);
	if (!wofp) { return false; }

	wofp << L"##CSO_File_List\n";

	int cnt = 0;
	for (auto it : m_VSCsoMap)
	{
		wofp << L"VS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_HSCsoMap)
	{
		wofp << L"HS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_DSCsoMap)
	{
		wofp << L"DS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_GSCsoMap)
	{
		wofp << L"GS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_PSCsoMap)
	{
		wofp << L"PS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	cnt = 0;
	for (auto it : m_CSCsoMap)
	{
		wofp << L"CS" << L"\t" << it.first << L"\t" << it.second << L'\n';
		cnt++;
	}

	wofp << L"#END";

	return true;
}

void CPGCShaderManager::createPresetShaderGroups()
{
	//DefaultShaderList.txt
	//loadHLSLScript(L"../../data/shader/script/DefaultHlslList.txt");
	loadCSODirectory(CPMAC_DEFAULT_CSO_SHADER_DIRPATH);

	saveCSOScript(CPMAC_DEFAULT_CSO_SHADER_SCRIPT_FILEPATH);
}
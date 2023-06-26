//////////////////////////////////////////////////
//
// CPShaderManager.h
//		쉐이더 통합 관리 매니저
//		쉐이더 컴파일은 D3DCompileFromFile()함수를 사용해 런타임 환경에서 수행한다.
//		사전 컴파일된 쉐이더 코드를 활용하는 방법도 있다.
//		FXC 또는 최신 쉐이더 언어 버전을 반영하는 DXC를 이용해 오프라인 컴파일을 수행하는 방법이 있다.
//		프로그래밍 가능한 Shader단계에는 다음의 리소스 바인딩 한계를 가진다.
//			SamplerState : 16개
//			ConstantBuffer : 15개, 버퍼당 상수 변수 4,096개
//			
//  
//////////////////////////////////////////////////

#pragma once
#include "CPRSBaseStruct.h"
#include "CPGCDxStateManager.h"
#include "CPGCFileIO.h"

#define CPRS_DEFAULT_SHADER_LANG_VERSION		CPGC_SHADER_LANG_VERSION

#define CPRS_VS_ENTRYPOINT						"VS"
#define CPRS_VS_TARGET							"vs"

#define CPRS_HS_ENTRYPOINT						"HS"
#define CPRS_HS_TARGET							"hs"

#define CPRS_DS_ENTRYPOINT						"DS"
#define CPRS_DS_TARGET							"ds"

#define CPRS_GS_ENTRYPOINT						"GS"
#define CPRS_GS_TARGET							"gs"

#define CPRS_PS_ENTRYPOINT						"PS"
#define CPRS_PS_TARGET							"ps"

#define CPRS_CS_ENTRYPOINT						"CS"
#define CPRS_CS_TARGET							"cs"

#define CPMAC_DEFAULT_CSO_SHADER_DIRPATH			L"../../data/shader/cso/"
#define CPMAC_DEFAULT_CSO_SHADER_SCRIPT_FILEPATH	L"../../data/shader/script/DefaultCsoList.txt"

enum CPRS_SHADER_COMPILE_TYPE
{
	CPRS_SCT_VS = 0,
	CPRS_SCT_HS,
	CPRS_SCT_DS,
	CPRS_SCT_GS,
	CPRS_SCT_GS_SO,
	CPRS_SCT_PS,
	CPRS_SCT_CS,
	CPRS_SCT_NUMBER_OF_TYPE
};

class CPGCShaderManager
{
private:
	ID3D11Device*															m_pDevice;

	std::set<std::wstring>													m_scriptFilenameSet;

	std::map<std::wstring, std::wstring>									m_VSHlslMap;
	std::map<std::wstring, std::wstring>									m_HSHlslMap;
	std::map<std::wstring, std::wstring>									m_DSHlslMap;
	std::map<std::wstring, std::wstring>									m_GSHlslMap;
	std::map<std::wstring, std::wstring>									m_PSHlslMap;
	std::map<std::wstring, std::wstring>									m_CSHlslMap;

	std::map<std::wstring, std::wstring>									m_GS_SOHlslMap;

	std::map<std::wstring, std::wstring>									m_VSCsoMap;
	std::map<std::wstring, std::wstring>									m_HSCsoMap;
	std::map<std::wstring, std::wstring>									m_DSCsoMap;
	std::map<std::wstring, std::wstring>									m_GSCsoMap;
	std::map<std::wstring, std::wstring>									m_PSCsoMap;
	std::map<std::wstring, std::wstring>									m_CSCsoMap;

	std::map<std::wstring, std::wstring>									m_GS_SOCsoMap;

	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11VertexShader>>		m_VSMap;
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11HullShader>>		m_HSMap;
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11DomainShader>>		m_DSMap;
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11GeometryShader>>	m_GSMap;
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11PixelShader>>		m_PSMap;
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ComputeShader>>		m_CSMap;

	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11GeometryShader>>	m_GS_SOMap;

	std::map<std::wstring, CPRS_ShaderGroup>								m_shaderPresetMap;

	std::vector<std::pair <std::string, std::string>>						m_compSet;

	HRESULT		compileShader(	std::wstring wszFileName,
								CPRS_SHADER_COMPILE_TYPE compileType,
								void* pShader,
								CPRS_SO_OPTION* pSOOpt = nullptr);

	HRESULT		createShader_Internal(	void* pData, 
										size_t iSize, 
										CPRS_SHADER_COMPILE_TYPE compileType,
										void* pShader,
										CPRS_SO_OPTION* pSOOpt = nullptr);

	std::vector<BYTE> loadCsoFile(std::wstring loadFilepath);

	CPGCShaderManager();
	~CPGCShaderManager();

public:
	CPGCShaderManager(const CPGCShaderManager& other) = delete;
	CPGCShaderManager& operator=(const CPGCShaderManager& other) = delete;
public:
	bool		init(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pDContext);
	bool		release();

	HRESULT	createShader(std::wstring wszShaderName,
		CPRS_SHADER_COMPILE_TYPE shaderType,
		std::wstring wszFilepath,
		CPRS_SO_OPTION* pSOOpt = nullptr);

	HRESULT createByteCodeShader(std::wstring wszShaderName,
		CPRS_SHADER_COMPILE_TYPE shaderType,
		std::wstring wszByteCodeFilepath,
		CPRS_SO_OPTION* pSOOpt = nullptr);

	ID3D11VertexShader*		getVS(std::wstring wszShaderName);
	ID3D11HullShader*		getHS(std::wstring wszShaderName);
	ID3D11DomainShader*		getDS(std::wstring wszShaderName);
	ID3D11GeometryShader*	getGS(std::wstring wszShaderName);
	ID3D11GeometryShader*	getGS_SO(std::wstring wszShaderName);
	ID3D11PixelShader*		getPS(std::wstring wszShaderName);
	ID3D11ComputeShader*	getCS(std::wstring wszShaderName);

	bool					getShaderPreset(std::wstring wsaShaderGroupName, CPRS_ShaderGroup& shaderGroup);

	void					makeShaderGroup(std::wstring VS,
											std::wstring HS,
											std::wstring DS,
											std::wstring GS,
											std::wstring PS,
											std::wstring CS, 
											CPRS_ShaderGroup& shaderGroup);

	bool loadHLSLScript(std::wstring wszLoadFilepath);
	bool loadHLSLDirectory(std::wstring wszDirPath);
	bool loadCSOScript(std::wstring wszLoadFilepath);
	bool loadCSODirectory(std::wstring wszDirPath);

	bool saveHLSLScript(std::wstring wszSaveFilepath);
	bool saveCSOScript(std::wstring wszSaveFilepath);

	void createPresetShaderGroups();

	//전역 인스턴스 반환
	static CPGCShaderManager& getInstance()
	{
		static CPGCShaderManager singleInst;
		return singleInst;
	}
};

#define CPGC_SHADER_MGR CPGCShaderManager::getInstance()


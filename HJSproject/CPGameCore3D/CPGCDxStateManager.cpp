#include "CPGCDxStateManager.h"

CPGCDXStateManager::CPGCDXStateManager()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;
}

CPGCDXStateManager::~CPGCDXStateManager()
{
	
}

bool CPGCDXStateManager::init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	if (!pDevice || !pDContext) { return false; }
	else 
	{ 
		m_pDevice = pDevice;  
		m_pDContext = pDContext;
	}

	HRESULT hr = S_OK;

	hr = createDefaultStates();

	return true;
}

bool CPGCDXStateManager::release()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;

	for (auto it : m_SStateMap)
	{
		it.second->Release();
		it.second = nullptr;
	}
	m_SStateMap.clear();

	for (auto it : m_RStateMap)
	{
		it.second->Release();
		it.second = nullptr;
	}
	m_RStateMap.clear();

	for (auto it : m_BStateMap)
	{
		it.second->Release();
		it.second = nullptr;
	}
	m_BStateMap.clear();

	for (auto it : m_DSStateMap)
	{
		it.second->Release();
		it.second = nullptr;
	}
	m_DSStateMap.clear();

	return true;
}

HRESULT CPGCDXStateManager::createSState(std::wstring wszSStateName, D3D11_SAMPLER_DESC samplerDesc)
{
	HRESULT hr = S_OK;

	if (m_SStateMap.find(wszSStateName) == m_SStateMap.end())
	{
		ID3D11SamplerState* newE;

		hr = m_pDevice->CreateSamplerState(&samplerDesc, &newE);
		if (SUCCEEDED(hr)) { m_SStateMap.insert(std::make_pair(wszSStateName, newE)); }
		else { printCreateErr(hr); }
	}
	else { hr = S_FALSE; }

	return hr;
}

HRESULT CPGCDXStateManager::createRState(std::wstring wszRStateName, D3D11_RASTERIZER_DESC rasterizerDesc)
{
	HRESULT hr = S_OK;

	if (m_RStateMap.find(wszRStateName) == m_RStateMap.end())
	{
		ID3D11RasterizerState* newE;

		hr = m_pDevice->CreateRasterizerState(&rasterizerDesc, &newE);
		if (SUCCEEDED(hr)) { m_RStateMap.insert(std::make_pair(wszRStateName, newE)); }
		else { printCreateErr(hr); }
	}
	else { hr = S_FALSE; }

	return hr;
}

HRESULT CPGCDXStateManager::createBState(std::wstring wszBStateName, D3D11_BLEND_DESC blendDesc)
{
	HRESULT hr = S_OK;

	if (m_BStateMap.find(wszBStateName) == m_BStateMap.end())
	{
		ID3D11BlendState* newE;

		hr = m_pDevice->CreateBlendState(&blendDesc, &newE);
		if (SUCCEEDED(hr)) { m_BStateMap.insert(std::make_pair(wszBStateName, newE)); }
		else { printCreateErr(hr); }
	}
	else { hr = S_FALSE; }

	return hr;
}

HRESULT CPGCDXStateManager::createDSState(std::wstring wszDSStateName, D3D11_DEPTH_STENCIL_DESC depthStencilDesc)
{
	HRESULT hr = S_OK;

	if (m_DSStateMap.find(wszDSStateName) == m_DSStateMap.end())
	{
		ID3D11DepthStencilState* newE;

		hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &newE);
		if (SUCCEEDED(hr)) { m_DSStateMap.insert(std::make_pair(wszDSStateName, newE)); }
		else { printCreateErr(hr); }
	}
	else { hr = S_FALSE; }

	return hr;
}

ID3D11SamplerState* CPGCDXStateManager::getSState(std::wstring wszSStateName)
{
	auto it = m_SStateMap.find(wszSStateName);
	if (it != m_SStateMap.end()) { return it->second; }

	return nullptr;
}

ID3D11RasterizerState* CPGCDXStateManager::getRState(std::wstring wszRStateName)
{
	auto it = m_RStateMap.find(wszRStateName);
	if (it != m_RStateMap.end()) { return it->second; }

	return nullptr;
}

ID3D11BlendState* CPGCDXStateManager::getBState(std::wstring wszBStateName)
{
	auto it = m_BStateMap.find(wszBStateName);
	if (it != m_BStateMap.end()) { return it->second; }

	return nullptr;
}

ID3D11DepthStencilState* CPGCDXStateManager::getDSState(std::wstring wszDSStateName)
{
	auto it = m_DSStateMap.find(wszDSStateName);
	if (it != m_DSStateMap.end()) { return it->second; }

	return nullptr;
}
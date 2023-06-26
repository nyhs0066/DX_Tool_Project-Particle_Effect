#include "CPGCTextRenderer.h"

CPGCTextRenderer::CPGCTextRenderer()
{

}

CPGCTextRenderer::~CPGCTextRenderer()
{
}

HRESULT CPGCTextRenderer::createD2D1Factory()
{
	HRESULT hr = S_OK;

	//D2D1_FACTORY_TYPE : 팩토리의 스레드 옵션 타입
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_pD2Factory.GetAddressOf());

	return hr;
}

HRESULT CPGCTextRenderer::createD2D1RenderTarget(IDXGISwapChain* pSwapchain)
{
	HRESULT hr = S_OK;

	if (!pSwapchain) { hr = E_POINTER; return hr; }

	Microsoft::WRL::ComPtr<IDXGISurface>			pBackBuf;

	hr = pSwapchain->GetBuffer(0, __uuidof(IDXGISurface), (void**)pBackBuf.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		D2D1_RENDER_TARGET_PROPERTIES rtp;

		rtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		rtp.pixelFormat.format = DXGI_FORMAT_UNKNOWN;
		rtp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		rtp.dpiX = 96;
		rtp.dpiY = 96;
		rtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
		rtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

		hr = m_pD2Factory->CreateDxgiSurfaceRenderTarget(
			pBackBuf.Get(),
			&rtp,
			m_pD2RenderTarget.GetAddressOf());
	}

	return hr;
}

HRESULT CPGCTextRenderer::createDWriteFactory()
{
	HRESULT hr = S_OK;

	//DWRITE_FACTORY_TYPE : 팩토리의 프로세스간 공유 옵션 타입
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		(IUnknown**)m_pDWFactory.GetAddressOf());

	return hr;
}

HRESULT CPGCTextRenderer::createDefaultDWriteTextFormats()
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<IDWriteTextFormat> newFormat;

	hr = m_pDWFactory->CreateTextFormat(
		L"consolas",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		15,
		L"en-us",
		newFormat.GetAddressOf());

	m_DWTextFormats.push_back(newFormat);

	return hr;
}

HRESULT CPGCTextRenderer::createDefaultSolidColorBrushs()
{
	HRESULT hr = S_OK;

	D2D1_COLOR_F colors[] =
	{
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.5f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.25f, 0.25f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.75f, 1.0f },
		{ 0.5f, 0.0f, 0.75f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
	};

	for (auto color : colors)
	{
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> newE;
		hr = m_pD2RenderTarget->CreateSolidColorBrush(color, newE.GetAddressOf());
		m_D2colorBrushs.push_back(newE);
	}

	return hr;
}

HRESULT CPGCTextRenderer::createDefaultDWriteTextLayouts()
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<IDWriteTextLayout> newE;

	std::wstring defaultString = L"테스트 텍스트";
	hr = m_pDWFactory->CreateTextLayout(defaultString.c_str(),
		defaultString.size(),
		m_DWTextFormats[0].Get(),
		100.0f, 100.0f,
		newE.GetAddressOf());

	m_pDWTextLayouts.push_back(newE);

	return hr;
}

HRESULT CPGCTextRenderer::create(IDXGISwapChain* pSwapchain)
{
	HRESULT hr = S_OK;

	hr = createD2D1Factory();
	if (FAILED(hr)) { return hr; }

	hr = createDWriteFactory();
	if (FAILED(hr)) { return hr; }

	hr = createD2D1RenderTarget(pSwapchain);
	if (FAILED(hr)) { return hr; }

	hr = createDefaultDWriteTextFormats();
	if (FAILED(hr)) { return hr; }

	hr = createDefaultDWriteTextLayouts();
	if (FAILED(hr)) { return hr; }

	hr = createDefaultSolidColorBrushs();
	if (FAILED(hr)) { return hr; }

	return hr;
}

HRESULT CPGCTextRenderer::addTextFormat(WCHAR* wszFontFamilyName, 
	WCHAR* localeName, 
	FLOAT fFontSize,
	DWRITE_FONT_WEIGHT weight,
	DWRITE_FONT_STYLE style,
	DWRITE_FONT_STRETCH stretch)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<IDWriteTextFormat> newE;

	hr = m_pDWFactory->CreateTextFormat(
		wszFontFamilyName,
		nullptr,
		weight,
		style,
		stretch,
		fFontSize,
		localeName,
		newE.GetAddressOf());

	if (SUCCEEDED(hr)) { m_DWTextFormats.push_back(newE); }
	
	return hr;
}

HRESULT CPGCTextRenderer::addColorBrush(D2D1_COLOR_F color)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> newE;
	hr = m_pD2RenderTarget->CreateSolidColorBrush(color, newE.GetAddressOf());

	if (SUCCEEDED(hr)) { m_D2colorBrushs.push_back(newE); }

	return hr;
}

HRESULT CPGCTextRenderer::addTextLayout(IDWriteTextFormat* pFormat, WCHAR* wszText, FLOAT fMaxWidth, FLOAT fMaxHeight)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<IDWriteTextLayout> newE;

	std::wstring defaultString = L"테스트 텍스트";
	hr = m_pDWFactory->CreateTextLayout(wszText,
		wcslen(wszText),
		pFormat,
		fMaxWidth, fMaxHeight,
		newE.GetAddressOf());

	if (SUCCEEDED(hr)) { m_pDWTextLayouts.push_back(newE); }

	return hr;
}

bool CPGCTextRenderer::init()
{
	return true;
}

bool CPGCTextRenderer::release()
{
	m_D2colorBrushs.clear();
	m_DWTextFormats.clear();
	m_pDWTextLayouts.clear();

	return true;
}

bool CPGCTextRenderer::resizeComponent(UINT iWidth, UINT iHeight, IDXGISwapChain* pSwapchain)
{
	releaseDependentResource();

	createDependentResource(pSwapchain);

	return true;
}

HRESULT CPGCTextRenderer::createDependentResource(IDXGISwapChain* pSwapchain)
{
	HRESULT hr = S_OK;

	hr = createD2D1RenderTarget(pSwapchain);
	if (FAILED(hr)) { return hr; }

	hr = createDefaultSolidColorBrushs();
	if (FAILED(hr)) { return hr; }

	return hr;
}

bool CPGCTextRenderer::releaseDependentResource()
{
	m_D2colorBrushs.clear();

	if (m_pD2RenderTarget.Get())
	{
		m_pD2RenderTarget.ReleaseAndGetAddressOf();
	}

	return true;
}

void CPGCTextRenderer::drawT(std::wstring wszString, FLOAT stX, FLOAT stY, UINT iFormatIdx, UINT iColorIdx)
{
	if (m_D2colorBrushs.size() && m_DWTextFormats.size())
	{
		UINT iColor = (iColorIdx % m_D2colorBrushs.size());
		UINT iFormat = (iFormatIdx % m_DWTextFormats.size());

		D2D1_RECT_F position = { stX , stY , stX + 500.0f, stY + 500.0f };

		m_pD2RenderTarget->BeginDraw();

		m_pD2RenderTarget->DrawText(wszString.c_str(),
			wszString.size(),
			m_DWTextFormats[iFormat].Get(),
			position,
			m_D2colorBrushs[iColor].Get());

		m_pD2RenderTarget->EndDraw();
	}
}

void CPGCTextRenderer::drawTLayout(UINT iLayoutIdx, UINT iColorIdx)
{
	if (m_D2colorBrushs.size() && m_pDWTextLayouts.size())
	{
		UINT iColor = (iColorIdx % m_D2colorBrushs.size());
		UINT iLayout = (iLayoutIdx % m_pDWTextLayouts.size());

		m_pD2RenderTarget->BeginDraw();

		m_pD2RenderTarget->DrawTextLayout({ 0.0f, 0.0f },
			m_pDWTextLayouts[iLayout].Get(),
			m_D2colorBrushs[iColor].Get());

		m_pD2RenderTarget->EndDraw();
	}
}
ID2D1Factory* CPGCTextRenderer::getD2D1Factory()
{
	return m_pD2Factory.Get();
}
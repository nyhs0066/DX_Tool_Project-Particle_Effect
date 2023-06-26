//////////////////////////////////////////////////
//
// CPGCTextRenderer.h
//		텍스트를 렌더링 하기 위한 헤더
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"

//DX 텍스트 렌더
#include <d2d1.h>		//DX 2D
#include <dwrite.h>		//Direct Write

using D2TextColorBrushs = std::vector<Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>>;
using DWTextFormats = std::vector<Microsoft::WRL::ComPtr<IDWriteTextFormat>>;
using DWTextLayouts = std::vector<Microsoft::WRL::ComPtr<IDWriteTextLayout>>;

enum CPGC_TEXT_COLOR
{
	CPGC_TC_BLACK = 0,
	CPGC_TC_RED,
	CPGC_TC_ORANGE,
	CPGC_TC_YELLOW,
	CPGC_TC_GREEN,
	CPGC_TC_BLUE,
	CPGC_TC_INDIGO,
	CPGC_TC_PURPLE,
	CPGC_TC_WHITE,
	CPGC_TC_NUMBER_OF_COLOR
};

enum CPGC_TEXT_FORMAT
{
	CPGC_TFORM_CONSOLAS = 0,
	CPGC_TFORM_NUMBER_OF_FORMAT
};

class CPGCTextRenderer
{
private:
	//화면에 그리기 위한 Direct 2D 인터페이스
	Microsoft::WRL::ComPtr<ID2D1Factory>			m_pD2Factory;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget>		m_pD2RenderTarget;

	//텍스트 색상 브러시 리스트
	D2TextColorBrushs								m_D2colorBrushs;

	//텍스트 관련 인터페이스 생성
	Microsoft::WRL::ComPtr<IDWriteFactory>			m_pDWFactory;

	//텍스트 포맷 리스트 : 고정 형식 - 가변 워딩 
	DWTextFormats									m_DWTextFormats;

	//텍스트 레이아웃 리스트 : 고정 워딩, 가변 형식
	DWTextLayouts									m_pDWTextLayouts;

	CPGCTextRenderer();
	~CPGCTextRenderer();

	//create 보조 함수들
	HRESULT createD2D1Factory();
	HRESULT createD2D1RenderTarget(IDXGISwapChain* pSwapchain);
	HRESULT createDWriteFactory();
	HRESULT createDefaultDWriteTextFormats();
	HRESULT createDefaultSolidColorBrushs();
	HRESULT createDefaultDWriteTextLayouts();

public:
	CPGCTextRenderer(const CPGCTextRenderer& other) = delete;
	CPGCTextRenderer& operator=(const CPGCTextRenderer& other) = delete;

public:
	bool init();
	bool release();

	HRESULT create(IDXGISwapChain* pSwapchain);

	//텍스트 포맷 / 색상 / 레이아웃 추가부
	HRESULT addTextFormat(	WCHAR* wszFontFamilyName,
							WCHAR* localeName,
							FLOAT fFontSize,
							DWRITE_FONT_WEIGHT weight,
							DWRITE_FONT_STYLE style,
							DWRITE_FONT_STRETCH stretch);
	HRESULT addColorBrush(D2D1_COLOR_F color);
	HRESULT addTextLayout(	IDWriteTextFormat* pFormat, 
							WCHAR* wszText, 
							FLOAT fMaxWidth, 
							FLOAT fMaxHeight);

	ID2D1Factory* getD2D1Factory();

	//크기 의존 요소들 재조정 작업
	bool resizeComponent(UINT iWidth, UINT iHeight, IDXGISwapChain* pSwapchain);
	HRESULT createDependentResource(IDXGISwapChain* pSwapchain);
	bool releaseDependentResource();

	void drawT(std::wstring wszString, FLOAT stX, FLOAT stY, UINT iFormatIdx, UINT iColorIdx);
	void drawTLayout(UINT iLayoutIdx, UINT iColorIdx);

	static CPGCTextRenderer& getInstance()
	{
		static CPGCTextRenderer singleInst;
		return singleInst;
	}
};

#define CPGC_TEXTRENDERER CPGCTextRenderer::getInstance()

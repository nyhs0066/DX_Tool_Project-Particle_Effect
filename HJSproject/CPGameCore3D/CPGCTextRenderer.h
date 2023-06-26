//////////////////////////////////////////////////
//
// CPGCTextRenderer.h
//		�ؽ�Ʈ�� ������ �ϱ� ���� ���
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"

//DX �ؽ�Ʈ ����
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
	//ȭ�鿡 �׸��� ���� Direct 2D �������̽�
	Microsoft::WRL::ComPtr<ID2D1Factory>			m_pD2Factory;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget>		m_pD2RenderTarget;

	//�ؽ�Ʈ ���� �귯�� ����Ʈ
	D2TextColorBrushs								m_D2colorBrushs;

	//�ؽ�Ʈ ���� �������̽� ����
	Microsoft::WRL::ComPtr<IDWriteFactory>			m_pDWFactory;

	//�ؽ�Ʈ ���� ����Ʈ : ���� ���� - ���� ���� 
	DWTextFormats									m_DWTextFormats;

	//�ؽ�Ʈ ���̾ƿ� ����Ʈ : ���� ����, ���� ����
	DWTextLayouts									m_pDWTextLayouts;

	CPGCTextRenderer();
	~CPGCTextRenderer();

	//create ���� �Լ���
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

	//�ؽ�Ʈ ���� / ���� / ���̾ƿ� �߰���
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

	//ũ�� ���� ��ҵ� ������ �۾�
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

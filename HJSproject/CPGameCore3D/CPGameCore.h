//////////////////////////////////////////////////
//
// CPGameCore.h
//		게임 프로젝트 제작을 위한 프레임 워크 헤더
//		CPGC_Object에 포함된 헤더
//			Timer
//			Input
//			Mesh
//				Vertex : VLayout
//			Material
//				Shader
//				Texture
//				DXState
//			Shape
//				BaseStruct
//			Sound
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCWindow.h"
#include "CPGCDxDevice.h"
#include "CPGCObject.h"
#include "CPGCTextRenderer.h"
#include "CPGCIMGUIModule.h"
#include "CPGCRenderTarget.h"
#include "CPGCCamera.h"
#include "CPGCSpriteManager.h"
#include "CPGCSoundManager.h"
#include "CPGCFBXLoader.h"
#include "CPGCEffectManager.h"
#include "CPGCRenderTarget.h"

class CPGameCore : public CPGCWindow
{
private:
	ID3D11Device*							m_pDevice;
	ID3D11DeviceContext*					m_pImDContext;

	UINT									m_iPrevWndWidth;
	UINT									m_iPrevWndHeight;

	D3D11_VIEWPORT							m_mainViewPort;

	CPGCActor								m_quad;
	
	int										m_iRenderTargetNum;
	int										m_iSRVNum;
	std::vector<CPGCRenderTarget*>			m_pMRT;

	//0 : COLOR(With Depth-Stencil) / 1 : Not Used / 2 : NORMAL / 3 : COLOR_STENCIL
	std::vector<ID3D11RenderTargetView*>	m_pRTVArray;

	//0 : COLOR / 1 : Not Used / 2 : NORMAL / 3 : COLOR_STENCIL / 4 : DEPTH_STENCIL
	std::vector<ID3D11ShaderResourceView*>	m_pSRVArray;

	//코어 프로세스군
	bool coreInit();

	bool coreUpdate();
	bool corePreUpdate();
	bool corePostUpdate();

	bool coreRender();
	bool corePreRender();
	bool corePostRender();

	bool coreRelease();

	void clearMRTs();
	void bindMRTs();

protected:
	HRESULT createMRTs(UINT iWidth, UINT iHeight);
	void releaseMRTs();

	//크기 의존 구성요소 재조정 작업
	bool resizeComponent(UINT iWidth, UINT iHeight);

	//전체화면 전환
	void toggleFullscreen();

public:
	CPGameCore(const WCHAR* wszWndName, const WCHAR* wszWcName);
	virtual ~CPGameCore();

	virtual bool init();

	virtual bool update();
	virtual bool preUpdate();
	virtual bool postUpdate();

	virtual bool render();
	virtual bool preRender();
	virtual bool postRender();

	virtual bool release();

	virtual int run();

	virtual LRESULT msgHandler(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};


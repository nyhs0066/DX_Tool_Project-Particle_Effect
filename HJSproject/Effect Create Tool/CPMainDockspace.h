/////////////////////////////////////////////////////////////////////////////////
//	CPMainDockspace.h
//		메인 도킹 스페이스 헤더
//		
//		ImGui::Separator()				구분선 삽입
//		ImGui::SeparatorText()			캡션이 있는 구분선 삽입
//		ImGui::SameLine()				같은 줄 배치
//		ImGui::Indent()					들여쓰기
//      ImGui::SameLine()				같은 라인 배치
//		HelpMarker()					도움 설명 표시
//		ImGui::GetTextLineHeight()		텍스트 줄 높이
//		ImGui::IsMouseDoubleClicked(0)	더블클릭 감지
//		
//		이름 생성부에 ##은 같은 이름 또는 이름이 없는 ImGui객체를 식별하기 위한 추가 스트링부를 지정한다.
//		모든 라벨은 고유해야한다.
//		
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CPEffectWorkspace.h"
#include "CPMapWorkspace.h"
#include "CPPicker.h"
#include "CPGCFBXLoader.h"

#define CPGC_MAINDOCKSPACE_SETTING_FILE_PATH L"../../data/setting/MyTool.txt"

#define DEFAULT_OLD_EFFECT_DIRECTORY_WSZPATH	L"../../data/save/particlesystem/old/"
#define DEFAULT_OLD_EFFECT_DIRECTORY_SZPATH		"../../data/save/particlesystem/old/"

#define DEFAULT_NEW_EFFECT_DIRECTORY_WSZPATH	L"../../data/save/particlesystem/new/"
#define DEFAULT_NEW_EFFECT_DIRECTORY_SZPATH		"../../data/save/particlesystem/new/"

enum CPIMGUI_EFFECTFILE_LOAD_OPT
{
	LOPT_CURRENT_VER,
	LOPT_PREV_VER,
	NUMBER_OF_LOAD_OPT
};

class CPMainDockspace
{
private:
	ID3D11Device*									m_pDevice;
	ID3D11DeviceContext*							m_pDContext;
	CPGCCamera*										m_pMainCam;

	//DX Render Target
	CPGCRenderTarget*								m_pMainRT;
	D3D11_VIEWPORT									m_prevMainRTVPort;
	D3D11_VIEWPORT									m_mainRTVPort;
	UINT											m_RTWindowOffsetY;

	bool											m_bResizeFlag;	//렌더 뷰 크기 재조정 플래그
	bool											m_bFullscreen;	//아직 미사용

	//전역 Dx State 사용설정 여부
	//RS
	bool											m_bEnableWireFrame;		//와이어 프레임

	//워크 스페이스
	CPEffectWorkspace*								m_pEffectWSpace;
	CPMapWorkspace*									m_pMapWSpace;

	//ImGui 윈도우 함수
	bool											m_bMainShow;
	bool											m_bOpenDXRTWindow;
	bool											m_bOpenDemoWindow;

	bool											m_bOpenEffectSystemWindow;
	bool											m_bOpenMapWindow;

	int												m_iShowEffectPopup;
	int												m_iShowMapPopup;

	CPPicker										m_picker;
	bool											m_isMapPicked;

	int												m_testNodeIdx;
	CPM_Vector3										m_vTestPickingPos;

	CPMainDockspace();
	~CPMainDockspace();

public:
	CPMainDockspace(const CPMainDockspace& other) = delete;
	CPMainDockspace& operator=(const CPMainDockspace& other) = delete;
public:
	bool init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);

	bool preUpdate();
	bool update();
	bool postUpdate();

	bool preRender();
	bool render();
	bool postRender();

	void effectRender();

	bool release();

	bool resizeComponent(UINT iWidth, UINT iHeight);
	bool setMainCam(CPGCCamera* pMainCam);
	bool getResizeFlag();

	void showMainDockSpace();
	void showStatusWindow();
	void showDXRTWindow();

	//이펙트 도구부
	void showEffectCreateWindow();

	//지형 생성부
	void showMapCreateWindow();

	//배치 오브젝트 및 FBX모델부

	//UI부

	//피킹 작업 부
	bool getIntersectionInMap();

	static CPMainDockspace& getInstance()
	{
		static CPMainDockspace singleInst;
		return singleInst;
	}
};

#define MAIN_DOCKSPACE	CPMainDockspace::getInstance()

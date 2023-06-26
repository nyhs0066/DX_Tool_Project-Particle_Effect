//////////////////////////////////////////////////
//
// CPGCWindow.h
//		어플리케이션 창 생성 헤더
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"
#include "CPGCIMGUIModule.h"
#include "ImGUI/imgui_impl_win32.h"

//사용자 Window class 이름 매크로
#define CP_CUSTOM_WND0		L"CP_mainWnd"

//Wheel 입력시 마우스 위치 추출
#define EXTRACT_LPARAM_X(lParam) ((LONG)(lParam & 0xffff))
#define EXTRACT_LPARAM_Y(lParam) ((LONG)((lParam >> 16) & 0xffff))


struct CPGC_MOUSE_WHEEL_STATE
{
	bool	bMoveWheel;
	int		iWheelDt;
	DWORD	dwExtraKeyInput;
	POINT	pt;
};

class CPGCWindow
{
private:
	//등록할 창 클래스 이름 / 창 이름
	const WCHAR* m_wszWcName;
	const WCHAR* m_wszWndName;

public:
	//창의 핸들
	static HWND m_hwnd;

	//창의 너비 / 높이
	static int	m_wndWidth;
	static int	m_wndHeight;

	//창의 최소화 플래그
	static bool	m_bCollapseFlag;

	//휠 상태 구조체
	static CPGC_MOUSE_WHEEL_STATE m_wheelState;

	//인스턴스 핸들
	static HINSTANCE m_hInstance;

	static bool bDxCreateFlag;

	//기반 클래스 창 프로시저
	static LRESULT __stdcall wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CPGCWindow(const WCHAR* wszWndName, const WCHAR* wszWcName = nullptr);
	virtual ~CPGCWindow();

	//창 이름 가져오기
	const WCHAR* getWndName();

	//창 생성 및 화면 출력
	bool createWndAndShow(LPCWSTR lpWindowName,
		DWORD dwStyle,
		int width,
		int height,
		DWORD dwExStyle,
		int X,
		int Y,
		HWND hWndParent,
		HMENU hMenu);

	//메시지 처리 프로세스
	bool msgProcess();

	//사용자 상속 정의 함수군
	virtual bool				init();
	virtual bool				update();
	virtual bool				render();
	virtual bool				release();
	virtual int					run();

	virtual LRESULT __stdcall	msgHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);	//메시지 처리 프로시저
	virtual bool				resizeComponent(UINT iWidth, UINT iHeight);				//창 크기 의존 구성요소 재조정 작업
	virtual void				toggleFullscreen();										//전체 화면 전환
};

#define TESTER_HEADER								int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPWSTR lpCmdLine, int nCmdShow) {
#define TESTER_MAIN(wszWindowName, width, height)	TestCase tester(wszWindowName, CP_CUSTOM_WND0); tester.createWndAndShow(wszWindowName, WS_OVERLAPPEDWINDOW, width, height, NULL, 0, 0, NULL, NULL); tester.run();
#define TESTER_END }
#define TESTER_RUN(wszWindowName, width, height) TESTER_HEADER TESTER_MAIN(wszWindowName, width, height) TESTER_END
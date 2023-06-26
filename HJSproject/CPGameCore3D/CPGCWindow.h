//////////////////////////////////////////////////
//
// CPGCWindow.h
//		���ø����̼� â ���� ���
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCStd.h"
#include "CPGCIMGUIModule.h"
#include "ImGUI/imgui_impl_win32.h"

//����� Window class �̸� ��ũ��
#define CP_CUSTOM_WND0		L"CP_mainWnd"

//Wheel �Է½� ���콺 ��ġ ����
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
	//����� â Ŭ���� �̸� / â �̸�
	const WCHAR* m_wszWcName;
	const WCHAR* m_wszWndName;

public:
	//â�� �ڵ�
	static HWND m_hwnd;

	//â�� �ʺ� / ����
	static int	m_wndWidth;
	static int	m_wndHeight;

	//â�� �ּ�ȭ �÷���
	static bool	m_bCollapseFlag;

	//�� ���� ����ü
	static CPGC_MOUSE_WHEEL_STATE m_wheelState;

	//�ν��Ͻ� �ڵ�
	static HINSTANCE m_hInstance;

	static bool bDxCreateFlag;

	//��� Ŭ���� â ���ν���
	static LRESULT __stdcall wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CPGCWindow(const WCHAR* wszWndName, const WCHAR* wszWcName = nullptr);
	virtual ~CPGCWindow();

	//â �̸� ��������
	const WCHAR* getWndName();

	//â ���� �� ȭ�� ���
	bool createWndAndShow(LPCWSTR lpWindowName,
		DWORD dwStyle,
		int width,
		int height,
		DWORD dwExStyle,
		int X,
		int Y,
		HWND hWndParent,
		HMENU hMenu);

	//�޽��� ó�� ���μ���
	bool msgProcess();

	//����� ��� ���� �Լ���
	virtual bool				init();
	virtual bool				update();
	virtual bool				render();
	virtual bool				release();
	virtual int					run();

	virtual LRESULT __stdcall	msgHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);	//�޽��� ó�� ���ν���
	virtual bool				resizeComponent(UINT iWidth, UINT iHeight);				//â ũ�� ���� ������� ������ �۾�
	virtual void				toggleFullscreen();										//��ü ȭ�� ��ȯ
};

#define TESTER_HEADER								int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPWSTR lpCmdLine, int nCmdShow) {
#define TESTER_MAIN(wszWindowName, width, height)	TestCase tester(wszWindowName, CP_CUSTOM_WND0); tester.createWndAndShow(wszWindowName, WS_OVERLAPPEDWINDOW, width, height, NULL, 0, 0, NULL, NULL); tester.run();
#define TESTER_END }
#define TESTER_RUN(wszWindowName, width, height) TESTER_HEADER TESTER_MAIN(wszWindowName, width, height) TESTER_END
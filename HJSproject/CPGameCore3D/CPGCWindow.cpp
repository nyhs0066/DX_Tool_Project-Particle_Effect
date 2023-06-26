#include "CPGCWindow.h"

HWND						CPGCWindow::m_hwnd = NULL;
int							CPGCWindow::m_wndWidth = 0;
int							CPGCWindow::m_wndHeight = 0;
bool						CPGCWindow::m_bCollapseFlag = false;
HINSTANCE					CPGCWindow::m_hInstance = NULL;
CPGC_MOUSE_WHEEL_STATE		CPGCWindow::m_wheelState = { false, 0, 0, {0, 0} };
bool						CPGCWindow::bDxCreateFlag = false;

LRESULT __stdcall CPGCWindow::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//파생 클래스 접근을 위한 변수
	CPGCWindow* derivedClass = nullptr;

	//논 클라이언트 영역 생성후 한번만 처리한다. WM_CREATE에서 처리해도 무방
	if (uMsg == WM_NCCREATE)
	{
		//파생 클래스의 정보를 받아서 창 유저 데이터 영역에 바인딩
		//이러면 파생 창 클래스의 정보를 함수로 언제든지 불러올 수 있다.
		CREATESTRUCT* crt = reinterpret_cast<CREATESTRUCT*>(lParam);
		derivedClass = reinterpret_cast<CPGCWindow*>(crt->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)derivedClass);

		derivedClass->m_hwnd = hwnd;
	}
	else
	{
		derivedClass = (CPGCWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	//파생 클래스인 경우 해당 클래스의 전용 창 메시지 처리기를 사용해 메시지 처리한다.
	if (derivedClass) { return derivedClass->msgHandler(uMsg, wParam, lParam); }

	//아닌 경우 기본처리
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

CPGCWindow::CPGCWindow(const WCHAR* wszWndName, const WCHAR* wszWcName) : m_wszWndName(wszWndName), m_wszWcName(wszWcName)
{
	m_hwnd = NULL;

	m_wndWidth = 800;
	m_wndHeight = 600;
}

CPGCWindow::~CPGCWindow()
{
}

const WCHAR* CPGCWindow::getWndName()
{
	return m_wszWndName;
}

bool CPGCWindow::createWndAndShow(LPCWSTR lpWindowName,
	DWORD dwStyle,
	int width,
	int height,
	DWORD dwExStyle,
	int X,
	int Y,
	HWND hWndParent,
	HMENU hMenu)
{
	//창 스타일에 맞춰서 클라이언트 영역의 크기를 주어진 인수 크기에 맞게 조정
	RECT windowArea = { X, Y, X + width, Y + height };
	AdjustWindowRectEx(&windowArea, dwStyle, 0, dwExStyle);
	m_wndWidth		= width;
	m_wndHeight	= height;

	//현재 시스템 설정값을 불러와서 화면 중앙좌표 구하기
	int centerX = (GetSystemMetrics(SM_CXFULLSCREEN) - m_wndWidth) / 2;
	int centerY = (GetSystemMetrics(SM_CYFULLSCREEN) - m_wndHeight) / 2;

	//창 클래스 속성값 설정 및 등록
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize				= sizeof(WNDCLASSEX);
	wcex.lpszClassName		= m_wszWcName;
	wcex.hInstance			= GetModuleHandle(NULL);
	wcex.lpfnWndProc		= wndProc;
	wcex.hbrBackground		= (HBRUSH)(COLOR_BACKGROUND + 10);

	ATOM ret = RegisterClassEx(&wcex);

	//창 생성
	m_hwnd = CreateWindowEx(dwExStyle,
		wcex.lpszClassName,
		lpWindowName,
		dwStyle,
		centerX,
		centerY,
		windowArea.right - windowArea.left,
		windowArea.bottom - windowArea.top,
		hWndParent,
		hMenu,
		wcex.hInstance,
		this);

	if (!m_hwnd) { return FALSE; }

	//창 화면 출력
	ShowWindow(m_hwnd, SW_SHOWDEFAULT);

	return TRUE;
}

bool CPGCWindow::msgProcess()
{
	//함수의 동작에 따른 반환값을 표현할 변수
	bool bRunflag = true;

	//메시지 구조체 생성
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	///////////////////////////////////////
	//
	//	hwnd가 NULL이므로 메인 스레드에 날아온 메시지를 처리한다.
	//		주 창은 메인 스레드를 사용한다.
	//		처리할 메시지가 없으면 0을 반환한다.
	// 
	//	메시지 필터링 최대/최솟값을 0을 사용해서 모든 메시지를 받는다.
	//	마지막 인수를 TRUE로 놔서 PeekMessage가 메시지를 큐에서 끌어오면 해당 메시지를 큐에서 비운다
	//	
	//  처리할 메시지가 없는 경우 렌더링 작업을 수행할 수 있도록 메시지 프로세스를 종료한다.
	// 
	//	반환값은 정상 처리인경우 true, 창 종료 메시지를 받은 경우 또는 오류가 발생한 경우 false처리한다.
	// 
	///////////////////////////////////////

	while (PeekMessage(&msg, NULL, 0, 0, TRUE) != 0)
	{
		if (msg.message == WM_QUIT) { bRunflag = false;  break; }
		else
		{
			//메시지중 가상 키 메시지를 WM_CHAR메시지로 변환한다.
			TranslateMessage(&msg);

			//메시지를 적절한 창 프로시저로 전파한다.
			DispatchMessage(&msg);
		}
	}

	return bRunflag;
}

bool CPGCWindow::init()
{
	return true;
}

bool CPGCWindow::update()
{
	return true;
}

bool CPGCWindow::render()
{
	return true;
}

bool CPGCWindow::release()
{
	CloseHandle(m_hwnd);
	return true;
}

int CPGCWindow::run()
{
	int iRet = 1;

	if (!init()) { iRet = -1; return iRet; }

	while (iRet == 1)
	{
		if (msgProcess())
		{
			if (!update()) { iRet = -1; }
			if (!render()) { iRet = -1; }
		}
		else { iRet = 0; }
	}

	if (!release()) { iRet = -1; }

	return iRet;
}

LRESULT __stdcall CPGCWindow::msgHandler(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_MOUSEWHEEL :
		{
			//wParam 상위 Word : 휠이 회전되는 거리가 WHEEL_DELTA배수로 표현된다. 양수는 앞, 음수는 뒤로 회전
			//	GET_WHEEL_DELTA_WPARAM으로 추출한다.
			//wParam 하위 Word : 마우스 휠 입력과 같이 발생한 키입력. MK_매크로로 전달된다.
			//	GET_KEYSTATE_WPARAM으로 추출한다.
			//lParam 상위 Word : 화면 왼쪽 상단을 원점으로 하는 마우스의 y좌표
			//	GET_Y_LPARAM으로 추출한다. HIWORD를 사용하면 안된다(음수값이 될 수도 있는데 DWORD로 반환하기 때문)
			//lParam 하위 Word : 화면 왼쪽 상단을 원점으로 하는 마우스의 x좌표
			//	GET_X_LPARAM으로 추출한다. LOWORD를 사용하면 안된다(음수값이 될 수도 있는데 DWORD로 반환하기 때문)
			
			m_wheelState.bMoveWheel = true;
			m_wheelState.iWheelDt += GET_WHEEL_DELTA_WPARAM(wParam);
			m_wheelState.dwExtraKeyInput = GET_KEYSTATE_WPARAM(wParam);
			
			m_wheelState.pt.x = EXTRACT_LPARAM_X(lParam);
			m_wheelState.pt.y = EXTRACT_LPARAM_Y(lParam);

			ScreenToClient(m_hwnd, &m_wheelState.pt);

			return 0;
		}

		//Fullscreen Test
		/*case WM_KEYDOWN :
		{
			if (wParam == VK_F8)
			{
				toggleFullscreen();
			}

			return 0;
		}*/

		case WM_SIZE :
		{
			//최소화가 아닌 경우에만 DX리소스 크기 재조정 작업을 수행한다.
			if (wParam != SIZE_MINIMIZED)
			{
				//lParam의 하위 WORD에는 재조정된 클라이언트 영역의 너비를,
				//상위 WORD에는 재조정된 클라이언트 영역의 높이를 반환한다.

				m_bCollapseFlag = false;

				m_wndWidth = LOWORD(lParam);
				m_wndHeight = HIWORD(lParam);

				if (bDxCreateFlag)
				{ 
					bDxCreateFlag = false;
					resizeComponent(m_wndWidth, m_wndHeight); 
				}
			}
			else
			{
				m_bCollapseFlag = true;
			}

			return 0;
		}

		case WM_CLOSE :
		{
			if (MessageBox(m_hwnd, L"Quit?", L"Comfirm Box", MB_OKCANCEL) == IDOK) { DestroyWindow(m_hwnd); }
			return 0;
		}

		case WM_DESTROY :
		{
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

bool CPGCWindow::resizeComponent(UINT iWidth, UINT iHeight)
{
	return true;
}

void CPGCWindow::toggleFullscreen()
{
}

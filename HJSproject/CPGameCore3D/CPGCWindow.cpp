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
	//�Ļ� Ŭ���� ������ ���� ����
	CPGCWindow* derivedClass = nullptr;

	//�� Ŭ���̾�Ʈ ���� ������ �ѹ��� ó���Ѵ�. WM_CREATE���� ó���ص� ����
	if (uMsg == WM_NCCREATE)
	{
		//�Ļ� Ŭ������ ������ �޾Ƽ� â ���� ������ ������ ���ε�
		//�̷��� �Ļ� â Ŭ������ ������ �Լ��� �������� �ҷ��� �� �ִ�.
		CREATESTRUCT* crt = reinterpret_cast<CREATESTRUCT*>(lParam);
		derivedClass = reinterpret_cast<CPGCWindow*>(crt->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)derivedClass);

		derivedClass->m_hwnd = hwnd;
	}
	else
	{
		derivedClass = (CPGCWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	//�Ļ� Ŭ������ ��� �ش� Ŭ������ ���� â �޽��� ó���⸦ ����� �޽��� ó���Ѵ�.
	if (derivedClass) { return derivedClass->msgHandler(uMsg, wParam, lParam); }

	//�ƴ� ��� �⺻ó��
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
	//â ��Ÿ�Ͽ� ���缭 Ŭ���̾�Ʈ ������ ũ�⸦ �־��� �μ� ũ�⿡ �°� ����
	RECT windowArea = { X, Y, X + width, Y + height };
	AdjustWindowRectEx(&windowArea, dwStyle, 0, dwExStyle);
	m_wndWidth		= width;
	m_wndHeight	= height;

	//���� �ý��� �������� �ҷ��ͼ� ȭ�� �߾���ǥ ���ϱ�
	int centerX = (GetSystemMetrics(SM_CXFULLSCREEN) - m_wndWidth) / 2;
	int centerY = (GetSystemMetrics(SM_CYFULLSCREEN) - m_wndHeight) / 2;

	//â Ŭ���� �Ӽ��� ���� �� ���
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize				= sizeof(WNDCLASSEX);
	wcex.lpszClassName		= m_wszWcName;
	wcex.hInstance			= GetModuleHandle(NULL);
	wcex.lpfnWndProc		= wndProc;
	wcex.hbrBackground		= (HBRUSH)(COLOR_BACKGROUND + 10);

	ATOM ret = RegisterClassEx(&wcex);

	//â ����
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

	//â ȭ�� ���
	ShowWindow(m_hwnd, SW_SHOWDEFAULT);

	return TRUE;
}

bool CPGCWindow::msgProcess()
{
	//�Լ��� ���ۿ� ���� ��ȯ���� ǥ���� ����
	bool bRunflag = true;

	//�޽��� ����ü ����
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	///////////////////////////////////////
	//
	//	hwnd�� NULL�̹Ƿ� ���� �����忡 ���ƿ� �޽����� ó���Ѵ�.
	//		�� â�� ���� �����带 ����Ѵ�.
	//		ó���� �޽����� ������ 0�� ��ȯ�Ѵ�.
	// 
	//	�޽��� ���͸� �ִ�/�ּڰ��� 0�� ����ؼ� ��� �޽����� �޴´�.
	//	������ �μ��� TRUE�� ���� PeekMessage�� �޽����� ť���� ������� �ش� �޽����� ť���� ����
	//	
	//  ó���� �޽����� ���� ��� ������ �۾��� ������ �� �ֵ��� �޽��� ���μ����� �����Ѵ�.
	// 
	//	��ȯ���� ���� ó���ΰ�� true, â ���� �޽����� ���� ��� �Ǵ� ������ �߻��� ��� falseó���Ѵ�.
	// 
	///////////////////////////////////////

	while (PeekMessage(&msg, NULL, 0, 0, TRUE) != 0)
	{
		if (msg.message == WM_QUIT) { bRunflag = false;  break; }
		else
		{
			//�޽����� ���� Ű �޽����� WM_CHAR�޽����� ��ȯ�Ѵ�.
			TranslateMessage(&msg);

			//�޽����� ������ â ���ν����� �����Ѵ�.
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
			//wParam ���� Word : ���� ȸ���Ǵ� �Ÿ��� WHEEL_DELTA����� ǥ���ȴ�. ����� ��, ������ �ڷ� ȸ��
			//	GET_WHEEL_DELTA_WPARAM���� �����Ѵ�.
			//wParam ���� Word : ���콺 �� �Է°� ���� �߻��� Ű�Է�. MK_��ũ�η� ���޵ȴ�.
			//	GET_KEYSTATE_WPARAM���� �����Ѵ�.
			//lParam ���� Word : ȭ�� ���� ����� �������� �ϴ� ���콺�� y��ǥ
			//	GET_Y_LPARAM���� �����Ѵ�. HIWORD�� ����ϸ� �ȵȴ�(�������� �� ���� �ִµ� DWORD�� ��ȯ�ϱ� ����)
			//lParam ���� Word : ȭ�� ���� ����� �������� �ϴ� ���콺�� x��ǥ
			//	GET_X_LPARAM���� �����Ѵ�. LOWORD�� ����ϸ� �ȵȴ�(�������� �� ���� �ִµ� DWORD�� ��ȯ�ϱ� ����)
			
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
			//�ּ�ȭ�� �ƴ� ��쿡�� DX���ҽ� ũ�� ������ �۾��� �����Ѵ�.
			if (wParam != SIZE_MINIMIZED)
			{
				//lParam�� ���� WORD���� �������� Ŭ���̾�Ʈ ������ �ʺ�,
				//���� WORD���� �������� Ŭ���̾�Ʈ ������ ���̸� ��ȯ�Ѵ�.

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

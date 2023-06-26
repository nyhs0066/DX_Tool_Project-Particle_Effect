#include "CPGCInput.h"

CPGCMainInput::CPGCMainInput()
{
	m_prevMousePt = { 0 };
	m_curMousePt = { 0 };
	m_mousePosOffset = { 0 };
	ZeroMemory(&m_dwKeyStates, sizeof(m_dwKeyStates));

	GetCursorPos(&m_prevMousePt);
	ScreenToClient(CPGCWindow::m_hwnd, &m_prevMousePt);
}

CPGCMainInput::~CPGCMainInput()
{
}

POINT CPGCMainInput::getMousePos()
{
	return m_curMousePt;
}

DWORD CPGCMainInput::getKeyState(SHORT vKey)
{
	if (0 < vKey && vKey < 256) { return m_dwKeyStates[vKey]; }
	else { return -1; }
}

POINT CPGCMainInput::getMouseOffset()
{
	return m_mousePosOffset;
}

CPGC_MOUSE_WHEEL_STATE CPGCMainInput::getMouseWheelState()
{
	return CPGCWindow::m_wheelState;
}

bool CPGCMainInput::init()
{
	return true;
}

bool CPGCMainInput::update()
{
	//현재 커서 좌표를 "화면 좌표"로 받는다. 
	GetCursorPos(&m_curMousePt);

	//클라이언트 좌표계로 화면 좌표를 변환한다.
	ScreenToClient(CPGCWindow::m_hwnd, &m_curMousePt);

	DWORD keyTemp = 0;

	for (int i = 0; i < MAX_NUMBER_OF_VKEY; i++)
	{
		//비동기 형식의 키 상태 확인 함수
		keyTemp = GetAsyncKeyState(i);

		if (keyTemp & 0x8000)
		{
			//키가 눌렸다면
			if (m_dwKeyStates[i] == KEY_FREE || m_dwKeyStates[i] == KEY_UP) 
			{
				m_dwKeyStates[i] = KEY_DOWN;
			}
			else { m_dwKeyStates[i] = KEY_HOLD; }
		}
		else
		{
			//키가 눌리지 않았다면
			if (m_dwKeyStates[i] == KEY_HOLD || m_dwKeyStates[i] == KEY_DOWN) 
			{
				m_dwKeyStates[i] = KEY_UP;
			}
			else { m_dwKeyStates[i] = KEY_FREE; }
		}
	}

	//마우스 오프셋 값을 받는다.
	m_mousePosOffset.x = m_prevMousePt.x - m_curMousePt.x;
	m_mousePosOffset.y = m_prevMousePt.y - m_curMousePt.y;

	m_prevMousePt = m_curMousePt;
	return true;
}

bool CPGCMainInput::release()
{
	return true;
}
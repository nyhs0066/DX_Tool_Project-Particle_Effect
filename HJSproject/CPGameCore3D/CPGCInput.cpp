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
	//���� Ŀ�� ��ǥ�� "ȭ�� ��ǥ"�� �޴´�. 
	GetCursorPos(&m_curMousePt);

	//Ŭ���̾�Ʈ ��ǥ��� ȭ�� ��ǥ�� ��ȯ�Ѵ�.
	ScreenToClient(CPGCWindow::m_hwnd, &m_curMousePt);

	DWORD keyTemp = 0;

	for (int i = 0; i < MAX_NUMBER_OF_VKEY; i++)
	{
		//�񵿱� ������ Ű ���� Ȯ�� �Լ�
		keyTemp = GetAsyncKeyState(i);

		if (keyTemp & 0x8000)
		{
			//Ű�� ���ȴٸ�
			if (m_dwKeyStates[i] == KEY_FREE || m_dwKeyStates[i] == KEY_UP) 
			{
				m_dwKeyStates[i] = KEY_DOWN;
			}
			else { m_dwKeyStates[i] = KEY_HOLD; }
		}
		else
		{
			//Ű�� ������ �ʾҴٸ�
			if (m_dwKeyStates[i] == KEY_HOLD || m_dwKeyStates[i] == KEY_DOWN) 
			{
				m_dwKeyStates[i] = KEY_UP;
			}
			else { m_dwKeyStates[i] = KEY_FREE; }
		}
	}

	//���콺 ������ ���� �޴´�.
	m_mousePosOffset.x = m_prevMousePt.x - m_curMousePt.x;
	m_mousePosOffset.y = m_prevMousePt.y - m_curMousePt.y;

	m_prevMousePt = m_curMousePt;
	return true;
}

bool CPGCMainInput::release()
{
	return true;
}
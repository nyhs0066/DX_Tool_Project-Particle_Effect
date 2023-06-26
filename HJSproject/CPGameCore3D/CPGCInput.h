//////////////////////////////////////////////////
//
// CPGCInput.h
//		���� �Է¿� ���� ó���� �����ϱ� ���� ���
// 
//////////////////////////////////////////////////

#pragma once
#include "CPGCWindow.h"

#define MAX_NUMBER_OF_VKEY	256

enum CPGCKEYSTATE { KEY_FREE = 0, KEY_DOWN, KEY_UP, KEY_HOLD };

class CPGCMainInput 
{
private:
	POINT m_prevMousePt;
	POINT m_curMousePt;
	POINT m_mousePosOffset;

	DWORD m_dwKeyStates[MAX_NUMBER_OF_VKEY];

	CPGCMainInput();
	~CPGCMainInput();
public:
	CPGCMainInput(const CPGCMainInput& other) = delete;
	CPGCMainInput& operator=(const CPGCMainInput& other) = delete;

	//���� �������� ���콺 ������ ��ġ�� �޴´�.
	POINT	getMousePos();

	//���� �������� Ű���� �Է� ���¸� �޴´�.
	DWORD	getKeyState(SHORT vKey);

	//���� ������ ���콺 ��ġ�� ���� ������ ���콺 ��ġ�� ��ġ ��ȭ���� �޴´�.
	POINT	getMouseOffset();

	//���콺 �� ���� ���¸� �ҷ��´�.
	CPGC_MOUSE_WHEEL_STATE getMouseWheelState();

	bool	init();
	bool	update();
	bool	release();

	//���� �ν��Ͻ� ��ȯ
	static CPGCMainInput& getInstance()
	{
		//���� �ʱ�ȭ
		static CPGCMainInput singleInst;
		return singleInst;
	}
};

#define CPGC_MAININPUT CPGCMainInput::getInstance()
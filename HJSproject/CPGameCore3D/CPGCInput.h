//////////////////////////////////////////////////
//
// CPGCInput.h
//		각종 입력에 대한 처리를 수행하기 위한 헤더
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

	//현재 프레임의 마우스 포인터 위치를 받는다.
	POINT	getMousePos();

	//현재 프레임의 키보드 입력 상태를 받는다.
	DWORD	getKeyState(SHORT vKey);

	//이전 프레임 마우스 위치와 현재 프레임 마우스 위치의 위치 변화량을 받는다.
	POINT	getMouseOffset();

	//마우스 휠 동작 상태를 불러온다.
	CPGC_MOUSE_WHEEL_STATE getMouseWheelState();

	bool	init();
	bool	update();
	bool	release();

	//전역 인스턴스 반환
	static CPGCMainInput& getInstance()
	{
		//늦은 초기화
		static CPGCMainInput singleInst;
		return singleInst;
	}
};

#define CPGC_MAININPUT CPGCMainInput::getInstance()
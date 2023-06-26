#pragma once
#include "CPGCCamera.h"

//extern "C"형식의 dll파일의 함수를 읽어 내기위한 헬퍼 함수
// hLibrary = 라이브러리 파일 핸들 포인터
// lpszLibrary = 라이브러리 파일의 경로
// nCount = 가변인수 쌍의 개수
//가변인수로 함수 포인터의 주소와 해당 함수의 라이브러리상 이름을 짝당 하나씩 넘겨준다.
static BOOL GetProcAddresses(HMODULE* hLibrary, LPCWSTR lpszLibrary, INT nCount, ...);
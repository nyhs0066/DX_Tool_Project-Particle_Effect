//////////////////////////////////////////////////
//
// CPGCStd.h
//		사용할 필수 헤더 및 구조 등을 모아놓은 기본 헤더 파일
//		
//////////////////////////////////////////////////

#pragma once
//창 기능 이용
#include <windows.h>

//파일 제어 및 관리
#include <fstream>
#include <io.h>			//디렉터리 로드 기능을 위한 헤더

//DX객체 스마트 포인터 이용을 위한 헤더
//ComPtr은 생성시 자동으로 nullptr이 할당된다.
#include <wrl.h>

//DX관련 핵심 기능
#include <d3d11.h>		//DX 3D 11
#include <dxgi.h>		//DXGI

//DX 런타임 컴파일러 - Shader컴파일러를 이용하기 위한 헤더
#include <d3dcompiler.h>

//DX ToolKit - DX 수학 라이브러리 구조체 래핑 + 커스텀 수학 구조체 헤더
//래핑말고 원 형태를 쓰고 싶으면 <DirectXMath.h>사용할 것
//네임스페이스는 DirectX::SimpleMath
//수학 라이브러리의 벡터는 기본이 영벡터고 행렬은 기본이 정규 기저 단위 행렬이다.
#include "CPGCMathWrap.h"

//STL
#include <string>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <stack>
#include <queue>

//스레드 기능 이용을 위한 STL
#include <thread>

//ANSI - 멀티바이트 - 유니코드 관련 헤더
#include <atlbase.h>		//wide(유니코드) 문자 - 멀티바이트 문자 상호 변환을 위한 헤더
#include <tchar.h>			//시스템에 따른 적절한 문자열 타입을 사용할 수 있게 해주는 매크로 헤더

//의사 난수 생성 커스텀 헤더
#include "CPRNGenerator.h"

//파일 I/O를 위한 커스텀 헤더
#include "CPGCFileIO.h"

//커스텀 매크로
#define CPGC_SHADER_LANG_VERSION				"_5_0"

#define CPGC_API_STR_CONVBUF_SIZE				4096

#define CPGC_VNAME2WSTR(id)						(L#id)
#define CPGC_VNAME2STR(id)						(#id)

#define CPGC_ARRAY_ELEM_NUM(A)					_ARRAYSIZE(A)

//singleton 구조
//C++11부터 정적 지역 변수의 초기화는 멀티 스레드 환경에서도 한번만 수행됨이 보장된다.
//	즉, Thread-Safe하다
//move 생성자는 복사 생성자 /할당 연산자를 명시적으로 선언하는 경우 자동 생성되지 않는다. 
//template singleton을 사용하고자 하는 경우 friend지정이 필수
template <class c>
class Singleton
{
public:
	Singleton(const Singleton& other) = delete;
	Singleton& operator=(const Singleton& other) = delete;

	//늦은 초기화 방식
	static c& getInstance()
	{
		static c ret;
		return ret;
	}
};

//Mayer's Singleton
//friend 지정은 필요 없지만 이 구조를 개별적으로 정의해 주어야 한다.

class Mayer_Singleton
{
private:
	Mayer_Singleton() {}
	~Mayer_Singleton() {}
public:
	Mayer_Singleton(const Mayer_Singleton& other) = delete;
	Mayer_Singleton& operator=(const Mayer_Singleton& other) = delete;
public:

	//늦은 초기화 방식
	static Mayer_Singleton& getInstance()
	{
		static Mayer_Singleton ret;
		return ret;
	}
};

//디버그 창에 DX리소스의 생성 오류 코드 출력 함수
static void printCreateErr(UINT iErr)
{
	std::wstring errStr = L"Error Code : ";
	errStr += std::to_wstring(iErr) + L"\n";
	OutputDebugString(errStr.c_str());
}

//wide 문자열(유니코드) / 멀티바이트 문자열 변환 함수
//ATL (Active Template Library)
static std::wstring atl_M2W(const std::string& multibyteStr)
{
	USES_CONVERSION;
	return std::wstring(A2W(multibyteStr.c_str()));
};

static std::string atl_W2M(const std::wstring& widebyteStr)
{
	USES_CONVERSION;
	return std::string(W2A(widebyteStr.c_str()));
};

//WINAPI
static char* api_W2M(const WCHAR* widebyteStr)
{
	char multibyteStr[CPGC_API_STR_CONVBUF_SIZE] = { 0 };

	int iLength = WideCharToMultiByte(CP_ACP, 0,
		widebyteStr, -1,
		0, 0,					//cbMuitiByte = 0이면 변환에 필요한 크기를 반환한다.
		NULL, NULL);
	int iRet = WideCharToMultiByte(CP_ACP, 0,
		widebyteStr, -1,				// 소스(-1은 null포함 문자열을 생성한다는 의미)
		multibyteStr, iLength,		// 대상
		NULL, NULL);			// 코드 페이지에 표현되지 않는 문자 추가부
	return multibyteStr;
}
static bool api_W2M(const WCHAR* src, char* pDest)
{
	int iLength = WideCharToMultiByte(CP_ACP, 0,
		src, -1, 0, 0, NULL, NULL);
	int iRet = WideCharToMultiByte(CP_ACP, 0,
		src, -1,				//  소스
		pDest, iLength,			// 대상
		NULL, NULL);
	if (iRet == 0) return false;
	return true;
}
static WCHAR* api_M2W(const char* multibyteStr)
{
	WCHAR widebyteStr[CPGC_API_STR_CONVBUF_SIZE] = { 0 };
	int iLength = MultiByteToWideChar(CP_ACP, 0,
		multibyteStr, -1,
		0, 0);						//cchWideChar = 0이면 변환에 필요한 크기를 반환한다.
	int iRet = MultiByteToWideChar(CP_ACP, 0,
		multibyteStr, -1,					//  소스(-1은 null포함 문자열을 생성한다는 의미)
		widebyteStr, iLength);			// 대상
	return widebyteStr;
}
static bool api_M2W(char* pSrc, WCHAR* pDest)
{
	int iLength = MultiByteToWideChar(CP_ACP, 0,
		pSrc, -1, 0, 0);
	int iRet = MultiByteToWideChar(CP_ACP, 0,
		pSrc, -1,  //  소스
		pDest, iLength); // 대상		
	if (iRet == 0) return false;
	return true;
}

struct CPRS_NAME
{
	std::wstring wszName;
	std::string szName;

	void setName(std::wstring name)
	{
		wszName = name;
		szName = atl_W2M(name);
	}
	void setName(std::string name)
	{
		wszName = atl_M2W(name);
		szName = name;
	}
};


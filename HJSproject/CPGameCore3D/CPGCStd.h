//////////////////////////////////////////////////
//
// CPGCStd.h
//		����� �ʼ� ��� �� ���� ���� ��Ƴ��� �⺻ ��� ����
//		
//////////////////////////////////////////////////

#pragma once
//â ��� �̿�
#include <windows.h>

//���� ���� �� ����
#include <fstream>
#include <io.h>			//���͸� �ε� ����� ���� ���

//DX��ü ����Ʈ ������ �̿��� ���� ���
//ComPtr�� ������ �ڵ����� nullptr�� �Ҵ�ȴ�.
#include <wrl.h>

//DX���� �ٽ� ���
#include <d3d11.h>		//DX 3D 11
#include <dxgi.h>		//DXGI

//DX ��Ÿ�� �����Ϸ� - Shader�����Ϸ��� �̿��ϱ� ���� ���
#include <d3dcompiler.h>

//DX ToolKit - DX ���� ���̺귯�� ����ü ���� + Ŀ���� ���� ����ü ���
//���θ��� �� ���¸� ���� ������ <DirectXMath.h>����� ��
//���ӽ����̽��� DirectX::SimpleMath
//���� ���̺귯���� ���ʹ� �⺻�� �����Ͱ� ����� �⺻�� ���� ���� ���� ����̴�.
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

//������ ��� �̿��� ���� STL
#include <thread>

//ANSI - ��Ƽ����Ʈ - �����ڵ� ���� ���
#include <atlbase.h>		//wide(�����ڵ�) ���� - ��Ƽ����Ʈ ���� ��ȣ ��ȯ�� ���� ���
#include <tchar.h>			//�ý��ۿ� ���� ������ ���ڿ� Ÿ���� ����� �� �ְ� ���ִ� ��ũ�� ���

//�ǻ� ���� ���� Ŀ���� ���
#include "CPRNGenerator.h"

//���� I/O�� ���� Ŀ���� ���
#include "CPGCFileIO.h"

//Ŀ���� ��ũ��
#define CPGC_SHADER_LANG_VERSION				"_5_0"

#define CPGC_API_STR_CONVBUF_SIZE				4096

#define CPGC_VNAME2WSTR(id)						(L#id)
#define CPGC_VNAME2STR(id)						(#id)

#define CPGC_ARRAY_ELEM_NUM(A)					_ARRAYSIZE(A)

//singleton ����
//C++11���� ���� ���� ������ �ʱ�ȭ�� ��Ƽ ������ ȯ�濡���� �ѹ��� ������� ����ȴ�.
//	��, Thread-Safe�ϴ�
//move �����ڴ� ���� ������ /�Ҵ� �����ڸ� ��������� �����ϴ� ��� �ڵ� �������� �ʴ´�. 
//template singleton�� ����ϰ��� �ϴ� ��� friend������ �ʼ�
template <class c>
class Singleton
{
public:
	Singleton(const Singleton& other) = delete;
	Singleton& operator=(const Singleton& other) = delete;

	//���� �ʱ�ȭ ���
	static c& getInstance()
	{
		static c ret;
		return ret;
	}
};

//Mayer's Singleton
//friend ������ �ʿ� ������ �� ������ ���������� ������ �־�� �Ѵ�.

class Mayer_Singleton
{
private:
	Mayer_Singleton() {}
	~Mayer_Singleton() {}
public:
	Mayer_Singleton(const Mayer_Singleton& other) = delete;
	Mayer_Singleton& operator=(const Mayer_Singleton& other) = delete;
public:

	//���� �ʱ�ȭ ���
	static Mayer_Singleton& getInstance()
	{
		static Mayer_Singleton ret;
		return ret;
	}
};

//����� â�� DX���ҽ��� ���� ���� �ڵ� ��� �Լ�
static void printCreateErr(UINT iErr)
{
	std::wstring errStr = L"Error Code : ";
	errStr += std::to_wstring(iErr) + L"\n";
	OutputDebugString(errStr.c_str());
}

//wide ���ڿ�(�����ڵ�) / ��Ƽ����Ʈ ���ڿ� ��ȯ �Լ�
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
		0, 0,					//cbMuitiByte = 0�̸� ��ȯ�� �ʿ��� ũ�⸦ ��ȯ�Ѵ�.
		NULL, NULL);
	int iRet = WideCharToMultiByte(CP_ACP, 0,
		widebyteStr, -1,				// �ҽ�(-1�� null���� ���ڿ��� �����Ѵٴ� �ǹ�)
		multibyteStr, iLength,		// ���
		NULL, NULL);			// �ڵ� �������� ǥ������ �ʴ� ���� �߰���
	return multibyteStr;
}
static bool api_W2M(const WCHAR* src, char* pDest)
{
	int iLength = WideCharToMultiByte(CP_ACP, 0,
		src, -1, 0, 0, NULL, NULL);
	int iRet = WideCharToMultiByte(CP_ACP, 0,
		src, -1,				//  �ҽ�
		pDest, iLength,			// ���
		NULL, NULL);
	if (iRet == 0) return false;
	return true;
}
static WCHAR* api_M2W(const char* multibyteStr)
{
	WCHAR widebyteStr[CPGC_API_STR_CONVBUF_SIZE] = { 0 };
	int iLength = MultiByteToWideChar(CP_ACP, 0,
		multibyteStr, -1,
		0, 0);						//cchWideChar = 0�̸� ��ȯ�� �ʿ��� ũ�⸦ ��ȯ�Ѵ�.
	int iRet = MultiByteToWideChar(CP_ACP, 0,
		multibyteStr, -1,					//  �ҽ�(-1�� null���� ���ڿ��� �����Ѵٴ� �ǹ�)
		widebyteStr, iLength);			// ���
	return widebyteStr;
}
static bool api_M2W(char* pSrc, WCHAR* pDest)
{
	int iLength = MultiByteToWideChar(CP_ACP, 0,
		pSrc, -1, 0, 0);
	int iRet = MultiByteToWideChar(CP_ACP, 0,
		pSrc, -1,  //  �ҽ�
		pDest, iLength); // ���		
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


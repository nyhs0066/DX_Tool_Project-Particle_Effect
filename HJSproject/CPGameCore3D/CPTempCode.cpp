#include "CPTempCode.h"

BOOL GetProcAddresses(HMODULE* hLibrary, LPCWSTR lpszLibrary, INT nCount, ...)
{
	BOOL bRet = TRUE;

	//가변인수 읽기 포인터 : 후에 va_start에 의해 가변인수 메모리의 시작 주소가 된다. 
	//바이트 단위로 이동하며 char* 의 재정의형이다.
	va_list va;

	//두번째 인수는 마지막 가변인수 다음인수 위치를 참조하기 위한 매개변수를 넘겨준다.
	//가변인수 읽기 시작 위치가 넘겨준 인수의 다음 위치가 된다.
	//매개변수는 오른쪽에서 왼쪽으로 읽히므로 념겨준 매개변수로 인해 읽을 가변인수의 개수가 결정된다.
	va_start(va, nCount);
	if ((*hLibrary = LoadLibrary(lpszLibrary))
		!= NULL)
	{
		FARPROC* lpfProcFunction = NULL;
		LPCSTR lpszFuncName = NULL;
		INT nIdxCount = 0;
		while (nIdxCount < nCount)
		{
			//va_arg는 가변인수를 넘겨준 타입 크기만큼 메모리에서 읽은 뒤 다음 가변인수를 읽기 위해 읽은 만큼
			//읽기 포인터를 이동시킨다.
			lpfProcFunction = va_arg(va, FARPROC*);
			lpszFuncName = va_arg(va, LPCSTR);
			if ((*lpfProcFunction = GetProcAddress(*hLibrary, lpszFuncName)) == NULL)
			{
				lpfProcFunction = NULL;
				bRet = FALSE;
				WCHAR wstrDestination[MAX_PATH] = { 0, };
				DWORD cchDestChar = strlen(lpszFuncName);
				int nResult = MultiByteToWideChar(CP_ACP, 0, lpszFuncName, -1,
					wstrDestination, cchDestChar);

				wstrDestination[cchDestChar - 1] = 0;
				MessageBox(0, wstrDestination, _T("Loading Fail"), MB_OK);
				break;
			}
			nIdxCount++;
		}
	}
	else
	{
		bRet = FALSE;
	}

	//가변인수 읽기 과정을 끝내고 읽기 포인터를 정리한다.
	va_end(va);
	if (bRet == FALSE && *hLibrary != NULL)
		FreeLibrary(*hLibrary);
	return bRet;
}


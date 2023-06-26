#include "CPTempCode.h"

BOOL GetProcAddresses(HMODULE* hLibrary, LPCWSTR lpszLibrary, INT nCount, ...)
{
	BOOL bRet = TRUE;

	//�����μ� �б� ������ : �Ŀ� va_start�� ���� �����μ� �޸��� ���� �ּҰ� �ȴ�. 
	//����Ʈ ������ �̵��ϸ� char* �� ���������̴�.
	va_list va;

	//�ι�° �μ��� ������ �����μ� �����μ� ��ġ�� �����ϱ� ���� �Ű������� �Ѱ��ش�.
	//�����μ� �б� ���� ��ġ�� �Ѱ��� �μ��� ���� ��ġ�� �ȴ�.
	//�Ű������� �����ʿ��� �������� �����Ƿ� ����� �Ű������� ���� ���� �����μ��� ������ �����ȴ�.
	va_start(va, nCount);
	if ((*hLibrary = LoadLibrary(lpszLibrary))
		!= NULL)
	{
		FARPROC* lpfProcFunction = NULL;
		LPCSTR lpszFuncName = NULL;
		INT nIdxCount = 0;
		while (nIdxCount < nCount)
		{
			//va_arg�� �����μ��� �Ѱ��� Ÿ�� ũ�⸸ŭ �޸𸮿��� ���� �� ���� �����μ��� �б� ���� ���� ��ŭ
			//�б� �����͸� �̵���Ų��.
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

	//�����μ� �б� ������ ������ �б� �����͸� �����Ѵ�.
	va_end(va);
	if (bRet == FALSE && *hLibrary != NULL)
		FreeLibrary(*hLibrary);
	return bRet;
}


#pragma once
#include "CPGCCamera.h"

//extern "C"������ dll������ �Լ��� �о� �������� ���� �Լ�
// hLibrary = ���̺귯�� ���� �ڵ� ������
// lpszLibrary = ���̺귯�� ������ ���
// nCount = �����μ� ���� ����
//�����μ��� �Լ� �������� �ּҿ� �ش� �Լ��� ���̺귯���� �̸��� ¦�� �ϳ��� �Ѱ��ش�.
static BOOL GetProcAddresses(HMODULE* hLibrary, LPCWSTR lpszLibrary, INT nCount, ...);
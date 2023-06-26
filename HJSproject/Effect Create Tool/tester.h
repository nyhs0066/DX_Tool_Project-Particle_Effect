/////////////////////////////////////////////////////////////////////////
//	EffectCreate_3
//		��ƼŬ �ý��� ���� ���� : map -> vector
//		��ƼŬ �ý��� ��� ����
//			������ġ ������ �Ӽ� �߰�
//			�ʱ�ȸ�� �Ӽ� �߰�
//			���� ���� ������ Ʈ������ X, Y ���� ���� ����
//			���ӵ� ���� �Ӽ� �߰�
//			�׸��� ���� �̹��� ����Ʈ���� ���� ����
/////////////////////////////////////////////////////////////////////////

#pragma once
#include "CPGameCore.h"
#include "CPMainDockspace.h"

class TestCase : public CPGameCore
{
public:
	TestCase(const WCHAR* wszWndName, const WCHAR* wszWcName);
	~TestCase();

	bool init() override;

	bool preUpdate() override;
	bool update() override;
	bool postUpdate() override;

	bool preRender() override;
	bool render() override;
	bool postRender() override;

	bool release() override;
};
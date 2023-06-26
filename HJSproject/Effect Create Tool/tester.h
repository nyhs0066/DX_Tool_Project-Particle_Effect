/////////////////////////////////////////////////////////////////////////
//	EffectCreate_3
//		파티클 시스템 구조 변경 : map -> vector
//		파티클 시스템 기능 개선
//			생성위치 오프셋 속성 추가
//			초기회전 속성 추가
//			수명에 따른 스케일 트랜스폼 X, Y 범위 설정 가능
//			가속도 범위 속성 추가
//			그리는 순서 이미터 리스트에서 변경 가능
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
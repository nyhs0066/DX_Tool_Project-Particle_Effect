#include "tester.h"

TestCase::TestCase(const WCHAR* wszWndName, const WCHAR* wszWcName) : CPGameCore(wszWndName, wszWcName)
{
}

TestCase::~TestCase()
{
}

bool TestCase::init()
{
	//도킹 스페이스 초기화
	MAIN_DOCKSPACE.init(CPGC_DXMODULE.getDevice(), CPGC_DXMODULE.getImDContext());

	return true;
}

bool TestCase::preUpdate()
{
	MAIN_DOCKSPACE.preUpdate();

	return true;
}

bool TestCase::update()
{
	MAIN_DOCKSPACE.update();

	return true;
}

bool TestCase::postUpdate()
{
	MAIN_DOCKSPACE.postUpdate();
	return true;
}

bool TestCase::preRender()
{
	MAIN_DOCKSPACE.preRender();

	return true;
}

bool TestCase::render()
{
	MAIN_DOCKSPACE.render();

	return true;
}

bool TestCase::postRender()
{
	MAIN_DOCKSPACE.postRender();

    return true;
}

bool TestCase::release()
{
	MAIN_DOCKSPACE.release();
	return true;
}

TESTER_RUN(L"Particle Effect Tool", 1280, 800)

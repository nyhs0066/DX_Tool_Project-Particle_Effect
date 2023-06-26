/////////////////////////////////////////////////////////////////////////////////
//	CPMainDockspace.h
//		���� ��ŷ �����̽� ���
//		
//		ImGui::Separator()				���м� ����
//		ImGui::SeparatorText()			ĸ���� �ִ� ���м� ����
//		ImGui::SameLine()				���� �� ��ġ
//		ImGui::Indent()					�鿩����
//      ImGui::SameLine()				���� ���� ��ġ
//		HelpMarker()					���� ���� ǥ��
//		ImGui::GetTextLineHeight()		�ؽ�Ʈ �� ����
//		ImGui::IsMouseDoubleClicked(0)	����Ŭ�� ����
//		
//		�̸� �����ο� ##�� ���� �̸� �Ǵ� �̸��� ���� ImGui��ü�� �ĺ��ϱ� ���� �߰� ��Ʈ���θ� �����Ѵ�.
//		��� ���� �����ؾ��Ѵ�.
//		
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CPEffectWorkspace.h"
#include "CPMapWorkspace.h"
#include "CPPicker.h"
#include "CPGCFBXLoader.h"

#define CPGC_MAINDOCKSPACE_SETTING_FILE_PATH L"../../data/setting/MyTool.txt"

#define DEFAULT_OLD_EFFECT_DIRECTORY_WSZPATH	L"../../data/save/particlesystem/old/"
#define DEFAULT_OLD_EFFECT_DIRECTORY_SZPATH		"../../data/save/particlesystem/old/"

#define DEFAULT_NEW_EFFECT_DIRECTORY_WSZPATH	L"../../data/save/particlesystem/new/"
#define DEFAULT_NEW_EFFECT_DIRECTORY_SZPATH		"../../data/save/particlesystem/new/"

enum CPIMGUI_EFFECTFILE_LOAD_OPT
{
	LOPT_CURRENT_VER,
	LOPT_PREV_VER,
	NUMBER_OF_LOAD_OPT
};

class CPMainDockspace
{
private:
	ID3D11Device*									m_pDevice;
	ID3D11DeviceContext*							m_pDContext;
	CPGCCamera*										m_pMainCam;

	//DX Render Target
	CPGCRenderTarget*								m_pMainRT;
	D3D11_VIEWPORT									m_prevMainRTVPort;
	D3D11_VIEWPORT									m_mainRTVPort;
	UINT											m_RTWindowOffsetY;

	bool											m_bResizeFlag;	//���� �� ũ�� ������ �÷���
	bool											m_bFullscreen;	//���� �̻��

	//���� Dx State ��뼳�� ����
	//RS
	bool											m_bEnableWireFrame;		//���̾� ������

	//��ũ �����̽�
	CPEffectWorkspace*								m_pEffectWSpace;
	CPMapWorkspace*									m_pMapWSpace;

	//ImGui ������ �Լ�
	bool											m_bMainShow;
	bool											m_bOpenDXRTWindow;
	bool											m_bOpenDemoWindow;

	bool											m_bOpenEffectSystemWindow;
	bool											m_bOpenMapWindow;

	int												m_iShowEffectPopup;
	int												m_iShowMapPopup;

	CPPicker										m_picker;
	bool											m_isMapPicked;

	int												m_testNodeIdx;
	CPM_Vector3										m_vTestPickingPos;

	CPMainDockspace();
	~CPMainDockspace();

public:
	CPMainDockspace(const CPMainDockspace& other) = delete;
	CPMainDockspace& operator=(const CPMainDockspace& other) = delete;
public:
	bool init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);

	bool preUpdate();
	bool update();
	bool postUpdate();

	bool preRender();
	bool render();
	bool postRender();

	void effectRender();

	bool release();

	bool resizeComponent(UINT iWidth, UINT iHeight);
	bool setMainCam(CPGCCamera* pMainCam);
	bool getResizeFlag();

	void showMainDockSpace();
	void showStatusWindow();
	void showDXRTWindow();

	//����Ʈ ������
	void showEffectCreateWindow();

	//���� ������
	void showMapCreateWindow();

	//��ġ ������Ʈ �� FBX�𵨺�

	//UI��

	//��ŷ �۾� ��
	bool getIntersectionInMap();

	static CPMainDockspace& getInstance()
	{
		static CPMainDockspace singleInst;
		return singleInst;
	}
};

#define MAIN_DOCKSPACE	CPMainDockspace::getInstance()

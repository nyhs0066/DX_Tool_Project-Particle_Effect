#pragma once
#include "CPGameCore.h"
#include "CPGCMap.h"
#include "CPGCSpacePartition.h"
#include "CPImGuiControls.h"

#define CPIMGUI_MIN_NUMBER_OF_CELL (2+1)
#define CPIMGUI_MAX_NUMBER_OF_CELL (4096+1)

#define CPIMGUI_MIN_CELL_SIZE		1.0f
#define CPIMGUI_MAX_CELL_SIZE		100.0f

#define CPIMGUI_MIN_PARTITION_DEPTH	1
#define CPIMGUI_MAX_PARTITION_DEPTH	8


class CPMapWorkspace
{
private:
	ID3D11Device*									m_pDevice;
	ID3D11DeviceContext*							m_pDContext;

	CPGCCamera*										m_pMainCam;
	CPGCRenderTarget*								m_pRT;

	//텍스처 검색 및 선택
	CPIMGUITexSelector*								m_pMapBaseTexSelector;
	CPIMGUITexSelector*								m_pHeightMapTexSelector;

	//지형 생성 변수
	int												m_iMCRow;
	int												m_iMCCol;
	int												m_iMCDepthLimit;
	float											m_fMCCellsize;

	bool											m_bUseHeightMap;

	CPMapWorkspace();
	~CPMapWorkspace();
public:
	CPGCMap*										m_pMap;
	CPGCQuadTree*									m_pQuadTree;
public:
	CPMapWorkspace(const CPMapWorkspace& other) = delete;
	CPMapWorkspace& operator=(const CPMapWorkspace& other) = delete;

public:
	bool init(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDContext,
		CPGCRenderTarget* pMainRT,
		CPGCCamera* pMainCam);

	bool preUpdate();
	bool update();
	bool postUpdate();

	bool preRender();
	bool render();
	bool postRender();

	bool release();

	void setMainCam(CPGCCamera* pCam);

	void openWorkspaceWindow();
	void showCreateMapWindow();

	void createNewMapPopup(int& popOpt, bool& bOpenWorkspace);
	void loadMapPopup(int& popOpt, bool& bOpenWorkspace);
	void saveMapPopup(int& popOpt);

	bool loadMap(std::wstring wszMapFileName);
	bool saveMap(std::wstring wszMapFileName);

	int	getRenderedLeafNodeCount();

	static CPMapWorkspace& getInstance()
	{
		static CPMapWorkspace singleInst;
		return singleInst;
	}
};

#define MAP_SPACE	CPMapWorkspace::getInstance()
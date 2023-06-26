#include "CPGCIMGUIModule.h"

CPGCIMGUIModule::CPGCIMGUIModule()
{
	m_bEnable = false;
}

CPGCIMGUIModule::~CPGCIMGUIModule()
{
}

bool CPGCIMGUIModule::init(HWND hwnd, ID3D11Device* pDevice, ID3D11DeviceContext* pImDContext)
{
	m_bEnable = true;

	if (!hwnd || !pDevice || !pImDContext) { return false; }

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsLight();

	m_pImguiIO = &ImGui::GetIO();

	//한글 폰트 사용
	m_pImguiIO->Fonts->AddFontFromFileTTF("../../data/fonts/Pretendard/Pretendard-Bold.ttf", 15.0f, NULL, m_pImguiIO->Fonts->GetGlyphRangesKorean());

	//도킹 가능 옵션 설정
	m_pImguiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	//새로운 윈도우를 생성하여 대화상자를 처리한다.
	//m_pImguiIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(pDevice, pImDContext);

	return true;
}

bool CPGCIMGUIModule::update()
{
	if (m_bEnable)
	{
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame(); 
	}
	return true;
}

bool CPGCIMGUIModule::render()
{
	if (m_bEnable)
	{
		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (m_pImguiIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
	return true;
}

bool CPGCIMGUIModule::release()
{
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();

	ImGui::DestroyContext();

	return true;
}

bool CPGCIMGUIModule::toggleEnable()
{
	m_bEnable = !m_bEnable;
	return m_bEnable;
}

bool CPGCIMGUIModule::isEnable()
{
	return m_bEnable;
}
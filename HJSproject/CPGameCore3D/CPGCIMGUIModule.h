#pragma once
#include "CPGCStd.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImGUI\ImGuiFileDialog.h"

class CPGCIMGUIModule
{
private:
	bool			m_bEnable;
	ImGuiIO*		m_pImguiIO;

	CPGCIMGUIModule();
	~CPGCIMGUIModule();

public:
	CPGCIMGUIModule(const CPGCIMGUIModule& other) = delete;
	CPGCIMGUIModule& operator=(const CPGCIMGUIModule& other) = delete;

public:
	bool init(HWND hwnd, ID3D11Device* pDevice, ID3D11DeviceContext* pImDContext);
	bool update();
	bool render();
	bool release();

	bool toggleEnable();
	bool isEnable();

	static CPGCIMGUIModule& getInstance()
	{
		static CPGCIMGUIModule singleInst;
		return singleInst;
	}
};

namespace CPImGui
{
	inline int resizeCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			std::string* my_str = (std::string*)data->UserData;
			//IM_ASSERT(my_str->c_str() == data->Buf);
			my_str->resize(data->BufTextLen);
			data->Buf = &(*my_str)[0];
		}
		return 0;
	}

	inline bool InputString(const char* label, std::string* my_str, ImGuiInputTextFlags flags = 0)
	{
		//IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		return ImGui::InputText(label, &(*my_str)[0], (size_t)my_str->size() + 1, flags | ImGuiInputTextFlags_CallbackResize, CPImGui::resizeCallback, (void*)my_str);
	}

	inline bool InputStringMultiline(const char* label, std::string* my_str, ImVec2 size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0)
	{
		//IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		return ImGui::InputTextMultiline(label, &(*my_str)[0], (size_t)my_str->size() + 1, size, flags | ImGuiInputTextFlags_CallbackResize, CPImGui::resizeCallback, (void*)my_str);
	}
}

#define CPGC_IMGUI_MODULE CPGCIMGUIModule::getInstance()
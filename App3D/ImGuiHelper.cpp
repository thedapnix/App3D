#include "ImGuiHelper.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

void SetupImGui(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//Setup ImGui io
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	//Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	//Enable Multiple Viewports

	//Setup style
	ImGui::StyleColorsDark();

	//Setup backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);
}

void StartImGuiFrame() //ImGui functions called before deciding what to render
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiWindowMenu(bool& isActive)
{
	//Internal functionality for setting up window menus so we don't have to rewrite this anytime we make a new window
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open.."))
			{
				isActive = true;
			}
			if (ImGui::MenuItem("Close.."))
			{
				isActive = false;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void ImGuiEngineWindow(Camera* camera, std::string fps, int state)
{
	//Setup
	static bool window_active = true;
	ImGui::Begin("D3D11Engine", &window_active, ImGuiWindowFlags_MenuBar);

	//Menu
	ImGuiWindowMenu(window_active);

	//Print whatever information we want from this window
	if (window_active)
	{
		ImGui::Text("Current FPS: %s", fps.c_str());
		ImGui::Text("----------------------------------");
		ImGui::Text("Current State: %s", state == 0 ? "Player" : "Camera");
		ImGui::Text("----------------------------------");
		ImGui::Text("Camera Position: (%f, %f, %f)", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z);
		ImGui::Text("Camera is looking towards: (%f, %f, %f)", camera->GetLook().x, camera->GetLook().y, camera->GetLook().z);
		ImGui::Text("----------------------------------");
		ImGui::Text("Deferred Rendering: %s", true ? "Enabled" : "Disabled");
		ImGui::Text("OBJ Parsing: %s", true ? "Enabled" : "Disabled");
		ImGui::Text("Frustum Culling: %s", true ? "Enabled-ish" : "Disabled");
		ImGui::Text("LOD Tesselation: %s", false ? "Enabled" : "Disabled");	
		ImGui::Text("Cube Mapping: %s", false ? "Enabled" : "Disabled");
		ImGui::Text("Billboarding: %s", false ? "Enabled" : "Disabled");
		ImGui::Text("Shadow Mapping: %s", false ? "Enabled" : "Disabled");
		ImGui::Text("----------------------------------");
		if (ImGui::Button("Enable Deferred"))
		{
			//Set a boolean to true
		}
	}

	ImGui::End();
}

void EndImGuiFrame() //After the finalized frame has been decided upon, render and update
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

void ClearImGui() //Cleanup
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
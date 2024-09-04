#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <Windows.h>
#include <d3d11.h>
#include <string>

#include "D3D11Engine.h"

void SetupImGui(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);

void StartImGuiFrame();

void ImGuiEngineWindow(Camera* camera, std::string fps, int state,
	bool& deferredIsEnabled, bool& cullingIsEnabled, bool& billboardingIsEnabled, bool& lodIsEnabled, bool& cubemapIsEnabled,
	int drawables,
	int rawX, int rawY);

void EndImGuiFrame();

void ClearImGui();
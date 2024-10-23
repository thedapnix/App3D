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

//Time to start having fun with ImGui, and learning how to make more intensive windows
//What do we want from an engine application? I'm starting simple so I'm just going to make two things for now: A scene view and a "modifier" view for lack of a better term
//1. Scene view is probably the most important thing, so the thing that we usually render onto the screen of our window should be rendered onto an imgui window
//2. Modifier view is going to be what I've been having as the separate ImGui window up until now, just another ImGui window that has buttons to press that can alter what's going on in the scene view

/*
Updated imgui to the latest version, got demo window back up and working, but alas, the way my render pipeline is setup (realistically, the order in which I handle viewports and rendertargets) don't work
ImGui windows that are dragged to the win32 window disappear, thus making basic functionality (not to mention dockspace) impossible right now
To-do: Study the official imgui example (most importantly, take note of the order of rendering). I sure hope deferred isn't what's messing things up.
*/

void SetupImGui(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);

void StartImGuiFrame();

//Leave this as is for now, this is the Modifier view
void ImGuiEngineWindow(Camera* camera, std::string fps, int state,
	bool& deferredIsEnabled, bool& cullingIsEnabled, bool& billboardingIsEnabled, bool& lodIsEnabled, bool& cubemapIsEnabled,
	int drawables, int selectableDrawables,
	int rawX, int rawY);

//Make the scene view and see if it's shit or not
void ImGuiSceneWindow(Camera* camera, D3D11Engine* engine);

void EndImGuiFrame();

void ClearImGui();
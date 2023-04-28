#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <Windowsx.h>
#include <memory>

#include "Timer.h"
#include "Mouse.h"
#include "Keyboard.h"

#include "D3D11Engine.h"

class App
{
public:
	App(HINSTANCE hInstance);
	~App() = default;

	LRESULT HandleUserInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //Needs to be public for the macro in the cpp file to access

	int Run();

private:
	/*Functions*/
	void DoFrame(float dt);
	void InterpretKeyboardInput();
	void OnMouseMove(WPARAM btnState, int x, int y); //Don't want to have it like this but I gotta, for now

	/*Window Stuff*/
	bool InitWindow();
	HINSTANCE m_instance;
	HWND m_hwnd;
	int m_width;
	int m_height;
	const wchar_t* m_name;

	/*Variables*/
	Mouse m_mouse;
	std::pair<float, float> m_radians;
	Keyboard m_keyboard;
	std::unique_ptr<Timer> m_timer;
	std::unique_ptr<D3D11Engine> m_engine;
};
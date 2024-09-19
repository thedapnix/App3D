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

#include "LevelSetup.h" //Includes D3D11Engine and EntityFramework

enum class States
{
	INVALID = -1,
	NO_CONTROL, FPC_CONTROL, GODCAM_CONTROL
};

enum class Levels
{
	INVALID,

	LEVEL1,
	LEVEL2,
	LEVEL3,
	LEVEL4,
	LEVEL5,
	LEVEL6,
	LEVEL7,
	LEVEL8,
	LEVEL9,

	PRE1,
	PRE2,
	PRE3,
	PRE4,
	PRE5,
	PRE6,
	PRE7,
	PRE8,
	PRE9,

	GAMEOVER,
};

class App
{
public:
	App(HINSTANCE hInstance);
	~App() = default;

	LRESULT HandleUserInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //Needs to be public for the macro in the cpp file to access

	int Run();
	
	//Cursor stuff
	void EnableCursor();			//Show
	void DisableCursor();			//Hide
	void ContainCursor(bool clip);	//Keep within window bounds

private:
	/*Functions*/
	//Considering making DoSetup() and DoFrame() be virtual functions, so we make a game or application that inherits from this App-class and has to redefine those two, 
	//leaving everything else as backend
	void DoSetup();			//Called when the program starts
	void DoFrame(float dt);	//Called every frame

	void InterpretKeyboardInput(float dt);
	void OnMouseMove(WPARAM btnState, int x, int y); //Don't want to have it like this but I gotta, for now
	void OnMouseRawMove(WPARAM btnState, int x, int y); //new: oof

	void HideCursor();
	void ShowCursor();

	/*Window Stuff*/
	bool InitWindow();
	HINSTANCE m_instance;
	RECT m_windowRect; //New: store this, use this when creating the window, then reuse when we want to contain the cursor
	HWND m_hwnd;
	int m_width;
	int m_height;
	const wchar_t* m_name;

	/*Enum states*/
	Levels m_currentLevel = Levels::PRE1;
	States m_currentState = States::NO_CONTROL;

	/*ImGui variables*/
	bool m_fpsShouldUpdate = false;

	/*Variables*/
	std::unique_ptr<Mouse> m_mouse;
	std::pair<float, float> m_radians;
	std::unique_ptr<Keyboard> m_keyboard;
	std::unique_ptr<Timer> m_timer;
	std::unique_ptr<Timer> m_fpsTimer;
	std::unique_ptr<Timer> m_preattentiveTimer; //Aptly named
	std::unique_ptr<D3D11Engine> m_engine;

	std::vector<char> rawBuffer; //new: raw mouse data storage
	bool m_cursorContained = false; //new, contain the mouse within the window, for use in first person mode
	bool m_cursorEnabled = true;
	//bool m_windowIsDirty = false; //new

	float m_playerSpeed = 0.025f;
};
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN

#include "App.h"

#pragma comment( lib, "user32" )			// link against the win32 library
#pragma comment( lib, "d3d11.lib" )			// direct3D library
#pragma comment( lib, "dxgi.lib" )			// directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" )	// shader compiler

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow)
{
	//Thinking of taking a page out of the book's book (yeah that's a sentence)
	//Ditch the Window-class and have it be a part of the App-class

	App app(hInstance);
	app.Run();

	_CrtDumpMemoryLeaks();

	return 0;
}
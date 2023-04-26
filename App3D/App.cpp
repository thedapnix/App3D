#include "App.h"

//#include "ImGui/imgui_impl_win32.h"  //This imgui section is for allowing imgui access in the window proc function
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Macro that allows window to refer to itself via the "this" keyword
namespace
{
    App* d3dApp = 0;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //checks if the window is a imgui unit or not. If it is then we don't handle the msg.
    //if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    //    return true;

    return d3dApp->HandleUserInput(hWnd, msg, wParam, lParam);
}

App::App(HINSTANCE hInstance)
    :m_instance(hInstance),
    m_hwnd(0),
    m_name(L"3D Project"),
    m_width(1024), m_height(768)
{
    d3dApp = this;

    InitWindow();

    m_timer = std::make_unique<Timer>(); //Constructor should also Start()
    m_engine = std::make_unique<D3D11Engine>(m_hwnd, m_width, m_height);
}

int App::Run()
{
    MSG msg = { 0 };

    //Run as long as we haven't called to quit the game
    while (msg.message != WM_QUIT)
    {
        //If there are window messages, process them
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //Game loop
        float deltaTime = (float)m_timer->GetMilisecondsElapsed();
        m_timer->Restart();
        
        //Perform game logic things
        DoFrame(deltaTime);

        //Update and render scene afterwards
        m_engine->Update(deltaTime);
    }

    return (int)msg.wParam;
}

void App::DoFrame(float dt)
{
    //TODO: change this to use the actual keyboard- and mouse-classes instead of this dogshit. Mouse will also allow for rotating the camera through click-and-drag
    if (GetAsyncKeyState('W') & 0x8000)
        m_engine->GetCamera().Walk(0.25f);

    if (GetAsyncKeyState('S') & 0x8000)
        m_engine->GetCamera().Walk(-0.25f);

    if (GetAsyncKeyState('A') & 0x8000)
        m_engine->GetCamera().Strafe(-0.25f);

    if (GetAsyncKeyState('D') & 0x8000)
        m_engine->GetCamera().Strafe(0.25f);

    m_engine->GetCamera().UpdateViewMatrix();
}

LRESULT App::HandleUserInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        //User attempts to quit program via things like ALT+F4 or pressing the X in the top-right corner of the window
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

        /*
        MOUSE INPUT
        */
    case WM_LBUTTONDOWN:
        m_mouse.OnPress(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;

    case WM_LBUTTONUP:
        m_mouse.OnRelease();
        break;

        /*
        KEYBOARD INPUT
        */
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!(lParam & 0x40000000)) //|| m_keyboard.AutoRepeatIsEnabled())
        {
            m_keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        m_keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
        break;

    case WM_CHAR:
        //m_keyboard.OnChar(static_cast<unsigned char>(wParam));
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam); //the default window proc.
}

bool App::InitWindow()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_instance;
    //wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"Class Name";
    //wc.hIconSm = wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));;
    RegisterClassEx(&wc);

    //Create window
    RECT wr = {};
    wr.left = 100;
    wr.right = m_width + wr.left;
    wr.top = 100;
    wr.bottom = m_height + wr.top;
    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE); //Our width and height doesn't include the borders

    m_hwnd = CreateWindowEx(
        0,															// Optional window styles.
        L"Class Name",												// Window class
        m_name,													    // Window text (Title)
        CS_OWNDC | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,	// Window style, min max and close, don't allow resizing because it looks awful
        CW_USEDEFAULT, 0,											// Position and size
        m_width, m_height,
        nullptr,													// Parent window    
        nullptr,													// Menu
        m_instance,												    // Instance handle
        nullptr														// Additional application data
    );

    if (m_hwnd == nullptr)
    {
        std::cerr << "hWnd was null, last error: " << GetLastError() << std::endl;
    }
    else
    {
        //Show the window
        ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    }

    return true;
}
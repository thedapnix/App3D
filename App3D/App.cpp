#include "App.h"

#include "ImGui/imgui_impl_win32.h"  //This imgui section is for allowing imgui access in the window proc function
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Macro that allows window to refer to itself via the "this" keyword
namespace
{
    App* d3dApp = 0;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //checks if the window is a imgui unit or not. If it is then we don't handle the msg.
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

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

    m_mouse = std::make_unique<Mouse>();
    m_keyboard = std::make_unique<Keyboard>();
    m_timer = std::make_unique<Timer>(); //Constructor should also Start()
    m_fpsTimer = std::make_unique<Timer>();
    m_engine = std::make_unique<D3D11Engine>(m_hwnd, m_width, m_height);

    //Name of the obj file followed by Translate, Rotate, Scale (all of which have default values)
    m_engine->CreateDrawable("Meshes/ground.obj", { 0.0f, -10.0f, 5.0f });
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
        else //Since moving the camera using the mouse is currently reliant on windows messages, I have to separate these so as to not fuck up the camera when I try using keyboard and mouse at the same time
        {
            //Game loop
            float deltaTime = (float)m_timer->GetMilisecondsElapsed();
            m_timer->Restart();

            //Perform game logic things
            DoFrame(deltaTime);
        }
    }

    return (int)msg.wParam;
}

void App::DoFrame(float dt)
{
    //THIS FUNCTION IS THE ONLY THING YOU AS A USER NEED TO CARE ABOUT
    /*Camera stuff*/
    InterpretKeyboardInput(dt);
    m_engine->GetCamera().UpdateViewMatrix();

    /*Imgui stuff (Right now just an fps counter)*/
    if (m_fpsTimer->GetMilisecondsElapsed() > 1000.0f)
    {
        m_fpsShouldUpdate = true;
        m_fpsTimer->Restart();
    }
    m_engine->ImGuiSceneData(m_engine.get(), m_fpsShouldUpdate, (int)m_currentState);
    m_fpsShouldUpdate = false;

    /*Render the new scene*/
    m_engine->Update(dt);
}

void App::InterpretKeyboardInput(float dt)
{
    if (m_currentState == States::CAMERA_CONTROL)
    {
        if (m_keyboard->IsKeyPressed(VK_SPACE))
        {
            m_currentState = States::PLAYER_CONTROL;
        }
        if (m_keyboard->IsKeyPressed(0x57)) //W
        {
            m_engine->GetCamera().Walk(0.025f * dt);
        }
        if (m_keyboard->IsKeyPressed(0x53)) //S
        {
            m_engine->GetCamera().Walk(-0.025f * dt);
        }
        if (m_keyboard->IsKeyPressed(0x44)) //D
        {
            m_engine->GetCamera().Strafe(0.025f * dt);
        }
        if (m_keyboard->IsKeyPressed(0x41)) //A
        {
            m_engine->GetCamera().Strafe(-0.025f * dt);
        }

        if (m_keyboard->IsKeyPressed(0x50)) //P
        {
            m_engine->GetCamera().LookAt(m_engine->GetCamera().GetPosition(), { 0.0f, 0.0f, 1.0f }, m_engine->GetCamera().GetUp());
        }
    }

    /*Player*/
    if (m_currentState == States::PLAYER_CONTROL)
    {
        if (m_keyboard->IsKeyPressed(VK_ESCAPE))
        {
            m_currentState = States::CAMERA_CONTROL;
        }

        if (m_keyboard->IsKeyPressed(0x44)) //D
        {
            m_engine->MovePlayer(0.1f);
        }
        if (m_keyboard->IsKeyPressed(0x41)) //A
        {
            m_engine->MovePlayer(-0.1f);
        }
    }
}

void App::OnMouseMove(WPARAM btnState, int x, int y)
{
    /*Camera*/
    if (m_currentState == States::CAMERA_CONTROL)
    {
        if ((btnState & MK_LBUTTON) != 0)
        {
            // Make each pixel correspond to a quarter of a degree.
            float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_mouse->GetLastPosX()));
            float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_mouse->GetLastPosY()));

            m_engine->GetCamera().Pitch(dy);
            m_engine->GetCamera().RotateY(dx);
        }

        m_mouse->SetLastPosX(x);
        m_mouse->SetLastPosY(y);
    }

    /*Player (probably won't do anything here)*/
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
        m_mouse->OnPress(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;

    case WM_LBUTTONUP:
        m_mouse->OnRelease();
        break;

    case WM_MOUSEMOVE:
        OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;

        /*
        KEYBOARD INPUT
        */
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!(lParam & 0x40000000))
            m_keyboard->OnKeyPressed(static_cast<unsigned char>(wParam));
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        m_keyboard->OnKeyReleased(static_cast<unsigned char>(wParam));
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
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
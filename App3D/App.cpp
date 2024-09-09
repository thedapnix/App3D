#include "App.h"

#include "ImGui/imgui_impl_win32.h"  //This imgui section is for allowing imgui access in the window proc function
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "LevelSetup.h"

//To allow for key events happening only once upon releasing a key, change the way keyboard input works
//Every frame (tick), RESET, then GET input, this way we will now if a key has been released during this iteration
//

//What: Store an empty pointer of type "App" and assign it the value of "this" in the App-constructor
//Why:  Windows static functions like MainWndProc don't allow the use of the "this" keyword, so this is a workaround
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
    m_preattentiveTimer = std::make_unique<Timer>();
    m_engine = std::make_unique<D3D11Engine>(m_hwnd, m_width, m_height);

    //Raw mouse input (not sure I'll need this but fuck it we ball)
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = 0;
    rid.hwndTarget = nullptr;
    if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
    {
        //throw an exception or something idk
    }

    DoSetup();
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

void App::EnableCursor()
{
    m_cursorEnabled = true;
    ShowCursor();
}

void App::DisableCursor()
{
    m_cursorEnabled = false;
    HideCursor();
}


void App::DoSetup()
{
    m_engine->GetCamera().SetPosition({ 0.0f, 12.0f, 0.0f }); //Previously 5.675 Z

    SetupLevel1(m_engine.get());    //Motion: X-axis        (B3/B2)
    //SetupLevel2(m_engine.get());    //Color: Red            (B3)
    //SetupLevel3(m_engine.get());    //Form: Oblong          (C1)
    //SetupLevel4(m_engine.get());    //Color: Blue           (B1)
    //SetupLevel5(m_engine.get());    //Motion: Y-axis        (C1/B1)
    //SetupLevel6(m_engine.get());    //Form: Sphere          (B2/B3)
    //SetupLevel7(m_engine.get());    //Motion: Z-axis        (C3)
    //SetupLevel8(m_engine.get());    //Color: Blue outline   (B3/C3)
    //SetupLevel9(m_engine.get());    //Form: Parallelepiped  (B1/B2/C1/C2) (aka big skewed cube)

    m_engine->CreateDrawable("Meshes/gun2.obj", { -1.0f, 10.85f, 1.75f }, {2.0f, 2.0f, 2.0f});
    //m_engine->CreateDrawable("Meshes/donut2.obj", { 0.0f, 10.0f, 0.0f });

    //m_engine->CreateLightSpot({ 0.0f + 540.0f, 5.0f, 5.0f }, 0.75f, 0.0f, 0.35f); //Light on the final grid crate

    //Call after every drawable and light for a scene has been created to initialize functionality for things like culling and shadows
    //(find a way to make this automatically happen after the DoSetup() function without needing the user to worry about this stuff?)
    m_engine->SetupQT();
    m_engine->SetupLights();
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

    m_engine->ImGuiSceneData(m_engine.get(), m_fpsShouldUpdate, (int)m_currentState, m_mouse->GetLastRawPosX(), m_mouse->GetLastRawPosY());
    m_fpsShouldUpdate = false;

    /*Render the new scene*/
    m_engine->Update(dt);

    //This can be set to true by the HandleUserInput() function, and will then be used in the InterpretKeyboardInput() function if it's appropriate
    m_keyboard->ResetReleaseInfo();
}

void App::InterpretKeyboardInput(float dt)
{
    /*State switching using the keys 1, 2, and 3 (No control, FPS player control, free camera control)*/
#ifdef _DEBUG
    if (m_keyboard->IsKeyPressed(0x31))
    {
        m_currentState = States::NO_CONTROL;
    }
    else if (m_keyboard->IsKeyPressed(0x32))
    {
        m_currentState = States::FPC_CONTROL;
        DisableCursor();
    }
    else if (m_keyboard->IsKeyPressed(0x33))
    {
        m_currentState = States::GODCAM_CONTROL;
        EnableCursor();
    }
#endif

    /*Player*/
    //Probably want to add another layer of abstraction and let these playermovement and interaction be defined in here, because the app-class isn't as user-friendly as i want it to be lol
    if (m_currentState == States::FPC_CONTROL)
    {
        if (m_keyboard->IsKeyPressed(0x57)) //W
        {
            m_engine->MovePlayerZ( 0.02f * dt);
        }
        if (m_keyboard->IsKeyPressed(0x53)) //S
        {
            m_engine->MovePlayerZ(-0.02f * dt);
        }

        if (m_keyboard->IsKeyPressed(0x44)) //D
        {
            m_engine->MovePlayerX( 0.02f * dt);
        }
        if (m_keyboard->IsKeyPressed(0x41)) //A
        {
            m_engine->MovePlayerX(-0.02f * dt);
        }

        if (m_keyboard->GetReleaseInfo().keyCode == 0x45 && m_keyboard->GetReleaseInfo().wasReleasedThisTick)
        {
            m_engine->PlayerInteract();
        }

    }

    /*Camera*/
    if (m_currentState == States::GODCAM_CONTROL)
    {
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

    /*None*/
    else
    {

    }
}

void App::OnMouseMove(WPARAM btnState, int x, int y)
{
    //God Camera
    if ((btnState & MK_LBUTTON) != 0) //If the left mouse button is being held down
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

void App::OnMouseRawMove(WPARAM btnState, int x, int y)
{
    //FPS Camera
    int prevX = m_mouse->GetLastRawPosX();
    int prevY = m_mouse->GetLastRawPosY();

    m_mouse->UpdateLastRawPosX(x);  //Increments or decrements
    m_mouse->UpdateLastRawPosY(y);  //

    // Make each pixel correspond to a quarter of a degree.
    float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(m_mouse->GetLastRawPosX() - prevX));
    float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(m_mouse->GetLastRawPosY() - prevY));

    m_engine->GetCamera().PitchFPC(dy);
    m_engine->GetCamera().RotateY(dx);
}

//Both of these are loops that call upon the WinAPI calls with the same name (maybe I should name these differently just to be extra sure the compiler doesn't confuse them)
void App::HideCursor()
{
    while (::ShowCursor(FALSE) >= 0);
}

//See comment above
void App::ShowCursor()
{
    while (::ShowCursor(TRUE) < 0);
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
        if (m_currentState != States::GODCAM_CONTROL) break; //Save time calculating
        OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;

        /*
        KEYBOARD INPUT
        */
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!(lParam & 0x40000000)) //0x40000000 is bit 30, which according to the windows documentation is the important thing when making sure we don't layer keypresses
            m_keyboard->OnKeyPressed(static_cast<unsigned char>(wParam));
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        m_keyboard->OnKeyReleased(static_cast<unsigned char>(wParam));
        break;

    case WM_INPUT: //Raw input data, idk why but this has to be after the other cases
        if (m_currentState != States::FPC_CONTROL) break; //Save time calculating
        UINT size;
        //Get size of input data
        if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1) //Fills the size-variable with the bytes required
        {
            //throw an exception or something idk
            break;
        }
        rawBuffer.resize(size);

        //Read input data
        if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
        {
            //throw an exception or something idk
            break;
        }

        //Process input data
        auto& rid = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
        if (rid.header.dwType == RIM_TYPEMOUSE && (rid.data.mouse.lLastX != 0 || rid.data.mouse.lLastY != 0))
        {
            //m_mouse.get()->OnRawDelta(rid.data.mouse.lLastX, rid.data.mouse.lLastY);
            OnMouseRawMove(wParam, (int)rid.data.mouse.lLastX, (int)rid.data.mouse.lLastY);
        }
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
#include "App.h"

#include "ImGui/imgui_impl_win32.h"  //This imgui section is for allowing imgui access in the window proc function
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "LevelSetup.h"

//Offsets from the camera position to the pov-gun, remains constant as it will always follow the camera using matrix magic
#define POV_OFFSET_X -1.00f
#define POV_OFFSET_Y -1.10f
#define POV_OFFSET_Z  1.75f

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

void App::ContainCursor(bool clip)
{
    //Plain and simple?
    if (!clip)
    {
        ClipCursor(NULL);
        return;
    }

    //Setup an area that we want to confine the cursor within
    RECT newRect;

    //if (m_windowIsDirty) //If we've moved the window, move the rect to align with it
    //{
        //GetWindowRect(m_hwnd, &newRect);
    //}
    //else //Otherwise, just set it to be the previous values
    //{
        //newRect = m_windowRect;  
    //}

    //We're not checking whether or not we NEED to get a new rect, nor do we call this function again if the window is ever moved while we're containing the cursor
    //This is fine since if the cursor is being contained, that means we can't really move the window anyway (you *CAN* but don't be a prick, I can make it idiot-proof later)
    GetWindowRect(m_hwnd, &newRect); //Might just not bother with the above, if just calling GetWindowRect() isn't an expensive call

    newRect.left += 0.25f * m_width;
    newRect.right -= 0.25f * m_width;
    newRect.top += 0.25f * m_height;
    newRect.bottom -= 0.25f * m_height;
    
    ClipCursor(&newRect); //Don't allow the cursor to leave these bounds (rawData still works, which is why we use this for fps controls)
}


void App::DoSetup()
{
//Default state if we're not in debug, aka we're running the exe
#ifndef _DEBUG
    m_currentState = States::FPC_CONTROL;
    m_cursorContained = true;
    ContainCursor(true);
    DisableCursor();
#endif

    //For the intended level
    //m_engine->GetCamera().SetPosition({ -5.0f, 12.0f, 10.0f });
    //m_engine->GetCamera().RotateY(DirectX::XM_PIDIV2);

    //For the test level
    m_engine->GetCamera().SetPosition({ 0.0f, 12.0f, -20.0f });
    //m_engine->GetCamera().Pitch(0.1f);

    //Gun (Remove when appropriate, obvi)
    /*
    m_engine->CreatePovDrawable("Meshes/gun.obj", { POV_OFFSET_X, POV_OFFSET_Y, POV_OFFSET_Z }, { 2.0f, 2.0f, 2.0f });

    m_engine->GetPovDrawables().at(0).SetPosition(
        POV_OFFSET_X,
        POV_OFFSET_Y,
        POV_OFFSET_Z
    );
    */

    //SetupLevel1(m_engine.get());
    //SetupLevel2(m_engine.get()); //Changing the level design
    //SetupTestLevel(m_engine.get());
    SetupInstancedLevel(m_engine.get());

    //Test crate that spins around to see how well normalmapping is working
    //int crateIdx = m_engine->CreateDrawable("Meshes/crate_wood.obj", { 0.0f, 12.0f, 30.0f });
    //m_engine->ApplyNormalMapToDrawable(crateIdx, "NormalMaps/wood.jpg");

    //int crateIdx = m_engine->CreateDrawable("Meshes/Test/crate.obj", { 0.0f, 12.0f, 5.0f });
    //m_engine->ApplyNormalMapToDrawable(crateIdx, "NormalMaps/wood_fancy_invert.png");

    //Cubemap(s) I'm just limit testing cool shit
    //m_engine->CreateReflectiveDrawable("Meshes/default_sphere.obj", { -17.5f, 10.0f, 55.0f }, {2.0f, 2.0f, 2.0f}); //Big floating sphere in the big room, between the purple and cyan light
    //m_engine->CreateReflectiveDrawable("Meshes/default_sphere.obj", { 7.0f, 6.0f, 11.0f }); //should be on top of the {7.0f, 3.0f, 11.0f} box, since default sphere is a unit -1 to +1 sphere
    //Accidentally just found out that I can try to have fun with portals by having two reflective objects that use the same cubemap

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

    /*Render the new scene*/
    m_engine->Update(dt);

    /*Imgui stuff (I think it's important that the engine has done its rendering stuff first?)*/
//#ifdef _DEBUG
//    if (m_fpsTimer->GetMilisecondsElapsed() > 1000.0f)
//    {
//        m_fpsShouldUpdate = true;
//        m_fpsTimer->Restart();
//    }
//    m_engine->ImGuiSceneData(m_engine.get(), m_fpsShouldUpdate, (int)m_currentState, m_mouse->GetLastRawPosX(), m_mouse->GetLastRawPosY());
//    m_fpsShouldUpdate = false;
//#endif

    //This can be set to true by the HandleUserInput() function, and will then be used in the InterpretKeyboardInput() function if it's appropriate
    m_keyboard->ResetReleaseInfo();
}

void App::InterpretKeyboardInput(float dt)
{
    /*State switching using the keys 1, 2, and 3 (No control, FPS player control, free camera control)*/
#ifdef _DEBUG
    //Teleport to different levels by holding down L and pressing the keys 1 through 6
    if (m_keyboard->IsKeyPressed(0x4C))
    {
        if (m_keyboard->IsKeyPressed(0x31))
        {
            m_engine->GetCamera().SetPosition({ -5.0f, 12.0f, 10.0f });
        }
        else if (m_keyboard->IsKeyPressed(0x32))
        {
            m_engine->GetCamera().SetPosition({ -5.0f + 60.0f, 12.0f, 10.0f + 60.0f });
        }
        else if (m_keyboard->IsKeyPressed(0x33))
        {
            m_engine->GetCamera().SetPosition({ -5.0f + 120.0f, 12.0f, 10.0f + 120.0f });
        }
        else if (m_keyboard->IsKeyPressed(0x34))
        {
            m_engine->GetCamera().SetPosition({ -5.0f + 180.0f, 12.0f, 10.0f + 180.0f });
        }
        else if (m_keyboard->IsKeyPressed(0x35))
        {
            m_engine->GetCamera().SetPosition({ -5.0f + 240.0f, 12.0f, 10.0f + 240.0f });
        }
        else if (m_keyboard->IsKeyPressed(0x36))
        {
            m_engine->GetCamera().SetPosition({ -5.0f + 300.0f, 12.0f, 10.0f + 300.0f });
        }
    }
    else //If not holding down L the keys 1, 2, and 3 swap between different camera states
    {
        if (m_keyboard->IsKeyPressed(0x31))
        {
            m_currentState = States::NO_CONTROL;
            m_cursorContained = false;
        }
        else if (m_keyboard->IsKeyPressed(0x32))
        {
            m_currentState = States::FPC_CONTROL;
            m_cursorContained = true;
            ContainCursor(true);
            DisableCursor();
        }
        else if (m_keyboard->IsKeyPressed(0x33))
        {
            m_currentState = States::GODCAM_CONTROL;
            m_cursorContained = false;
            ContainCursor(false);
            EnableCursor();
        }
    }
#endif

    if (m_keyboard->IsKeyReleased(0x1B)) //Escape
    {
        if (MessageBox(NULL, L"Are you sure you want to quit the game?", L"Exit", MB_YESNO) == IDYES)
        {
            DestroyWindow(m_hwnd);
            return;
        }
    }

    /*Player*/
    //Probably want to add another layer of abstraction and let these playermovement and interaction be defined in here, because the app-class isn't as user-friendly as i want it to be lol
    if (m_currentState == States::FPC_CONTROL)
    {
        if (m_keyboard->IsKeyPressed(0x57)) //W
        {
            m_engine->MovePlayerZ( m_playerSpeed * dt);
        }
        if (m_keyboard->IsKeyPressed(0x53)) //S
        {
            m_engine->MovePlayerZ( -m_playerSpeed * dt);
        }

        if (m_keyboard->IsKeyPressed(0x44)) //D
        {
            m_engine->MovePlayerX( m_playerSpeed * dt);
        }
        if (m_keyboard->IsKeyPressed(0x41)) //A
        {
            m_engine->MovePlayerX(-m_playerSpeed * dt);
        }

        if (m_keyboard->IsKeyReleased(0x45)) //If it was specifically released on this tick
        {
            m_engine->PlayerInteract();
        }

        /*if (m_keyboard->GetReleaseInfo().keyCode == 0x45 && m_keyboard->GetReleaseInfo().wasReleasedThisTick)
        {
            m_engine->PlayerInteract();
        }*/

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
        if (m_currentState != States::GODCAM_CONTROL) break;
        //const POINTS pts = MAKEPOINTS(lParam); //Get the x- and y-coordinates of the mouse 
        //(nah, this doesn't work with raw movement, only for things like clicking and dragging outside the window bounds and making sure stuff still works, 
        //which is still important, but not what i'm looking for right now)
        //if (pts.x >= 0 && pts.x < m_width && pts.y >= 0 && pts.y < m_height)
        //{
        //    OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        //}
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
    //RECT wr = {};
    RECT adjustedRect;
    UINT offsetX = 125;
    UINT offsetY = 100;

    //Setup the "regular" rect used for containing the cursor within the bounds of the window
    m_windowRect.left = 0;
    m_windowRect.right = m_width;
    m_windowRect.top = 0;
    m_windowRect.bottom = m_height;

    //Setup the adjusted rect for window creation
    adjustedRect = m_windowRect;
    adjustedRect.left += offsetX;
    adjustedRect.right += offsetX;
    adjustedRect.top += offsetY;
    adjustedRect.bottom += offsetY;

    AdjustWindowRect(&adjustedRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE); //Our width and height doesn't include the borders

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
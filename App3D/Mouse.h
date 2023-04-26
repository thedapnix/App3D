#pragma once
#include <Windows.h>
#include <DirectXMath.h>

//Class may get a Keyboard-style makeover, using an embedded Event-class keeping track of buttons being pressed or released

class Mouse
{
public:
	Mouse();
	~Mouse() = default;

	void OnPress(HWND& window, int x, int y);
	void OnRelease();
	void OnMove(WPARAM btnState, int x, int y);

	bool IsLMBPressed();
private:
	bool m_lmbPressed = false;
	DirectX::XMFLOAT2 m_lastPos;
};
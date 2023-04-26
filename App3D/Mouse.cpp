#include "Mouse.h"

Mouse::Mouse()
{
	m_lastPos.x = 0.0f;
	m_lastPos.y = 0.0f;
}

void Mouse::OnPress(HWND& window, int x, int y)
{
	SetCapture(window);
	m_lmbPressed = true;
}

void Mouse::OnRelease()
{
	ReleaseCapture();
	m_lmbPressed = false;
}

void Mouse::OnMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		//Click-and-drag functionality if needed
	}

	m_lastPos.x = (float)x;
	m_lastPos.y = (float)y;
}

bool Mouse::IsLMBPressed()
{
	return m_lmbPressed;
}

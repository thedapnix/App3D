#include "Mouse.h"

Mouse::Mouse()
{
	m_lastPos.first = 0;
	m_lastPos.second = 0;
}

void Mouse::OnPress(HWND& window, int x, int y)
{
	m_lastPos = { x, y };
	SetCapture(window);
}

void Mouse::OnRelease()
{
	ReleaseCapture();
}

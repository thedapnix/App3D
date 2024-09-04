#include "Mouse.h"

Mouse::Mouse()
{
	m_lastPos.first = 0;
	m_lastPos.second = 0;

	//new
	m_lastRawPos.first = 0;
	m_lastRawPos.second = 0;
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

void Mouse::OnRawDelta(int dx, int dy)
{

}

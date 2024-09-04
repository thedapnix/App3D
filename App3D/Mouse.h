#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <Windows.h>
#include <DirectXMath.h>

#include <utility>

//Class may get a Keyboard-style makeover, using an embedded Event-class keeping track of buttons being pressed or released

class Mouse
{
public:
	Mouse();
	~Mouse() = default;
	Mouse& operator=(const Mouse&) = delete;
	Mouse(const Mouse&) = delete;

	void OnPress(HWND& window, int x, int y);
	void OnRelease();
	void OnRawDelta(int dx, int dy);

	int GetLastPosX() { return m_lastPos.first; }
	int GetLastPosY() { return m_lastPos.second; }

	int GetLastRawPosX() { return m_lastRawPos.first; }	//new
	int GetLastRawPosY() { return m_lastRawPos.second; }//

	void SetLastPosX(int x) { m_lastPos.first = x; }
	void SetLastPosY(int y) { m_lastPos.second = y; }

	void UpdateLastRawPosX(int x) { m_lastRawPos.first += x; }
	void UpdateLastRawPosY(int y) { m_lastRawPos.second += y; }

private:
	bool m_lmbPressed = false;
	std::pair<int, int> m_lastPos;
	std::pair<int, int> m_lastRawPos; //new
};
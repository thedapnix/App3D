#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <chrono>

class Timer
{
public:
	Timer();
	~Timer() = default;
	Timer& operator=(const Timer&) = delete;
	Timer(const Timer&) = delete;

	/*bool Start();
	bool Stop();*/
	void Restart();

	double GetMilisecondsElapsed();

private:
	std::chrono::time_point<std::chrono::steady_clock> m_startPoint;
	std::chrono::time_point<std::chrono::steady_clock> m_stopPoint;
	bool m_isRunning = true;
};
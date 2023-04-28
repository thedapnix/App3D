#include "Timer.h"

using namespace std::chrono;

Timer::Timer()
{
	m_startPoint = steady_clock::now();
	m_stopPoint = steady_clock::now();

	m_isRunning = true;
}

//bool Timer::Start()
//{
//	return true;
//}
//
//bool Timer::Stop()
//{
//	return false;
//}

void Timer::Restart()
{
	m_isRunning = true;
	m_startPoint = steady_clock::now();
}

double Timer::GetMilisecondsElapsed()
{
	if (m_isRunning)
	{
		auto elapsedTime = duration<double, std::milli>(steady_clock::now() - m_startPoint);
		return elapsedTime.count();
	}
	else
	{
		auto elapsedTime = duration<double, std::milli>(m_stopPoint - m_startPoint);
		return elapsedTime.count();
	}
}

#include "Keyboard.h"

bool Keyboard::IsKeyPressed(unsigned char keyCode)
{
    return m_keyStates[keyCode];
}

void Keyboard::OnKeyPressed(unsigned char keyCode)
{
	m_keyStates[keyCode] = true; //Yes, the key has been pressed. Will be set to false in the OnKeyReleased() function
	m_keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keyCode)); //Push the relevant event to our queue
	TrimBuffer(m_keyBuffer); //Trim, aka pop from the buffer if its size is greater than what we allow as the maximum
}

void Keyboard::OnKeyReleased(unsigned char keyCode)
{
	m_keyStates[keyCode] = false;
	m_keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keyCode));
	TrimBuffer(m_keyBuffer);
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer)
{
	while (buffer.size() > m_bufferSize)
	{
		buffer.pop();
	}
}
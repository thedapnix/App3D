#pragma once
#include <Windows.h>
#include <queue>
#include <optional>
#include <bitset>
//#include "ControllerEnums.hpp"

/*
NOTE ON EVERYTHING THAT'S COMMENTED OUT HERE:
Not deleting these because they'll be necessary if I decide (in the future) to have functionality for reading ALL KEYS, but right now it's just not needed
*/

class Keyboard
{
public:
	friend class App;

	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
		};
		Event(Type type, unsigned char keyCode)
			:type(type), keyCode(keyCode) {}
		bool IsPressed() { return this->type == Type::Press; }
		bool IsReleased() { return this->type == Type::Release; }
		unsigned char GetKeyCode() { return this->keyCode; }
	private:
		Type type;
		unsigned char keyCode;
	};

	Keyboard() = default;
	~Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	//Key Events
	bool IsKeyPressed(unsigned char keyCode);

	//std::optional<Event> ReadKey();
	//bool KeyIsEmpty();
	//void ClearKey();

	//Char Events
	//std::optional<char> ReadChar();
	//bool CharIsEmpty();
	//void ClearChar();

	//void EnableAutoRepeat();
	//void DisableAutoRepeat();
	//bool AutoRepeatIsEnabled();
private:
	void OnKeyPressed(unsigned char keyCode);
	void OnKeyReleased(unsigned char keyCode);
	//void OnChar(char character);
	//void ClearStates();

	template<typename T>
	void TrimBuffer(std::queue<T>& buffer);

private:
	static constexpr UINT m_keys = 256u;		//All possible key-presses (static constexpr needed for bitset to accept, might as well do the same to bufferSize, as these will never change)
	static constexpr UINT m_bufferSize = 16u;	//Maximum amount of key-presses we allow at one time
	std::bitset<m_keys> m_keyStates;			//true or false values for every possible key

	bool m_autoRepeatEnabled = false;

	std::queue<Event> m_keyBuffer;
	//std::queue<char> charBuffer;
};

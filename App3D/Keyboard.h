#pragma once
#include <Windows.h>
#include <queue>
#include <optional>
#include <bitset>
//#include "ControllerEnums.hpp"

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

	//No need for anything fancy on the constructor/destructor
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

	//We could also have a ClearAll() function that calls both ClearKey() and ClearChar()? Not sure we'll need it

	/*
	Actual functions called by the GameEngine-class
	We could avoid having these as public by setting Keyboard to have GameEngine as a friend class, thoughts?
	*/
private:
	void OnKeyPressed(unsigned char keyCode);
	void OnKeyReleased(unsigned char keyCode);
	//void OnChar(char character);
	//void ClearStates();

	template<typename T>
	void TrimBuffer(std::queue<T>& buffer);
private:
	//Put the functions above here if we go for the friend-class solution

private:
	static constexpr UINT m_keys = 256u;		//All possible key-presses (static constexpr needed for bitset to accept, might as well do the same to bufferSize, as these will never change)
	static constexpr UINT m_bufferSize = 64u;	//Maximum amount of key-presses we allow at one time
	std::bitset<m_keys> m_keyStates;			//true or false values for every possible key

	bool m_autoRepeatEnabled = false;

	//As a result of not being able to press multiple buttons, we now have queues filled with key events and characters
	std::queue<Event> m_keyBuffer;
	//std::queue<char> charBuffer;
};

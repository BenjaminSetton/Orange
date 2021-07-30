#include "../Misc/pch.h"

#include "Input.h"
#include "../Core/ApplicationHandle.h"

std::bitset<256> Input::m_keys = std::bitset<256>(false);

Input::Input() : EventObserver(EventCategory::KEYBOARD){}
Input::~Input(){}


void Input::KeyUp(unsigned int key)
{
	m_keys[key] = false;
}
void Input::KeyDown(unsigned int key) 
{
	m_keys[key] = true;
}
bool Input::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}

void Input::OnEvent(const Event& event)
{

	VX_LOG_INFO("OnEvent is called");
}

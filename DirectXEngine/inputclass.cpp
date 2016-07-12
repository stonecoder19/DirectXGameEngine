#include "inputclass.h"


InputClass::InputClass()
{

}

InputClass::InputClass(const InputClass& inputClass)
{

}

InputClass::~InputClass()
{

}

void InputClass::Initialize()
{
	int i;

	//initialize all keys to not being pressed
	for (i = 0;i < 256;i++)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	m_keys[input] = false;
	return;
}

bool InputClass::isKeyDown(unsigned int input)
{
	return m_keys[input];
}
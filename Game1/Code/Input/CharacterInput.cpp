#include "CharacterInput.h"
#include <Input/Listener.h>
#include <Input/Constants.h>


void Input::CharacterInput::feed(Input::Listener * inp, float dt)
{
	(void)dt;
	bool left = false;
	bool right = false;
	bool down = false;
	bool up = false;
	bool j = false;
	leftX_ = .0f;
	leftY_ = .0f;
	
	if ( 
		inp->isKeyPressed(Key::Left) || 
		( inp->isPadActive( 0 ) && ( inp->getPadLeftStick(0).getX() < -.5f || inp->getPadButton(0, Pad::Left) ) )
		)
	{
		left = true;
	}
	if (
		inp->isKeyPressed(Key::Right) ||
		(inp->isPadActive(0) && (inp->getPadLeftStick(0).getX() > .5f || inp->getPadButton(0, Pad::Right)))
		)
	{
		right = true;
	}
	if (
		inp->isKeyPressed(Key::Down) ||
		(inp->isPadActive(0) && (inp->getPadLeftStick(0).getY() < -.5f || inp->getPadButton(0, Pad::Down)))
		)
	{
		down = true;
	}
	if (
		inp->isKeyPressed(Key::Up) ||
		(inp->isPadActive(0) && (inp->getPadLeftStick(0).getY() > .5f || inp->getPadButton(0, Pad::Up)))
		)
	{
		up = true;
	}


	if (
		inp->isKeyPressed(Key::Space) || (inp->isPadActive(0) && inp->getPadButton(0, Pad::A) )
		)
	{
		j = true;
	}

	jump = j;
	if (left)
	{
		leftX_ += -1.0f;
	}
	if (right)
	{
		leftX_ += 1.0f;
	}
	if (down)
	{
		leftY_ -= 1.0f;
	}
	if (up)
	{
		leftY_ += 1.0f;
	}
}

Input::CharacterInput::CharacterInput()
{
}


Input::CharacterInput::~CharacterInput()
{
}

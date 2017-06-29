#include "MenuInput.h"
#include <Input/Listener.h>
#include <Input/Constants.h>


Input::MenuInput::MenuInput()
{
}


Input::MenuInput::~MenuInput()
{
}

void Input::MenuInput::feed(Input::Listener * inp, float dt)
{
	downTimer_ = max(.0f, downTimer_ - dt);
	upTimer_ = max(.0f, upTimer_ - dt);
	leftTimer_ = max(.0f, leftTimer_ - dt);
	rightTimer_ = max(.0f, rightTimer_ - dt);
	up = false;
	down = false;
	left = false;
	right = false;
	enter = false;
	start = false;
	float leftRightChange = .1f;
	if (leftRightImmediate_)
	{
		leftRightChange = .0f;
	}

	for (unsigned int i = 0; i < 4; ++i)
	{
		if (inp->isPadActive(i))
		{
			if (inp->getPadButtonOnce(i, Pad::Start) ) start = true;
			if (inp->getPadButtonOnce(i, Pad::A)) enter = true;
			if (inp->getPadButtonOnce(i, Pad::Up)) { up = true; upTimer_ = .2f; }
			if (inp->getPadButtonOnce(i, Pad::Down)) { down= true; downTimer_ = .2f; }
			if (inp->getPadButtonOnce(i, Pad::Left)) { left = true; leftTimer_ = .1f; }
			if (inp->getPadButtonOnce(i, Pad::Right)) { right = true; rightTimer_ = .1f; }
			vmath::Vector3 leftStick = inp->getPadLeftStick(i);
			if (leftStick.getY() > .8f && upTimer_ == .0f) { up = true; upTimer_ = .2f; }
			if (leftStick.getY() <-.8f && downTimer_ == .0f) { down = true; downTimer_ = .2f; }
			if (leftStick.getX() < -.8f && leftTimer_ == .0f) { left = true; leftTimer_ = leftRightChange; }
			if (leftStick.getX() < .8f && rightTimer_ == .0f) { right = true; rightTimer_ = leftRightChange; }
		}
	}
	

	if (inp->getKeyDown(Key::Escape)) start = true;
	if (inp->getKeyDown(Key::Enter)) enter = true;
	if (inp->getKeyDown(Key::Up)) { up = true; upTimer_ = .2f; }
	if (inp->getKeyDown(Key::Down)) { down = true; downTimer_ = .2f; }
	if (inp->isKeyPressed(Key::Up) && upTimer_ == .0f) { up = true; upTimer_ = .2f; }
	if (inp->isKeyPressed(Key::Down) && downTimer_ == .0f) { down = true; downTimer_ = .2f; }
	if (inp->isKeyPressed(Key::Left) && leftTimer_ == .0f) { left = true; leftTimer_ = leftRightChange; }
	if (inp->isKeyPressed(Key::Right) && rightTimer_ == .0f) { right= true; rightTimer_ = leftRightChange; }

}

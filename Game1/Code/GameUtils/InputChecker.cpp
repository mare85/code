#include "InputChecker.h"
#include <Input/Listener.h>
#include <Game/Transform.h>
#include <cstdio>

GameUtils::InputChecker::InputChecker(Game::Text* text)
	: text_( text )
{
}


GameUtils::InputChecker::~InputChecker()
{
}

void GameUtils::InputChecker::start()
{
	text_->setText("Keynumber            ");
}

void GameUtils::InputChecker::update(const Game::UpdateContext * uctx)
{
	for (int i = 0; i < 256; ++i)
	{
		if (uctx->input->isKeyPressed((unsigned char)i))
		{
			char str[256];
			sprintf_s(str, "Keynumber : %d", i);
			text_->setText(str);
			return;
		}
	}
	text_->setText("Keynumber");

	if ( uctx->input->isPadActive( 0 ) )
	{
		if (leftStickTransform)
		{
			leftStickTransform->setTranslation(uctx->input->getPadLeftStick( 0 ) - vmath::Vector3( .5f, .0f, .0f ));
			leftStickTransform->setScale(vmath::Vector3(1.0f - uctx->input->getPadLeftTrigger(0)));
		}
		if (rightStickTransform)
		{
			rightStickTransform->setTranslation(uctx->input->getPadRightStick(0) + vmath::Vector3(.5f, .0f, .0f));
			rightStickTransform->setScale(vmath::Vector3(1.0f - uctx->input->getPadRightTrigger(0)));
		}
	}
	else
	{
		if (leftStickTransform)
		{
			leftStickTransform->setTranslation(vmath::Vector3(.0f));
			leftStickTransform->setScale(vmath::Vector3(1.0f));
		}
		if (rightStickTransform)
		{
			rightStickTransform->setTranslation(vmath::Vector3(.0f));
			rightStickTransform->setScale(vmath::Vector3(1.0f));
		}
	}



}

void GameUtils::InputChecker::stop()
{
}

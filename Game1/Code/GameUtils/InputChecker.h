#pragma once

#include <Game/Updater.h>
#include <Game/Text.h>

namespace GameUtils
{
	class InputChecker
		: public Game::Updater
	{
		Game::Text* text_ = nullptr;
	public:
		InputChecker(Game::Text* text);
		~InputChecker();

		Game::Transform* leftStickTransform = nullptr;
		Game::Transform* rightStickTransform = nullptr;

		void start();
		void update(const Game::UpdateContext* uctx);
		void stop();
	};

}
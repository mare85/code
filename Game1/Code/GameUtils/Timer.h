#pragma once
#include <Game/Updater.h>
#include <Game/Text.h>


namespace GameUtils
{
	class Timer :
		public Game::Updater
	{
		Game::Text* text_ = nullptr;
		float time_ = .0f;
	public:
		Timer( Game::Text* text) : text_ ( text ) {}
		~Timer() {}

		void start();
		void update(const Game::UpdateContext* uctx);
		void stop();

	};

}
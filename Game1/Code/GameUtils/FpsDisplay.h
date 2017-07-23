#pragma once
#include <Game/Updater.h>
#include <Game/Text.h>

namespace GameUtils
{
	class FpsDisplay :
		public Game::Updater
	{
		Game::Text* text_ = nullptr;
		float timeVals_[ 100 ];
		unsigned int valsCounter_ = 0;
	public:
		FpsDisplay(Game::Text* text) : text_(text) {}
		~FpsDisplay() {}

		void start() override;
		void update(const Game::UpdateContext* uctx) override;
		void stop() override;

	};

}
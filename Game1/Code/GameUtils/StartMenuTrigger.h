#pragma once

#include <Game/Updater.h>
#include <Input/MenuInput.h>

namespace GameUtils
{
	class StartMenuTrigger
		: public Game::Updater
	{
		Input::MenuInput input_;
	public:
		StartMenuTrigger();
		~StartMenuTrigger();
		void start() override;
		void update(const Game::UpdateContext* uctx) override;
		void stop() override;
	};

}
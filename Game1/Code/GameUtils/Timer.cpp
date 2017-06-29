#include "Timer.h"
#include <cstdio>


void GameUtils::Timer::start()
{
	time_ = .0f;
	text_->setText("0 s");
	text_->setMaxCharNumber(50);
}

void GameUtils::Timer::update(const Game::UpdateContext* uctx)
{
	char timerStr[50];
	sprintf_s(timerStr, "%d s", (unsigned int)time_);
	text_->setText(timerStr);
	time_ += uctx->deltaTime;
}

void GameUtils::Timer::stop()
{
	time_ = .0f;
}
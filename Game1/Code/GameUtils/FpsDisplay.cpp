#include "FpsDisplay.h"
#include <cstdio>


void GameUtils::FpsDisplay::start()
{
	text_->setText("0 fps");
	text_->setMaxCharNumber(50);
}

void GameUtils::FpsDisplay::update(const Game::UpdateContext* uctx)
{
	char timerStr[50];
	timeVals_[valsCounter_] = uctx->deltaTime;
	++valsCounter_;
	if (valsCounter_ == 100)
	{
		valsCounter_ = 0;
	}
	
	float timeAvg = .0f;
	float maxTime = 0.0f;
	for (unsigned int i = 0; i < 100; ++i)
	{
		timeAvg += timeVals_[i];
		maxTime = max(maxTime, timeVals_[i]);
	}
	timeAvg *= .01f;
	
	
	sprintf_s(timerStr, "%3.1f fps avg     %3.1f fps min", 1.0f / timeAvg, 1.0f / maxTime);
	text_->setText(timerStr);
}

void GameUtils::FpsDisplay::stop()
{
}
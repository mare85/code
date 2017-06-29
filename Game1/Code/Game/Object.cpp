#include "Object.h"
#include <Game/ParamSet.h>

void Game::Object::allocParamSet()
{
	paramSet_ = new Game::ParamSet();
}

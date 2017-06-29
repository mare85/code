#include "StartMenuTrigger.h"
#include <Input/Listener.h>
#include <Game/App.h>
#include <Sound/System.h>
#include <Black/Editor/Editor.h>
GameUtils::StartMenuTrigger::StartMenuTrigger()
{
}


GameUtils::StartMenuTrigger::~StartMenuTrigger()
{
}

void GameUtils::StartMenuTrigger::start()
{
}

void GameUtils::StartMenuTrigger::update(const Game::UpdateContext * uctx)
{
	
	if (Game::App::EditorEnabled() && Black::Editor::Editor::IsInEditionMode())
		return;
	input_.feed(uctx->input, uctx->deltaTime);
	if (input_.start)
	{
		Game::App* app = Game::App::Instance();
		app->showMenu();
		app->setPaused(true);
		Sound::pauseAll();
	}
}

void GameUtils::StartMenuTrigger::stop()
{
}

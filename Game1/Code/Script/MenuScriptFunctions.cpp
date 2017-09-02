#include "MenuScriptFunctions.h"
#include <Script/MenuScript.h>
#include <Game/App.h>
#include <Sound/System.h>
#include <MusicRoom/Launcher.h>
#include <Black/Launcher.h>
#include <Neuron/Launcher.h>

static void __showMenu() { Game::App::Instance()->showMenu(); }
static void __hideMenu() { Game::App::Instance()->hideMenu(); }
static void __dropMenu() { Game::App::Instance()->dropMenu(); }
static void __dropScene() { Game::App::Instance()->dropScene(); }

static void __fadeIn() { Game::App::Instance()->fade( .0f, 0.7f ); }
static void __fadeOut() { Game::App::Instance()->fade(1.0f, 0.7f); }

static void __loadStartMenu()
{
	Game::App::Instance()->loadMenu( "pauseMenu" );
}

static void __loadMainMenu()
{
	Game::App::Instance()->loadMenu("mainMenu");
}

template <void (*PARAM)(void)>
void launch()
{
	Game::App::Instance()->hideMenu();
	Script::addTimedFunction(1.0f, &__fadeOut);
	Script::addTimedFunction(1.1f, &__dropMenu);
	Script::addTimedFunction(1.2f, &__loadStartMenu);
	Script::addTimedFunction(2.0f, PARAM);
	Script::addTimedFunction(2.0f, &__fadeIn);
}

static void __unpause()
{
	Game::App::Instance()->setPaused(false);
	Sound::unpauseAll();
}

static void __startMenu_Continue()
{
	Game::App::Instance()->hideMenu();
	Script::addTimedFunction(1.1f, &__unpause);
}

static void __startMenu_BackToMenu()
{
	Game::App::Instance()->hideMenu();
	Sound::stopAll();
	Script::addTimedFunction(0.6f, &__fadeOut);
	Script::addTimedFunction(1.1f, &__dropScene);
	Script::addTimedFunction(1.1f, &__dropMenu);
	Script::addTimedFunction(1.1f, &__fadeIn);
	Script::addTimedFunction(1.3f, &__unpause);
	Script::addTimedFunction(1.35f, &__loadMainMenu);
	
	Script::addTimedFunction(1.4f, &__showMenu);
}

static void __exitTimed()
{
		Game::App::Instance()->requestExit();
}

static void __exitGame()
{
	__fadeOut();
	Sound::stopAll();
	Script::addTimedFunction(1.0f, &__exitTimed);
}

void Script::registerMainMenuFunctions()
{
	registerMainMenuCall( "exit", &__exitGame);
	registerMainMenuCall( "start", &launch<&Black::run>);
	registerMainMenuCall( "startMusicRoom", &launch<&MusicRoom::launch>);
	registerMainMenuCall( "startNeuron", &launch<&Neuron::launch>);
	registerMainMenuCall( "editor", &launch<&Black::levelEditor>);
	registerMainMenuCall( "animEditor", &launch<&Black::animEditor>);
	registerMainMenuCall( "showMenu", &__showMenu);
	registerMainMenuCall( "hideMenu", &__hideMenu);
	registerMainMenuCall( "dropMenu", &__dropMenu);
	registerMainMenuCall( "dropScene", &__dropScene);
	registerMainMenuCall( "startMenu_continue", &__startMenu_Continue);
	registerMainMenuCall( "startMenu_backToMain", &__startMenu_BackToMenu);
}
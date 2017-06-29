#include "MenuScriptFunctions.h"
#include <Script/MenuScript.h>
#include <Game/App.h>
#include <Game/Transform.h>
#include <Game/Sprite.h>
#include <Game/Text.h>
#include <Game/Scene.h>
#include <Black/Character.h>
#include <Black/GroundMesh.h>
#include <Black/GroundManager.h>
#include <Black/EventSystem.h>
#include <Black/Ground.h>
#include <Black/TextSubsystem.h>
#include <Black/BirdsManager.h>

#include <MusicRoom/Character.h>
#include <MusicRoom/Walls.h>


#include <GameUtils/Timer.h>
#include <GameUtils/InputChecker.h>
#include <GameUtils/StartMenuTrigger.h>
#include <Black/Editor/Editor.h>
#include <Black/Editor/CharacterAnimEditor.h>
#include <Sound/System.h>

static bool __inEditor = false;

static void __showMenu() { Game::App::Instance()->showMenu(); }
static void __hideMenu() { Game::App::Instance()->hideMenu(); }
static void __dropMenu() { Game::App::Instance()->dropMenu(); }
static void __dropScene() { Game::App::Instance()->dropScene(); }

static void __fadeIn() { Game::App::Instance()->fade( .0f, 0.7f ); }
static void __fadeOut() { Game::App::Instance()->fade(1.0f, 0.7f); }

static void _createAndAddTestScene()
{
	Game::App::Instance()->setRenderFunction(Game::RenderFunction::kBlack);
	Game::App::Instance()->setEditorEnabled(__inEditor);
	Game::Transform* mainCam = new Game::Transform("mainCam");
	mainCam->setTranslation(vmath::Vector3(0.0f, 0.0f, .0f));
	
	Game::Scene* scene = new Game::Scene("scene");
	Black::EventSystem::StartUp(scene);
	if (Game::App::EditorEnabled())
		Black::Editor::Editor* editor = new Black::Editor::Editor(scene);

	Black::GroundManager::startUp(scene, 1);
	Black::GroundManager* groundManager = Black::GroundManager::getInstance();
	scene->addTransform(mainCam);
	scene->createOrtographicCamera(mainCam->getHashName(), 2.5f);
	GameUtils::StartMenuTrigger* startMenuTrigger = new GameUtils::StartMenuTrigger();
	scene->addUpdater(startMenuTrigger);
	Black::Character* character = new Black::Character(scene, "character");
	new Black::BirdsManager( 100, scene );
	scene->addUpdater(new Black::TextSubsystem(scene, 1, "pl","assets/fonts/myfont" ));
	scene->start();
	Game::App::Instance()->setScene(scene);
}

static void _createAndAddMusicRoomScene()
{
	Game::App::Instance()->setRenderFunction(Game::RenderFunction::kStandard);
	Game::App::Instance()->setEditorEnabled(false);
	Game::Transform* mainCam = new Game::Transform("mainCam");
	mainCam->setTranslation(vmath::Vector3(0.0f, 0.0f, .0f));


	Game::Scene* scene = new Game::Scene("scene");
	scene->addTransform(mainCam);
	scene->createPersp(mainCam->getHashName(), 2.5f);
	GameUtils::StartMenuTrigger* startMenuTrigger = new GameUtils::StartMenuTrigger();
	scene->addUpdater(startMenuTrigger);
	MusicRoom::Character* character = new MusicRoom::Character(scene, "character");
	MusicRoom::WallsUpdater* wallsUpdater = new MusicRoom::WallsUpdater(character);
	scene->addUpdater(wallsUpdater);
	MusicRoom::Walls* walls = new MusicRoom::Walls("walls");
	Game::Transform* wallsTransform = new Game::Transform("walls");
	wallsTransform->carryObject(walls);
	scene->addTransform(wallsTransform);
	scene->start();
	Game::App::Instance()->setScene(scene);
}

static void _createAndAddAnimEditorScene()
{
	Game::App::Instance()->setRenderFunction(Game::RenderFunction::kBlack);
	Game::App::Instance()->setEditorEnabled(__inEditor);
	Game::Transform* mainCam = new Game::Transform("mainCam");
	mainCam->setTranslation(vmath::Vector3(0.0f, 0.0f, .0f));
	
	Game::Scene* scene = new Game::Scene("animEditor");
	Black::EventSystem::StartUp(scene);
	scene->addTransform(mainCam);
	scene->createOrtographicCamera(mainCam->getHashName(), 2.5f);
	Game::App::Instance()->setPPColor0( vmath::Vector3(1.1f, .8f, .5f) );
	Game::App::Instance()->setPPColor1( vmath::Vector3(1.1f, .9f, .5f) );
	GameUtils::StartMenuTrigger* startMenuTrigger = new GameUtils::StartMenuTrigger();
	scene->addUpdater(startMenuTrigger);
	new Black::Editor::CharacterAnimEditor(scene);

	scene->start();
	Game::App::Instance()->setScene(scene);
}

static void __loadStartMenu()
{
	Game::App::Instance()->loadMenu( "pauseMenu" );
}

static void __loadMainMenu()
{
	Game::App::Instance()->loadMenu("mainMenu");
}


static void __newGameMusicRoom()
{
	__inEditor = false;
	Game::App::Instance()->hideMenu();
	Script::addTimedFunction(1.0f, &__fadeOut);
	Script::addTimedFunction(1.1f, &__dropMenu);
	Script::addTimedFunction(1.2f, &__loadStartMenu);
	Script::addTimedFunction(2.0f, &_createAndAddMusicRoomScene);
	Script::addTimedFunction(2.0f, &__fadeIn);

}


static void __newGame()
{
	__inEditor = false;
	Game::App::Instance()->hideMenu();
	Script::addTimedFunction(1.0f, &__fadeOut);
	Script::addTimedFunction(1.1f, &__dropMenu);
	Script::addTimedFunction(1.2f, &__loadStartMenu);
	Script::addTimedFunction(2.0f, &_createAndAddTestScene);
	Script::addTimedFunction(2.0f, &__fadeIn);

}


static void __editor()
{
	__inEditor = true;
	Game::App::Instance()->hideMenu();
	Script::addTimedFunction(1.0f, &__fadeOut);
	Script::addTimedFunction(1.1f, &__dropMenu);
	Script::addTimedFunction(1.2f, &__loadStartMenu);
	Script::addTimedFunction(2.0f, &_createAndAddTestScene);
	Script::addTimedFunction(2.0f, &__fadeIn);

}

static void __animEditor()
{
	Game::App::Instance()->hideMenu();
	Script::addTimedFunction(1.0f, &__fadeOut);
	Script::addTimedFunction(1.1f, &__dropMenu);
	Script::addTimedFunction(1.2f, &__loadStartMenu);
	Script::addTimedFunction(2.0f, &_createAndAddAnimEditorScene);
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

static void __quitToMain()
{
	Sound::stopAll();
	Script::addTimedFunction(1.0f, &__exitTimed);
}


void Script::registerMainMenuFunctions()
{
	registerMainMenuCall( "exit", &__exitGame);
	registerMainMenuCall( "start", &__newGame);
	registerMainMenuCall( "startMusicRoom", &__newGameMusicRoom);
	registerMainMenuCall( "editor", &__editor);
	registerMainMenuCall( "animEditor", &__animEditor);
	registerMainMenuCall( "showMenu", &__showMenu);
	registerMainMenuCall( "hideMenu", &__hideMenu);
	registerMainMenuCall( "dropMenu", &__dropMenu);
	registerMainMenuCall( "dropScene", &__dropScene);
	registerMainMenuCall( "startMenu_continue", &__startMenu_Continue);
	registerMainMenuCall("startMenu_backToMain", &__startMenu_BackToMenu);
}
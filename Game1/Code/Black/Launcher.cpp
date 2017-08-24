#include <Black/Launcher.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * Black::Launcher
 */
#include <Game/App.h>
#include <Game/Transform.h>
#include <Game/Scene.h>
#include <GameUtils/StartMenuTrigger.h>
#include <Black/EventSystem.h>
#include <Black/Editor/Editor.h>
#include <Black/Character.h>
#include <Black/BirdsManager.h>
#include <Black/TextSubsystem.h>
#include <Black/Editor/CharacterAnimEditor.h>
void Black::run()
{
	Game::App::Instance()->setRenderFunction(Game::RenderFunction::kBlack);
	Game::App::Instance()->setEditorEnabled(false);
	Game::Transform* mainCam = new Game::Transform("mainCam");
	mainCam->setTranslation(vmath::Vector3(0.0f, 0.0f, .0f));
	
	Game::Scene* scene = new Game::Scene("scene");
	Black::EventSystem::StartUp(scene);

	Black::GroundManager::startUp(scene, 1);
	scene->addTransform(mainCam);
	scene->createOrtographicCamera(mainCam->getHashName(), 2.5f);
	GameUtils::StartMenuTrigger* startMenuTrigger = new GameUtils::StartMenuTrigger();
	scene->addUpdater(startMenuTrigger);
	new Black::Character(scene, "character");
	new Black::BirdsManager( 100, scene );
	scene->addUpdater(new Black::TextSubsystem(scene, 1, "pl","assets/fonts/myfont" ));
	scene->start();
	Game::App::Instance()->setScene(scene);
}
void Black::levelEditor()
{
	Game::App::Instance()->setRenderFunction(Game::RenderFunction::kBlack);
	Game::App::Instance()->setEditorEnabled(true);
	Game::Transform* mainCam = new Game::Transform("mainCam");
	mainCam->setTranslation(vmath::Vector3(0.0f, 0.0f, .0f));
	
	Game::Scene* scene = new Game::Scene("scene");
	Black::EventSystem::StartUp(scene);
	new Black::Editor::Editor(scene);

	Black::GroundManager::startUp(scene, 1);
	scene->addTransform(mainCam);
	scene->createOrtographicCamera(mainCam->getHashName(), 2.5f);
	GameUtils::StartMenuTrigger* startMenuTrigger = new GameUtils::StartMenuTrigger();
	scene->addUpdater(startMenuTrigger);
	new Black::Character(scene, "character");
	new Black::BirdsManager( 100, scene );
	scene->addUpdater(new Black::TextSubsystem(scene, 1, "pl","assets/fonts/myfont" ));
	scene->start();
	Game::App::Instance()->setScene(scene);
}
void Black::animEditor()
{
	Game::App::Instance()->setRenderFunction(Game::RenderFunction::kBlack);
	Game::App::Instance()->setEditorEnabled(false);
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


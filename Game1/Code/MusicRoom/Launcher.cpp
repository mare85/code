#include <MusicRoom/Launcher.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * MusicRoom::Launcher
 */
#include <Game/App.h>
#include <Game/Transform.h>
#include <Game/Scene.h>
#include <GameUtils/StartMenuTrigger.h>
#include <MusicRoom/Character.h>
#include <MusicRoom/Walls.h>
void MusicRoom::launch()
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

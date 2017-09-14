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
#include <GameUtils/SpectrumVisualisation.h>
void MusicRoom::launch()
{
	Game::App::Instance()->setRenderFunction(Game::RenderFunction::kStandard);
	Game::App::Instance()->setDepthEnabled(true);
	Game::Transform* mainCam = new Game::Transform("mainCam");
	mainCam->setTranslation(vmath::Vector3(0.0f, 0.0f, .0f));

	Game::Scene* scene = new Game::Scene("scene");
	scene->addTransform(mainCam);
	scene->createPersp(mainCam->getHashName(), 2.5f);
	GameUtils::StartMenuTrigger* startMenuTrigger = new GameUtils::StartMenuTrigger();
	scene->addUpdater(startMenuTrigger);

	GameUtils::SpectrumVisualisation* spectrumVis = 
		new GameUtils::SpectrumVisualisation( "spectrum", 4.0f, 128.0f );
	Game::Transform* spectrumTransf = new Game::Transform( "spectrumTransform");
	spectrumTransf->carryObject( spectrumVis );
	spectrumTransf->setRotation( vmath::Quat::rotationY( 1.570796f ) * vmath::Quat::rotationX(1.570796f));
	spectrumTransf->setTranslation( vmath::Vector3( .0f, 3.0f, .0f ) );
	scene->addTransform( spectrumTransf );
	
	MusicRoom::Character* character = new MusicRoom::Character(scene, "character");
	character->setExtraTransform( spectrumTransf );
	MusicRoom::WallsUpdater* wallsUpdater = new MusicRoom::WallsUpdater(character);
	scene->addUpdater(wallsUpdater);
	MusicRoom::Walls* walls = new MusicRoom::Walls("walls");
	Game::Transform* wallsTransform = new Game::Transform("walls");
	wallsTransform->carryObject(walls);
	scene->addTransform(wallsTransform);
	scene->start();
	Game::App::Instance()->setScene(scene);
}

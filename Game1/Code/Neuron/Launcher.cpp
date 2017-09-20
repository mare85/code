#include <Neuron/Launcher.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * Neuron::Launcher
 */
#include <Game/App.h>
#include <Game/Transform.h>
#include <Game/Scene.h>
#include <GameUtils/StartMenuTrigger.h>
#include <Neuron/Network.h>
#include <Neuron/TravelCamera.h>
#include <GameUtils/FreeRoamCamera.h>
void Neuron::launch()
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
	Neuron::Network* network = new Neuron::Network();
	scene->addUpdater(network);

	GameUtils::FreeRoamCamera* cam = new GameUtils::FreeRoamCamera( scene, "camera" );
	//Neuron::TravelCamera* cam = new Neuron::TravelCamera( scene, network->getNodes(), "travelCam" );
	(void)cam;

	Game::Transform* networkTransform = new Game::Transform("walls");
	networkTransform->carryObject(network);
	scene->addTransform(networkTransform);
	scene->start();
	Game::App::Instance()->setScene(scene);
}
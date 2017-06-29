#include "LevelFunctions.h"
#include <Black/Ground.h>
#include <Black/TreeMesh.h>
#include <Black/GrassMesh.h>
#include <Black/BlackSprite.h>
#include <Black/Event.h>
#include <Black/GroundManager.h>
#include <Game/Transform.h>
#include <Game/App.h>
#include <Game/Scene.h>
#include <Script/ScriptFileLoader.h>

#include <Black/EventSystem.h>
#include <Black/Editor/Editor.h>

static void __UnserialiseTree(const Script::Params& p, Game::Scene* scene)
{

	unsigned int layerId = p.intParam[0];
	Black::Ground* ground = Black::GroundManager::getInstance()->getGround(layerId);
	Black::TreeMesh* treeMesh = new Black::TreeMesh(p.stringParams[0]);

	Game::Transform* t = new Game::Transform(p.stringParams[0]);
	float x = p.floatParam[0];
	float y = ground->getTopHeight(x - .3f) + treeMesh->elevation;
	treeMesh->setLayer(layerId);

	t->setTranslation(vmath::Vector3(x, y, .0f));
	t->carryObject(treeMesh);
	scene->addTransform(t);

	if (Game::App::EditorEnabled())
		Black::Editor::Editor::Add(treeMesh);
}

static void __UnserialiseSprite(const Script::Params& p, Game::Scene* scene)
{
	unsigned int layerId = p.intParam[0];
	Black::Ground* ground = Black::GroundManager::getInstance()->getGround(layerId);
	float x = p.floatParam[0];
	float y = ground->getTopHeight(x - .3f);
	Game::Transform* t = new Game::Transform(p.stringParams[0]);
	Black::BlackSprite* sprite = new Black::BlackSprite(p.stringParams[0], p.stringParams[1], layerId, .0f);

	t->setTranslation(vmath::Vector3(x, y, .0f));
	t->carryObject(sprite);
	scene->addTransform(t);
	if (Game::App::EditorEnabled())
		Black::Editor::Editor::Add(sprite);
}
static void __SetFloatParam(const Script::Params& p, Game::Scene* scene)
{
	Game::Object* obj = scene->findObject(p.intParam[0]);
	Game::ParamSet* ps = obj->getPaarmSet();
	assert(ps);
	unsigned int id = ps->getIdByname(p.stringParams[0]);
	ps->setFloat(id, p.floatParam[0], obj);
}
static void __SetStringParam(const Script::Params& p, Game::Scene* scene)
{
	Game::Object* obj = scene->findObject(p.intParam[0]);
	Game::ParamSet* ps = obj->getPaarmSet();
	assert(ps);
	unsigned int id = ps->getIdByname(p.stringParams[0]);
	ps->setString(id, p.stringParams[1], obj);
}
static void __SetIntParam(const Script::Params& p, Game::Scene* scene)
{
	Game::Object* obj = scene->findObject(p.intParam[0]);
	Game::ParamSet* ps = obj->getPaarmSet();
	assert(ps);
	unsigned int id = ps->getIdByname(p.stringParams[0]);
	ps->setInt(id, p.intParam[1], obj);
}

static void __UnserialiseGrass(const Script::Params& p, Game::Scene* scene)
{
	Black::GrassMesh* mesh = new Black::GrassMesh(p.intParam[0], p.intParam[1]);
	mesh->createAndAddTransform(scene);
	if (Game::App::EditorEnabled())
		Black::Editor::Editor::Add(mesh);
}

static void __UnserialiseEvent(const Script::Params& p, Game::Scene* scene)
{

	unsigned int layerId = p.intParam[0];
	Black::Ground* ground = Black::GroundManager::getInstance()->getGround(layerId);
	Black::Event* evt = new Black::Event(p.stringParams[0]);

	Game::Transform* t = new Game::Transform(p.stringParams[0]);
	float x = p.floatParam[0];
	float y = ground->getTopHeight(x);
	evt = new Black::Event(p.stringParams[0]);
	evt->setLayer(layerId);


	t->setTranslation(vmath::Vector3(x, y, .0f));
	t->carryObject(evt);
	scene->addTransform(t);
	Black::EventSystem::GetInstance()->addEvent(evt);
	if (Game::App::EditorEnabled())
		Black::Editor::Editor::Add(evt);
}


void Script::registerLevelFunctions()
{

	registerCommand("level", "tree", "sfd", &__UnserialiseTree);
	registerCommand("level", "sprite", "sdfs", &__UnserialiseSprite);
	registerCommand("level", "grass", "dd", &__UnserialiseGrass);
	registerCommand("level", "setfloat", "dsf", &__SetFloatParam);
	registerCommand("level", "setstring", "dss", &__SetStringParam);
	registerCommand("level", "setint", "dsd", &__SetIntParam);
	registerCommand("level", "event", "sfd", &__UnserialiseEvent);

}

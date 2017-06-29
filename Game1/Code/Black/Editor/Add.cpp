#include "Add.h"
#include <Black/Editor/Editor.h>
#include <Black/BlackSprite.h>
#include <Black/GrassMesh.h>
#include <Black/TreeMesh.h>
#include <Black/Event.h>
#include <Black/Ground.h>
#include <Game/App.h>

Black::TreeMesh* Black::Editor::add::tree()
{
	Editor* ed = Editor::GetInstance();
	static unsigned int treeAddId = 1;
	unsigned int layerId = GroundManager::getInstance()->getLayerIndex();
	Ground* ground = Black::GroundManager::getInstance()->getGround(layerId);
	float x = ed->position_.getX();
	float y = ground->getTopHeight(x - .3f) - .1f;
	char stringname[250];
	do
	{
		sprintf_s(stringname, "tree%03d", treeAddId);
		++treeAddId;
	} while (ed->scene_->findObject(Util::createHash(stringname)));

	Game::Transform* t = new Game::Transform(stringname);
	TreeMesh* treeMesh = new TreeMesh(stringname);
	treeMesh->elevation = -.3f;
	treeMesh->setLayer(layerId);

	t->setTranslation(vmath::Vector3(x, y, .0f));
	t->carryObject(treeMesh);
	ed->scene_->addTransform(t);
	Game::App::Instance()->requestReloadLayerGraphics(ed->layerId_);
	Editor::Add(treeMesh);
	return treeMesh;
}

Black::GrassMesh* Black::Editor::add::grass()
{
	Editor* ed = Editor::GetInstance();
	Editor::_Sector& sec = ed->sectors_[ed->currentSector_];
	bool foundGrass = false;
	for (unsigned int i = 0; i < sec.nHashes; ++i)
	{
		Game::Object *obj = ed->scene_->findObject(sec.hashes_[i]);
		if (dynamic_cast<Black::GrassMesh*>(obj))
		{
			foundGrass = true;
			return static_cast<Black::GrassMesh*>(obj);
		}
	}
	if (!foundGrass)
	{
		GrassMesh* mesh = new GrassMesh(ed->layerId_, ed->currentSector_ % GroundManager::eNSectors);
		mesh->createAndAddTransform(ed->scene_);
		Editor::Add(mesh);
		Game::App::Instance()->requestReloadLayerGraphics(ed->layerId_);
		return mesh;
	}
	return nullptr;
}

Black::BlackSprite* Black::Editor::add::sprite(const char * path)
{
	Editor* ed = Editor::GetInstance();
	static unsigned int spriteAddId = 1;
	unsigned int layerId = GroundManager::getInstance()->getLayerIndex();
	Ground* ground = Black::GroundManager::getInstance()->getGround(layerId);
	float x = ed->position_.getX();
	float y = ground->getTopHeight(x - .3f);
	char stringname[250];
	do
	{
		sprintf_s(stringname, "blackSprite%03d", spriteAddId);
		++spriteAddId;
	} while (ed->scene_->findObject(Util::createHash(stringname)));

	Game::Transform* t = new Game::Transform(stringname);
	BlackSprite* spriteMesh = new BlackSprite(stringname, path, layerId, .0f);
	t->setTranslation(vmath::Vector3(x, y, .0f));
	t->carryObject(spriteMesh);
	ed->scene_->addTransform(t);
	Editor::Add(spriteMesh);
	Game::App::Instance()->requestReloadLayerGraphics(ed->layerId_);
	return spriteMesh;
}

Black::Event* Black::Editor::add::event()
{
	Editor* ed = Editor::GetInstance();
	static unsigned int evtAddId = 1;
	unsigned int layerId = GroundManager::getInstance()->getLayerIndex();
	Ground* ground = Black::GroundManager::getInstance()->getGround(layerId);
	float x = ed->position_.getX();
	float y = ground->getTopHeight(x);
	char stringname[250];
	do
	{
		sprintf_s(stringname, "evt%04d", evtAddId);
		++evtAddId;
	} while (ed->scene_->findObject(Util::createHash(stringname)));

	Game::Transform* t = new Game::Transform(stringname);
	Event* evt = new Event(stringname);
	evt->setLayer(layerId);

	t->setTranslation(vmath::Vector3(x, y, .0f));
	t->carryObject(evt);
	ed->scene_->addTransform(t);
	Game::App::Instance()->requestReloadLayerGraphics(ed->layerId_);
	Editor::Add(evt);
	return evt;
}

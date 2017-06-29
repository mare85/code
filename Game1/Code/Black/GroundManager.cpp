#include "GroundManager.h"

#include <Game/Scene.h>
#include <Game/Transform.h>
#include <Game/StaticMesh.h>
#include <Graphics/VertexTypes.h>
#include <Black/Ground.h>
#include <Util/RandomGenerator.h>
#include <Black/TreeMesh.h>
#include <Black/BlackSprite.h>
#include <Black/GroundMesh.h>
#include <Script/ScriptFileLoader.h>


Black::GroundManager* __inst = nullptr;

float Black::GroundManager::SegmentWidth = 2.0f;
float Black::GroundManager::HalfSegmentWidth = Black::GroundManager::SegmentWidth * .5f;
float Black::GroundManager::WorldWrap = Black::GroundManager::eNSectors * Black::GroundManager::HalfSegmentWidth;
float Black::GroundManager::WorldWidth = Black::GroundManager::eNSectors * Black::GroundManager::SegmentWidth;

void Black::GroundManager::startUp(Game::Scene * scene, unsigned int levelId )
{
	__inst = new GroundManager();
	Util::RandomGenerator gen( 0x78123687);
	FILE* f = nullptr;

	__inst->levelId_ = levelId;
	float * heights = __inst->heights_;

	char filePath[250];
	sprintf_s(filePath, "assets/levels/level%d/ground.bin", levelId );


	fopen_s(&f, filePath, "rb");
	fread(heights, sizeof(float), eNLayers * eNSectors, f);
	fclose(f);


	for (int i = 0; i < eNLayers; ++i)
	{
		Black::Ground* ground = new Black::Ground(); // for clarity
		__inst->grounds_[ i ] = ground;
		ground->generate(heights + i * Black::Ground::eNColliders);

		char groundname[20];
		sprintf_s(groundname, "ground%d", i + 1);
		__inst->meshes_[ i ] = new Black::GroundMesh(groundname, ground);
		__inst->meshes_[ i ]->setLayer(i);

		Game::Transform* transform = new Game::Transform(groundname);
		transform->carryObject(__inst->meshes_[i]);
		scene->addTransform(transform);
	}
	{
		//char filePath[250];

		Script::runScript(scene, levelId, "level.txt" );

		//sprintf_s(filePath, "assets/levels/level%d/", levelId);
		//Script::loadLevel( scene, filePath );
	}

}

void Black::GroundManager::shutDown()
{
	delete __inst;
}

Black::GroundManager* Black::GroundManager::getInstance()
{
	return __inst;
}

bool Black::GroundManager::switchToNextLowerGround(float x, float y,float dir)
{
	if (dir > .5f)
	{
		int index1 = ( layerIndex_ + 1 + eNLayers) % eNLayers;
		float h1 = grounds_[index1]->getTopHeight(x);
		if (h1 < y - .5f)
		{
			inc();
			return true;
		}
	}
	else if(dir < -.5f)
	{
		int index2 = ( layerIndex_ + eNLayers - 1 ) % eNLayers;
		float h2 = grounds_[index2]->getTopHeight(x);
		if (h2 < y - .5f)
		{
			dec();
			return true;
		}
	}
	return false;
}

void Black::GroundManager::elevate(unsigned int index, float value)
{
	unsigned int layerId = index / eNSectors;
	heights_[index] += value;
	grounds_[layerId]->generate(heights_ + layerId * Black::Ground::eNColliders);
	meshes_[layerId]->requestUpdate();
}

void Black::GroundManager::saveLayer(unsigned int layerId)
{

	char filePath[250];
	sprintf_s(filePath, "assets/levels/level%d/ground%d.txt", levelId_, layerId);

	FILE* f = nullptr;;
	fopen_s(&f, filePath, "w");
	for (unsigned int j = 0; j < Black::Ground::eNColliders; ++j)
	{
		fprintf_s(f, "%3.2f ", heights_[layerId * Black::Ground::eNColliders + j]);
		if ((j % 10) == 1)
		{
			fprintf_s(f, "\n");
		}
	}
	fclose(f);
}

void Black::GroundManager::save()
{
	char filePath[250];
	sprintf_s(filePath, "assets/levels/level%d/ground.bin", levelId_);

	FILE* f = nullptr;;
	fopen_s(&f, filePath, "wb");
	fwrite(heights_, sizeof(float), Black::GroundManager::eNLayers * Black::GroundManager::eNSectors, f);
	fclose(f);
}

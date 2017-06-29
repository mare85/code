#include "GrassMesh.h"
#include <Game/Object.h>
#include <Game/Scene.h>
#include <Game/App.h>
#include <Game/ParamSet.h>
#include <Game/Transform.h>
#include <Util/Utils.h>
#include <Util/RandomGenerator.h>
#include <Black/GroundManager.h>
#include <Black/Ground.h>

#include <cstdio>

void Black::GrassMesh::_generateGrass(vmath::Vector4 * outData )
{
	Util::RandomGenerator gen(hashName_);
	float x0 = sectorId_ * GroundManager::SegmentWidth - GroundManager::WorldWrap;
	float halfWidth = GroundManager::HalfSegmentWidth;
	xCenter_ = x0 + halfWidth;
	Ground* ground = GroundManager::getInstance()->getGround(layerId_);

	for (unsigned int i2 = 0; i2 < grassCount_; ++i2)
	{
		float x = gen.getFloat(x0, x0 + GroundManager::SegmentWidth);
		vmath::Vector3 startPos(x - xCenter_, ground->getTopHeight(x) - .05f, .0f);
		vmath::Vector3 middlePos = startPos + height_ * vmath::Vector3(gen.getFloat(-.6667f, .66667f), gen.getFloat(.66667f, 1.33333f), .0f);
		vmath::Vector3 endPos = middlePos + height_ * vmath::Vector3(gen.getFloat(.66667f, 3.33333f), gen.getFloat(.6667f, 1.3333f), .0f);
		Util::fillHair(outData, grassSteps_, startPos, middlePos, endPos, .1f, gen.getFloat(1.0f));

		outData += 2 * grassSteps_;
	}
}

void Black::GrassMesh::__OnParamChange(Game::Object * obj)
{
	GrassMesh* gm = reinterpret_cast<GrassMesh*>(obj);
	Graphics::VertexDesc grassDesc = Graphics::VertexDesc::get(Graphics::POS4);
	vmath::Vector4* grassData = reinterpret_cast<vmath::Vector4*>(gm->allocateData(2 * gm->grassSteps_ * gm->grassCount_, &grassDesc));
	gm->_generateGrass(grassData);
	Game::App::Instance()->requestReloadLayerGraphics(gm->layerId_);
}

Black::GrassMesh::GrassMesh(unsigned int layerId, unsigned int sectorId)
	: StaticMesh("grass_0_0")
	
	, layerId_ (layerId )
	, sectorId_( sectorId )
{

	allocParamSet();
	Game::ParamSet* ps = getPaarmSet();
	ps->clear();
	//ps->registerParam(Game::eStringParam, "comment", comment, nullptr);
	ps->registerFloat( "height", &height_, __OnParamChange);
	setLayer( layerId_ );
	sprintf_s( name_, "grass_%d_%d", layerId, sectorId );
	hashName_ = Util::createHash(name_);
	grassSteps_ = 8;
	grassCount_ = 250;
	
	setShader("assets/Shaders/BlackWave.fx", "BlackWave");

	Graphics::VertexDesc grassDesc = Graphics::VertexDesc::get(Graphics::POS4);
	vmath::Vector4* grassData = reinterpret_cast<vmath::Vector4*>(allocateData(2 * grassSteps_ * grassCount_, &grassDesc));
	_generateGrass(grassData);
	float halfWidth = GroundManager::HalfSegmentWidth;
	setBounds(-halfWidth, halfWidth);
}

Black::GrassMesh::~GrassMesh()
{
}

void Black::GrassMesh::serialize(unsigned char* buffer, unsigned int* outBytes, unsigned int maxBytes)
{
	char data[220];
	sprintf_s(data, "level.grass %d %d\n", layerId_, sectorId_);
	if (hasPaarmSet())
	{

		getPaarmSet()->serialize(data, 220, this);
	}

	*outBytes = min((unsigned int)strlen(data) + 1, maxBytes);

	memcpy(buffer, data, *outBytes);
}

Game::Transform* Black::GrassMesh::createAndAddTransform(Game::Scene* scene) const
{
	Game::Transform* transformGrass = new Game::Transform(getName());
	transformGrass->setTranslation(vmath::Vector3(xCenter_, .0f, .0f));
	transformGrass->carryObject((Game::Object*)this);
	scene->addTransform(transformGrass);
	return transformGrass;
}

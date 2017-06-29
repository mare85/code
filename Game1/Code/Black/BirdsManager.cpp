#include "BirdsManager.h"
#include <stdio.h>
#include <Game/Scene.h>
#include <Game/Transform.h>
#include <Black/Character.h>
#include <Black/Ground.h>


static Black::BirdsManager* __instance = nullptr;

void Black::BirdsManager::_UpdateLayer(const Game::UpdateContext * uctx, unsigned int layerIndex)
{
	UpdatealbeMesh* mesh = mesh_[layerIndex];
	vmath::Vector4* meshBuffer = mesh->getMappedData();
	vmath::Vector4* meshPtr_ = meshBuffer;
	float dt = uctx->deltaTime;
	unsigned int startIndex = layerIndex * maxBoidPerLayer_;
	float* phases = phases_ + startIndex;
	float* phasePtr = phases;
	float* ttls = ttls_ + startIndex;
	float* ttlPtr = ttls;
	vmath::Vector3* positions = positions_ + startIndex;
	vmath::Vector3* posPtr = positions;
	vmath::Vector3* velocities = velocities_ + startIndex;
	vmath::Vector3* velPtr = velocities;
	float* sizes = sizes_ + startIndex;
	
	for( unsigned int i = 0; i < numBoids_[layerIndex]; ++i )
	{
		*phasePtr += dt * 1.0f;
		if( *phasePtr > 6.283f) *phasePtr -= 6.283f;
		*posPtr += *velPtr * dt;
		*velPtr += vmath::Vector3( sin( *phasePtr ), cos( 3.0f * *phasePtr ), .0f ) * dt;
		*ttlPtr -= dt;
		
		++phasePtr;
		++ttlPtr;
		++posPtr;
		++velPtr;
	}

	//releasing birds
	
	unsigned int numBoids = numBoids_[layerIndex];
	for( unsigned int i = numBoids - 1; i < numBoids ; --i )
	{
		if( ttls[i] < .0f )
		{
			--numBoids;
			ttls[i] = ttls[numBoids];
			sizes[i] = sizes[numBoids];
			phases[i] = phases[numBoids];
			positions[i] = positions[numBoids];
			velocities[i] = velocities[numBoids];
		}
	}
	numBoids_[layerIndex] = numBoids;
	//drawing
	if( meshPtr_ )
	{
		for( unsigned int i = 0; i < numBoids_[layerIndex]; ++i ) {
			_DrawBird( layerIndex, i, meshPtr_ );
			meshPtr_ += nVertsPerBoid;
		}
		if( maxBoidPerLayer_ - numBoids_[layerIndex] )
		{
			memset( meshPtr_, 0, sizeof(vmath::Vector4) * nVertsPerBoid * (maxBoidPerLayer_ - numBoids_[layerIndex]) );
		}
	}

	//adding
	if( layerIndex == GroundManager::getInstance()->getLayerIndex() )
	{
		float baseX = Character::GetInstance()->cameraPosition().getX();
		toEmit_ += dt * rate_;
		while( toEmit_ > 1.0f )
		{
			if( numBoids_[layerIndex] == maxBoidPerLayer_)
			{
				toEmit_  = .0f;
			}
			else
			{
				float x = baseX + gen.getFloat(-3.0f, 3.0f);
				float y = GroundManager::getInstance()->getGround( layerIndex )->getTopHeight(x);
				unsigned int index = numBoids_[layerIndex];
				++numBoids_[layerIndex];
				positions[ index ] = vmath::Vector3( x,y,.0f);
				velocities[ index ] = vmath::Vector3( 1.0f + gen.getFloat(-.3f, .3f),2.0f + gen.getFloat(-1.0f, 1.0f),.0f);
				ttls[ index ] = gen.getFloat( 5.0f, 10.0f);
				phases[ index ] = gen.getFloat( 6.283f);
				sizes[ index ] = gen.getFloat( .25f, .5f );
				toEmit_  -= 1.0f;
			}
			
		}
	}
}



void Black::BirdsManager::_DrawBird(unsigned int layerIndex, unsigned int inLayerIndex, vmath::Vector4 * vertBuffer)
{
	unsigned int index = layerIndex * maxBoidPerLayer_ + inLayerIndex;
	vmath::Vector4 * vert = vertBuffer;
	vmath::Vector4 basePosition = vmath::Vector4( positions_[ index ], .0f );
	float phase = phases_[ index ];
	float wingflap = sin( phase * 14.0f ) * .25f + .2f;
	float scale = min( ttls_[index] * 2.0f, 1.0f ) * 1.0f * sizes_[ index ];
	float baseThickness = 0.02f;
	*vert = basePosition + vmath::Vector4( -1.0f, wingflap, .0f, .0f ) * scale;
	*(vert+ 1 ) = *vert; vert += 2;
	for( unsigned int i = 1; i < 11; ++i )
	{
		float arch = i * ( 10 - i ) * .005f;
		float wingDist = (10 - i) * .1f;
		float thickness = baseThickness * i;
		vmath::Vector4 offset = vmath::Vector4( -wingDist, wingflap * wingDist + arch, .0f, .0f ); 
		*vert = basePosition +  offset * scale;
		*(vert+ 1 ) = *vert- vmath::Vector4( .0f, -thickness, .0f, .0f ) * scale; vert += 2;
	}
	for( unsigned int i = 9; i > 0; --i )
	{
		float arch = i * ( 10 - i ) * .005f;
		float wingDist = (10 - i) * .1f;
		float thickness = baseThickness * i;
		vmath::Vector4 offset = vmath::Vector4( wingDist, wingflap * wingDist + arch, .0f, .0f ); 
		*vert = basePosition + offset * scale;
		*(vert+ 1 ) = *vert- vmath::Vector4( .0f, -thickness, .0f, .0f ) * scale; vert += 2;
	}
	*vert = basePosition + vmath::Vector4( 1.0f, wingflap * .3f, .0f, .0f ) * scale;
	*(vert + 1 ) = *vert; vert += 2;
}

Black::BirdsManager::BirdsManager(unsigned int maxBoidCount, Game::Scene* scene)
{
	assert( __instance == nullptr );
	__instance  = this;
	maxBoidPerLayer_ = maxBoidCount;
	scene_ = scene;
	positions_ = new vmath::Vector3[ maxBoidCount * GroundManager::eNLayers * 2];
	velocities_ = positions_ + maxBoidCount * GroundManager::eNLayers;
	sizes_ = new float[ maxBoidCount * GroundManager::eNLayers * 3];
	phases_ = sizes_ + maxBoidCount * GroundManager::eNLayers;
	ttls_ = phases_ + maxBoidCount * GroundManager::eNLayers;

	for( unsigned int i = 0; i < GroundManager::eNLayers; ++i )
	{
		char nameBuffer[20];
		char nameBufferT[20];
		sprintf_s( nameBuffer, "birdsMesh%d", i );
		sprintf_s( nameBufferT, "birdsMeshT%d", i );
		mesh_[ i ] = new UpdatealbeMesh( nameBuffer, maxBoidCount * nVertsPerBoid );
		mesh_[ i ]->setLayer( i );
		numBoids_[ i ] = 0;
		Game::Transform* t = new Game::Transform(nameBufferT);
		t->carryObject( mesh_[i] );
		scene->addTransform( t );
	}
	scene->addUpdater( this );
}

Black::BirdsManager::~BirdsManager()
{
	assert( __instance == this );
	__instance = nullptr;
	delete [] positions_;
	delete [] sizes_;
}

void Black::BirdsManager::start()
{
}

void Black::BirdsManager::update(const Game::UpdateContext * uctx)
{
	if( burstTime_ > .0f)
	{
		burstProgress_ += uctx->deltaTime / burstTime_;
		rate_ = burstMax_ * ( 1.0f - burstProgress_ );
		if( burstProgress_ >= 1.0f )
		{
			burstProgress_ = 0.0f;
			burstTime_  = .0f;
			rate_ = .0f;
		}
	}
	for( unsigned int li = 0; li < GroundManager::eNLayers; ++li )
		_UpdateLayer( uctx, li );
}

void Black::BirdsManager::stop()
{
}

void Black::BirdsManager::burst(float maxRate, float time)
{
	burstProgress_ = .0f;
	burstMax_ = maxRate;
	burstTime_ = time;
}



Black::BirdsManager * Black::BirdsManager::Instance()
{
	return __instance;
}

void Black::BirdsManager::Release()
{
	if( __instance )
		delete __instance;
}

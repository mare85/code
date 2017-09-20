#include <Neuron/TravelCamera.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * Neuron::TravelCamera
 */
#include <Game/Scene.h>
#include <Game/Transform.h>
#include <Game/Camera.h>
#include <Neuron/Nodes.h>
namespace Neuron {

const float TravelCamera::Speed = 1.5f;
const float TravelCamera::ChangeDist = 4.0f;
const float TravelCamera::ChangeFactor = 1.0f;

TravelCamera::TravelCamera( Game::Scene* scene, Nodes* nodes, const char* name )
: nodes_( nodes )
, scene_( scene )
, pos_( .0f )
{
	rootTransform_ = new Game::Transform(name);
	scene_->addTransform(rootTransform_);

	cam_ = scene_->createPersp(rootTransform_->getHashName(), .75f);
	cam_->setNearFar( .1f, 100.0f );
	scene_->setActiveCamera(rootTransform_->getHashName());

	scene_->addUpdater(this);
}

TravelCamera::~TravelCamera()
{
}

void TravelCamera::start() 
{
	pos_ = vmath::Vector3( .0f );
	front_ = vmath::Vector3( .0f, .0f, 1.0f );
	side_ = vmath::Vector3( 1.0f, .0f, .0f );
	targetIndex_ = InvalidTarget;
}

void TravelCamera::update(const Game::UpdateContext* uctx) 
{
	changeTimer_ += uctx->deltaTime;
	// pick nearest target
	if( targetIndex_ == InvalidTarget )
	{
		vmath::Vector3 targetTestPoint = pos_ + front_;
		float minDistSq = 1.0e20f;

		for( unsigned int i = 0; i < nodes_->nNodes; ++i )
		{
			Nodes::Node& n = nodes_->nodes_[ i ];
			float distSq = lengthSqr( n.pos_ - targetTestPoint );
			
			if( distSq < minDistSq )
			{
				minDistSq = distSq;
				targetIndex_ = i;
			}
		}
	}
	
	pos_ += front_ * uctx->deltaTime * Speed;
	vmath::Vector3& target = nodes_->nodes_[ targetIndex_ ].pos_;
	vmath::Vector3 targetDiff = target - pos_;
	float targetDist = length( targetDiff );

	if( targetDist < ChangeDist ) 
	{
		//picking next target
		Nodes::Node& n = nodes_->nodes_[ targetIndex_ ];
		vmath::Vector3 desiredDir = normalize( front_ + gen_.getVector3( .8f ) );
		float maxDot = -1.0f;
		unsigned int candidate = 0;
		
		for( unsigned int i = 0; i < n.nConnections_; ++i )
		{
			float d = dot( n.edgeDirs_[ i ], desiredDir );
			if( d > maxDot )
			{
				candidate = i;
				maxDot = d;
			}
		}

		Nodes::Edge& e = nodes_->edges_[ n.edgeIndices_[ candidate ] ];
		targetIndex_ = ( targetIndex_ == e.index0_ ) ? e.index1_ : e.index0_;
		changeTimer_ = .0f;
	}

	float changeFactor = fmin( 1.0f, changeTimer_ ) * ChangeFactor;

	//navigate slowly towards target
	vmath::Vector3 targetDir = targetDiff / targetDist;
	front_ = vmath::normalize( front_ + targetDir * uctx->deltaTime * changeFactor );
	vmath::Vector3 y = normalize( cross( -front_, side_ ) );
	side_ = cross( y, -front_ );
	rootTransform_->setRotation( vmath::Quat( vmath::Matrix3(side_, y, -front_ ) ) );
	rootTransform_->setTranslation( pos_ );
}

void TravelCamera::stop() 
{
}

}

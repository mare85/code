#include <Neuron/Nodes.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * Neuron::Nodes
 */
#include <Util/RandomGenerator.h>
#include <assert.h>
namespace Neuron {

const float Nodes::StartingAmp = 4.0f;
const float Nodes::AmpFadeOff = Nodes::StartingAmp * .05f;
const float Nodes::DefaultProgressSpeed = 5.0f;
const float Nodes::FadeOffTime = 2.0f;
const float Nodes::BoostFadeOffSpeed = 2.5f;
const float Nodes::BoostStrength = 20.0f;

Nodes::Nodes( float range )
{
	Util::RandomGenerator gen;
	// picking random node positions
	for( unsigned int ni = 0; ni < nNodes; ++ni )
	{
		nodes_[ ni ].pos_ = gen.getVector3( range );
		nodes_[ ni ].offsetPhase_ = gen.getVector3( .5f ) + vmath::Vector3( .5f );
		nodes_[ ni ].offsetSpeed_ = gen.getVector3( .1f ) + vmath::Vector3( .1f );
		nodes_[ ni ].offsetBoost_ = .0f;
		nodes_[ ni ].fadeOffTimer_ = .0f;
	}
	unsigned int curNEdges = 0;
	unsigned int i = 0;
	float sizeSqr = 16.0f * range * range;	

	while( curNEdges < nEdges)
	{
		float nearestDistSqr = sizeSqr;
		unsigned int nearestI = 0xffffffff;
		if( isFull(i))
		{
			i = ( (i+1)%nNodes );
			continue;
		}
		//finding nearest node to current
		for( unsigned int j = 0; j < nNodes; ++j )
		{
			if( i != j && !isFull(j) && !isConnected(i, j) )
			{
				float distSqr = lengthSqr( nodes_[i].pos_ - nodes_[j].pos_);
				if(distSqr < nearestDistSqr)
				{
					nearestI = j;
					nearestDistSqr = distSqr;
				}
			}
		}
		if( nearestI != 0xffffffff)
			addConnection( i, nearestI, curNEdges );
		i = ( (i+1)%nNodes );
	}
	// deternining edge directions for all the nodes
	for( unsigned int i = 0; i < nNodes; ++i)
	{
		Node& n = nodes_[i];
		vmath::Vector3 massCenter( .0f);
		for( unsigned int ei = 0; ei < n.nConnections_; ++ei)
		{
			Edge& e = edges_[ n.edgeIndices_[ei] ];
			unsigned int otherI = ( e.index0_ != i ) ? e.index0_ : e.index1_;  
			Node& n2 = nodes_[otherI];
			vmath::Vector3 dir = normalize( n2.pos_- n.pos_ ); 
			n.edgeDirs_ [ei] = dir; 
			massCenter += dir;
		}
		vmath::Quat randomRotation = vmath::Quat::rotation( 1.5f,
			normalize( gen.getVector3(1.0f ) )
		);
		if( n.nConnections_ > 1)
		{
			massCenter *= 1.0f / n.nConnections_;
			for( unsigned int ei = 0; ei < n.nConnections_; ++ei)
			{
				vmath::Vector3 dir = n.edgeDirs_[ ei ] - massCenter;
				if( lengthSqr( dir ) > .001f)
					n.edgeDirs_[ ei ] = normalize( n.edgeDirs_[ ei ] - massCenter );
				n.edgeDirs_[ ei ] = vmath::rotate( randomRotation, n.edgeDirs_[ ei ] );
			}
		}
	}
	assert( curNEdges == nEdges );
}


bool Nodes::isConnected( unsigned int index0, unsigned int index1) const
{
	const Node& n = nodes_[index0];
	for( unsigned int i = 0; i < n.nConnections_; ++i)
	{
		const Edge& e = edges_[ n.edgeIndices_[i] ];
		if ( e.index0_ == index1 || e.index1_ == index1 ) return true;
	}
	return false;
}


void Nodes::addConnection( unsigned int index0, unsigned int index1, unsigned int & nCurEdges)
{
	edges_[nCurEdges].setup(index0,index1);
	Node& n0 = nodes_[index0];
	Node& n1 = nodes_[index1];
	assert(n0.nConnections_< nEdgesPerNode );
	assert(n1.nConnections_< nEdgesPerNode );
	n0.edgeIndices_[ n0.nConnections_ ] = nCurEdges;
	++n0.nConnections_;
	n1.edgeIndices_[ n1.nConnections_ ] = nCurEdges;
	++n1.nConnections_;
	++nCurEdges;
}

void Nodes::update( float deltaTime )
{
	for( unsigned int i = 0; i < nEdges; ++i )
	{
		Edge& e = edges_[ i ];
		e.progress_ += e.progressSpeed_ * deltaTime;

		if( e.progressSpeed_ < .0f && e.progress_ < .0f )
		{
			e.progressSpeed_ = .0f;
			float newAmp = e.amp_ - AmpFadeOff;
			if( newAmp > .0f ) burstSignal( e.index0_, newAmp );
			e.amp_ = .0f;
		}	
		else if( e.progressSpeed_ > .0f && e.progress_ > 1.0f ) 
		{
			e.progressSpeed_ = .0f;
			float newAmp = e.amp_ - AmpFadeOff;
			if( newAmp > .0f ) burstSignal( e.index1_, newAmp );
			e.amp_ = .0f;
		}
	}

	const vmath::Vector3 HalfVec( .5f );
	const vmath::Vector3 OneVec( 1.0f );

	for( unsigned int i = 0; i < nNodes; ++i )
	{
		Node& n = nodes_[ i ];
		n.fadeOffTimer_ = fmaxf( n.fadeOffTimer_ - deltaTime, .0f ); 
		n.offsetBoost_ = fmaxf( n.offsetBoost_ - deltaTime * BoostFadeOffSpeed * n.boostAmp_, .0f );
		float boost = 1.0f - n.offsetBoost_;
		boost = n.offsetBoost_ * boost * boost * 7.0f; // 7 reaches value of about 1.0 at maximum t*(1.0 - t)^2
		vmath::Vector3 speed = n.offsetSpeed_ * (1.0f + BoostStrength * boost );
		n.offsetPhase_ += deltaTime * speed;
		n.offsetPhase_ -= HalfVec + // non iffy fractPerElem 
			vmath::copySignPerElem( HalfVec, n.offsetPhase_ - OneVec );
	}
}

void Nodes::burstSignal( unsigned int nodeIndex, float amp )
{
	Node& n = nodes_[ nodeIndex ];

	if( n.fadeOffTimer_ > .0f )
		return;

	n.fadeOffTimer_ = FadeOffTime;
	n.offsetBoost_ = 1.0f;
	n.boostAmp_ = amp;
	float speedRandomness = .6f / 15.0f;

	for( unsigned int i = 0; i < n.nConnections_; ++i )
	{
		Edge& e = edges_[ n.edgeIndices_[ i ] ];

		if( e.amp_ > .0f ) continue;

		if( e.index0_ == nodeIndex )
		{
			e.amp_ = amp;
			e.progressSpeed_ = DefaultProgressSpeed * ( .7f + speedRandomness * ( i & 15) ); 
			e.progress_ = .0f;
		}
		else
		{
			e.amp_ = amp;
			e.progressSpeed_ = -DefaultProgressSpeed * ( .8f + speedRandomness * ( i & 15) );
			e.progress_ = 1.0f;
		}
	}
}

}

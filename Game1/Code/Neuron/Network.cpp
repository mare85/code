#include <Neuron/Network.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * Neuron::Network
 */
#include <Util/RandomGenerator.h>
#include <Game/Transform.h>
namespace Neuron {


struct Nodes
{
	enum {
		nNodes = 200,
		nEdges = 400,
		nEdgesPerNode = 15,
	};
	struct Node
	{
		vmath::Vector3 pos_;
		unsigned int nConnections_ = 0;
		unsigned int edgeIndices_[nEdgesPerNode];
		vmath::Vector3 edgeDirs_[nEdgesPerNode];
	};
	struct Edge
	{
		unsigned int index0_;
		unsigned int index1_;
		float progress_ = .0f;
		float progressSpeed_ = .0f;
		float fadeOffTime_ = .0f;
		void setup ( unsigned int index0, unsigned int index1)
		{
			index0_ = index0;
			index1_ = index1;
		}
	};
	Node nodes_[nNodes];
	Edge edges_[nEdges];
	Nodes( float range)
	{
		Util::RandomGenerator gen;
		// picking random node positions
		for( unsigned int ni = 0; ni < nNodes; ++ni )
		{
			nodes_[ ni ].pos_ = vmath::Vector3( 
				gen.getFloat( -1.0f, 1.0f),
				gen.getFloat( -1.0f, 1.0f),
				gen.getFloat( -1.0f, 1.0f)
				) * range;
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
		assert(curNEdges == nEdges );
	}
	bool isFull( unsigned int index ) const { return nodes_[index].nConnections_ == nEdgesPerNode;}
	bool isConnected( unsigned int index0, unsigned int index1) const
	{
		const Node& n = nodes_[index0];
		for( unsigned int i = 0; i < n.nConnections_; ++i)
		{
			const Edge& e = edges_[ n.edgeIndices_[i] ];
			if ( e.index0_ == index1 || e.index1_ == index1 ) return true;
		}
		return false;
	}
	void addConnection( unsigned int index0, unsigned int index1, unsigned int & nCurEdges)
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
};
// Neuron::Network
void Network::_initBeziers(Graphics::GdiContext * gdiCtx)
{
	beziers_ = new vmath::Vector4[ nSegmentsPerEdge ];
	{ // filling up the bezier factors
		float step = 1.0f / nSegmentsPerEdge;
		float t = step * .5f;
		float s = 1.0f - t;
		for( unsigned int ei = 0; ei < nSegmentsPerEdge; ++ei)
		{
			beziers_[ei] = vmath::Vector4( s*s*s, 3.0f*s*s*t, 3.0f*s*t*t, t*t*t);
			t += step;
			s -= step;
		}
	}
	beziersBuffer_ = gdiCtx->createBuffer( beziers_, 
		nSegmentsPerEdge * sizeof(vmath::Vector4), Graphics::BufferType::Compute );
	srvBeziers_ = gdiCtx->createByteAddressSrv( beziersBuffer_ );
}
void Network::_initMatrixBuffer( Graphics::GdiContext * gdiContex )
{
	assert(nodes_);
	nMats_ = nodes_->nEdges * nSegmentsPerEdge;
	const unsigned int buffSize = nMats_ * sizeof( vmath::Matrix4 );
	vmath::Matrix4 *data = new vmath::Matrix4[nMats_];
	vmath::Matrix4* dataPtr = data;
	for( unsigned int ei = 0; ei < nodes_->nEdges; ++ei )
	{
		Nodes::Edge& e = nodes_->edges_[ei];
		Nodes::Node& n0 = nodes_->nodes_[ e.index0_ ];
		Nodes::Node& n1 = nodes_->nodes_[ e.index1_ ];

		vmath::Vector3 cp0 = n0.pos_;
		vmath::Vector3 cp3 = n1.pos_;
		vmath::Vector3 diff = cp3 - cp0;
		float dist = length( diff );
		unsigned int edgeNodeIndex0 = 0;
		unsigned int edgeNodeIndex1 = 0;
		for(unsigned int i = 0; i != n0.nConnections_; ++i )
			if ( ei == n0.edgeIndices_[i] ) edgeNodeIndex0 = i;
		for(unsigned int i = 0; i != n1.nConnections_; ++i )
			if ( ei == n1.edgeIndices_[i] ) edgeNodeIndex1 = i;
		vmath::Vector3& dir0 = n0.edgeDirs_[ edgeNodeIndex0 ];
		vmath::Vector3& dir1 = n1.edgeDirs_[ edgeNodeIndex1 ];
		vmath::Vector3 cp1 = cp0 + dir0 * dist * .5f;
		vmath::Vector3 cp2 = cp3 + dir1 * dist * .5f;
		// cp0 1 2 and 3 are bezier controll points
		vmath::Vector3 z = dir0;
		vmath::Vector3 x = vmath::Vector3(1.0f, .0f, .0f);
		if( z.getX() > .99f || z.getX() < .99f )
			x = vmath::Vector3( .0f, 1.0f, .0f );
		vmath::Vector3 y = normalize( cross( z,x ) );
		x = cross( y,z );

		vmath::Vector3 prevPos = cp0;
		for( unsigned int i = 0; i < nSegmentsPerEdge; ++i )
		{
			vmath::Vector4& b = beziers_[i];
			vmath::Vector3 pos = b.getX() * cp0 + b.getY() * cp1 + b.getZ() * cp2 + b.getW() * cp3;	
			z = normalize( pos - prevPos );
			x = normalize( cross( y,z ));
			y = cross( z,x );
			*dataPtr = vmath::Matrix4( vmath::Matrix3( x,y,z ), pos );
			prevPos = pos;
			++dataPtr;
		}
	}
	matrixBuffer_ = gdiContex->createBuffer( data, buffSize, Graphics::BufferType::Compute);
	delete[] data;
	srvMatrix_ = gdiContex->createByteAddressSrv(matrixBuffer_);
}
void Network::_initRadiusBuffers(Graphics::GdiContext* gdiContext)
{
	float* radiuses = new float[nSegmentsPerEdge];
	float step = 1.0f / nSegmentsPerEdge;
	float t = step * .5f;
	for( unsigned int i = 0; i != nSegmentsPerEdge; ++i)
	{
		float x = t * (1.0f - t) * 4.0f;
		radiuses[ i ] = nodeRadius_ * (1.0f - .8f * x);
		t += step;
	}
	radiusesBuffer_ = gdiContext->createBuffer( radiuses, nSegmentsPerEdge * sizeof(float), Graphics::BufferType::Compute);
	delete[] radiuses;

	unsigned int nRings = nSegmentsPerEdge * nodes_->nEdges;
	float* pulse = new float[ nRings ];
	for( unsigned int i = 0; i != nRings; ++i) pulse[i] = 1.0f;
	pulseBuffer_ = gdiContext->createBuffer( pulse, nRings * sizeof(float), Graphics::BufferType::CpuToCompute);
	delete[] pulse;
	srvRadiuses_ = gdiContext->createByteAddressSrv( radiusesBuffer_);
	srvPulse_ = gdiContext->createByteAddressSrv( pulseBuffer_ );

}
//Game::Object
void Network::loadData(Graphics::GdiContext* gdiContext) 
{
	if ( ! nodes_)
		nodes_ = new Nodes(25.0f);
	_initBeziers(gdiContext);
	_initRadiusBuffers(gdiContext);
	_initMatrixBuffer(gdiContext);
	drawCBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::BufferType::Constant);
	nVerts_ = nodes_->nEdges * nSegmentsPerEdge * 10;
	nIndices_ = nodes_->nEdges * nSegmentsPerEdge * 60;
	unsigned int nRings = nodes_->nEdges * nSegmentsPerEdge;
	unsigned int nPosBytes = nVerts_ * 12;
	vertexPosBuffer_ = gdiContext->createBuffer( nullptr, nPosBytes, Graphics::BufferType::ComputeToVertex);
	uavVertexPos_ = gdiContext->createByteAddressUav( vertexPosBuffer_ );
	unsigned int* indices = new unsigned int [ nIndices_ ];
	{
		unsigned int* outIndex = indices;
		unsigned int inIndex = 0;
		for( unsigned int i = 0; i < nRings; ++i )
		{
			for( unsigned int i2 = 0; i2 < 5; ++i2 )
			{
				unsigned int ti = i2;
				unsigned int ni = ( i2 + 1 ) % 5;
				*outIndex = inIndex + ti * 2; ++outIndex;
				*outIndex = inIndex + ti * 2 + 1; ++outIndex;
				*outIndex = inIndex + ni * 2 + 1; ++outIndex;
				*outIndex = inIndex + ti * 2; ++outIndex;
				*outIndex = inIndex + ni * 2 + 1; ++outIndex;
				*outIndex = inIndex + ni * 2; ++outIndex;
				*outIndex = inIndex + ti * 2; ++outIndex;
				*outIndex = inIndex + ni * 2 + 1; ++outIndex;
				*outIndex = inIndex + ti * 2 + 1; ++outIndex;
				*outIndex = inIndex + ti * 2; ++outIndex;
				*outIndex = inIndex + ni * 2; ++outIndex;
				*outIndex = inIndex + ni * 2 + 1; ++outIndex;
			}
			inIndex += 10;
		}
	}
	indexBuffer_ = gdiContext->createBuffer( indices, nIndices_ * sizeof(unsigned int), Graphics::BufferType::Index );
	delete[] indices;
	typedef Graphics::Semantic S;
	Graphics::VertexDesc vDescPos = { { S::Pos3 } };
	sh_ = gdiContext->createShader( "assets/Shaders/neuronColor.fx", &vDescPos, "neuronColor" );
	cs_ = gdiContext->createComputeShader( "assets/Shaders/neuron_cs.fx","cs_getVPos","neuron/cs_main" );
	
	//debugBuffer_ = gdiContext->createBuffer( nullptr, nPosBytes, Graphics::BufferType::Debug );
}
void Network::unloadData(Graphics::GdiContext* gdiContext) 
{
	//gdiContext->releaseBuffer(debugBuffer_);
	gdiContext->releaseComputeShader( cs_ );
	gdiContext->releaseShader( sh_ );
	gdiContext->releaseUav( uavVertexPos_ );
	gdiContext->releaseSrv( srvMatrix_ );
	gdiContext->releaseSrv( srvPulse_ );
	gdiContext->releaseSrv( srvRadiuses_ );
	gdiContext->releaseSrv( srvBeziers_ );

	gdiContext->releaseBuffer( indexBuffer_ );
	gdiContext->releaseBuffer( vertexPosBuffer_ );
	gdiContext->releaseBuffer( matrixBuffer_ );
	gdiContext->releaseBuffer( radiusesBuffer_ );
	gdiContext->releaseBuffer( pulseBuffer_ );
	gdiContext->releaseBuffer( beziersBuffer_ );
	delete[] beziers_; beziers_ = nullptr;
	delete nodes_; nodes_ = nullptr;
}
void Network::render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext ) 
{
	Graphics::ConstantBufferData cbData;
	cbData.view_ = renderContext->view_;
	cbData.projection_ = renderContext->projection_;
	cbData.world_ = getTransform()->getWorldPose();
	gdiContext->bindShader(sh_);
	gdiContext->updateBuffer(drawCBuff_, &cbData);
	gdiContext->setConstantBuffer(drawCBuff_);
	gdiContext->drawTriangles(indexBuffer_, vertexPosBuffer_, nIndices_);
}
void Network::updateGfx(Graphics::GdiContext* gdiContext) 
{
	gdiContext->dispatchComputeShader(cs_,{srvMatrix_,srvRadiuses_,srvPulse_,srvBeziers_},{uavVertexPos_},nMats_);
	//gdiContext->copyBuffer( debugBuffer_, vertexPosBuffer_ );
	//float* outVerts = reinterpret_cast<float*>(gdiContext->mapRead(debugBuffer_));
	//gdiContext->unmap(debugBuffer_);
}
//Game::Updater
void Network::start() 
{
}
void Network::update(const Game::UpdateContext* uctx) 
{
	(void)uctx;
}
void Network::stop() 
{
}

}

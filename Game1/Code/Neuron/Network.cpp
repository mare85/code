#include <Neuron/Network.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * Neuron::Network
 */
#include <Util/RandomGenerator.h>
#include <Game/Transform.h>
#include <Neuron/Nodes.h>

namespace Neuron {

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
	assert( nodes_ );
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

void Network::_initProgAmp( Graphics::GdiContext* ctx )
{
	assert( nodes_ );
	const unsigned int buffSize = nodes_->nEdges * sizeof( ProgAmp );
	progAmpBuffer_ = ctx->createBuffer( nullptr, buffSize, Graphics::BufferType::CpuToCompute );
	srvProgAmp_ = ctx->createByteAddressSrv( progAmpBuffer_ );
}

void Network::_initOffsets( Graphics::GdiContext* ctx )
{
	assert( nodes_ );
	const unsigned int buffSize = nodes_->nEdges * 2 * sizeof( vmath::Vector3 );
	offsetsBuffer_ = ctx->createBuffer( nullptr, buffSize, Graphics::BufferType::CpuToCompute );
	srvOffsets_ = ctx->createByteAddressSrv( offsetsBuffer_ );
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
	srvRadiuses_ = gdiContext->createByteAddressSrv( radiusesBuffer_);
}

Network::Network() 
: Game::Object( "network" )
{
	gen = new Util::RandomGenerator();
}
Network::~Network()
{
	delete gen;
}

//Game::Object
void Network::loadData(Graphics::GdiContext* gdiContext) 
{
	if ( ! nodes_)
		nodes_ = new Nodes(25.0f);

	_initBeziers( gdiContext );
	_initRadiusBuffers( gdiContext );
	_initMatrixBuffer( gdiContext );
	_initProgAmp( gdiContext );
	_initOffsets( gdiContext );
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
	csVPos_ = gdiContext->createComputeShader( "assets/Shaders/neuron_cs.fx","cs_getVPos", "neuron/csvpos" );
	
	//debugBuffer_ = gdiContext->createBuffer( nullptr, nPosBytes, Graphics::BufferType::Debug );
}
void Network::unloadData(Graphics::GdiContext* gdiContext) 
{
	//gdiContext->releaseBuffer(debugBuffer_);
	gdiContext->releaseComputeShader( csVPos_ );
	gdiContext->releaseShader( sh_ );
	gdiContext->releaseUav( uavVertexPos_ );
	gdiContext->releaseSrv( srvMatrix_ );
	gdiContext->releaseSrv( srvRadiuses_ );
	gdiContext->releaseSrv( srvBeziers_ );
	gdiContext->releaseSrv( srvProgAmp_ );
	gdiContext->releaseSrv( srvOffsets_ );


	gdiContext->releaseBuffer( indexBuffer_ );
	gdiContext->releaseBuffer( vertexPosBuffer_ );
	gdiContext->releaseBuffer( matrixBuffer_ );
	gdiContext->releaseBuffer( radiusesBuffer_ );
	gdiContext->releaseBuffer( beziersBuffer_ );
	gdiContext->releaseBuffer( progAmpBuffer_ );
	gdiContext->releaseBuffer( offsetsBuffer_ );
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
	//updating prog amp buffer
	ProgAmp* progAmpData = reinterpret_cast<ProgAmp*>( gdiContext->mapWrite( progAmpBuffer_ ) );
	ProgAmp* paPtr = progAmpData;
	
	for( unsigned int i = 0; i < nodes_->nEdges; ++i )
	{
		Nodes::Edge& e = nodes_->edges_[ i ];
		paPtr->amp = e.amp_;
		paPtr->progress = e.progress_;
		++paPtr;
	}
	gdiContext->unmap( progAmpBuffer_ );

	//updating offsets
	vmath::Vector3* offsetsPtr = reinterpret_cast< vmath::Vector3* >( gdiContext->mapWrite( offsetsBuffer_ ) );
	for( unsigned int i = 0; i < nodes_->nEdges; ++i )
	{
		Nodes::Edge& e = nodes_->edges_[ i ];
		Nodes::Node& n0 = nodes_->nodes_[ e.index0_ ];
		Nodes::Node& n1 = nodes_->nodes_[ e.index1_ ];
		*offsetsPtr = n0.offsetPhase_; ++offsetsPtr;
		*offsetsPtr = n1.offsetPhase_; ++offsetsPtr;
	}
	gdiContext->unmap( offsetsBuffer_ );

	gdiContext->dispatchComputeShader(csVPos_,{srvMatrix_,srvRadiuses_,srvProgAmp_,srvBeziers_, srvOffsets_},{uavVertexPos_},nMats_);
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
	if( ! nodes_ )
		return;

	pulseTimer -= uctx->deltaTime;
	
	if( pulseTimer < .0f )
	{
		nodes_->burstSignal( gen->getUint( nodes_->nNodes ),Nodes::StartingAmp );
		pulseTimer = 2.8f;
	}
	
	nodes_->update(uctx->deltaTime);
}
void Network::stop() 
{
}

}

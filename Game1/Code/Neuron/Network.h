#ifndef NEURON_NETWORK_DEF
#define NEURON_NETWORK_DEF
/*
 * (C) Copytight 2017 Marek Bielawski
 * Neuron::Network
 */
#include <Game/Updater.h>
#include <Graphics/GdiContext.h>

#include <Game/Object.h>

namespace Util { class RandomGenerator; }
namespace Neuron {

struct Nodes;

class Network :
	public Game::Updater,
	public Game::Object
{
	static const unsigned int nSegmentsPerEdge;
	static const float NodesRange;
	static const float NodeOffsetRange;

	struct ComputeCBuffData {
		unsigned int nSegmentsPerEdge_;
		float offsetRange_;
		float padding[2];

	};
	struct ProgAmp { float progress; float amp; };
	Util::RandomGenerator* gen;
	vmath::Vector4* beziers_ = nullptr;
	Graphics::Shader* sh_ = nullptr;
	Graphics::ComputeShader* csVPos_ = nullptr;
	Graphics::Buffer* radiusesBuffer_ = nullptr;
	Graphics::Buffer* beziersBuffer_ = nullptr;
	Graphics::Buffer* matrixBuffer_ = nullptr;
	Graphics::Buffer* vertexPosBuffer_ = nullptr;
	Graphics::Buffer* drawCBuff_ = nullptr;
	Graphics::Buffer* computeCBuff_ = nullptr;
	Graphics::Buffer* indexBuffer_ = nullptr;
	Graphics::Buffer* debugBuffer_ = nullptr;
	Graphics::Buffer* progAmpBuffer_ = nullptr;
	Graphics::Buffer* offsetsBuffer_ = nullptr;

	Graphics::Srv* srvRadiuses_ = nullptr;
	Graphics::Srv* srvBeziers_ = nullptr;
	Graphics::Srv* srvProgAmp_ = nullptr;
	Graphics::Srv* srvMatrix_ = nullptr;
	Graphics::Srv* srvOffsets_ = nullptr;
	Graphics::Uav* uavVertexPos_ = nullptr;

	Nodes* nodes_ = nullptr;
	float nodeRadius_ = .5f;
	float ringThickness = .1f;
	float pulseTimer = .2f;
	unsigned int nMats_ = 0;
	unsigned int nVerts_ = 0;
	unsigned int nIndices_ = 0;
	void _initBeziers( Graphics::GdiContext* gdiCtx );
	void _initMatrixBuffer( Graphics::GdiContext* ctx );
	void _initProgAmp( Graphics::GdiContext* ctx );
	void _initOffsets( Graphics::GdiContext* ctx );
	void _initRadiusBuffers(Graphics::GdiContext*);
public:
	Network();
	~Network();

	//Game::Object
	void loadData(Graphics::GdiContext* gdiContext) override;
	void unloadData(Graphics::GdiContext* gdiContext) override;
	void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext ) override;
	void updateGfx(Graphics::GdiContext* gdiContext) override; 
	//Game::Updater
	void start() override;
	void update(const Game::UpdateContext* uctx) override;
	void stop() override;

	bool isUpdaterManagedByScene() const { return false; }
};

}
#endif

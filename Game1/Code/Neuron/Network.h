#ifndef NEURON_NETWORK_DEF
#define NEURON_NETWORK_DEF
/*
 * (C) Copytight 2017 Marek Bielawski
 * Neuron::Network
 */
#include <Game/Updater.h>
#include <Graphics/GdiContext.h>

#include <Game/Object.h>

namespace Neuron {

struct Nodes;
class Network :
	public Game::Updater,
	public Game::Object
{
	enum {
		nSegmentsPerEdge = 50 
	};
	vmath::Vector4* beziers_ = nullptr;
	Graphics::Shader* sh_ = nullptr;
	Graphics::ComputeShader* cs_ = nullptr;
	Graphics::Buffer* radiusesBuffer_ = nullptr;
	Graphics::Buffer* beziersBuffer_ = nullptr;
	Graphics::Buffer* pulseBuffer_ = nullptr;
	Graphics::Buffer* matrixBuffer_ = nullptr;
	Graphics::Buffer* vertexPosBuffer_ = nullptr;
	Graphics::Buffer* drawCBuff_ = nullptr;
	Graphics::Buffer* indexBuffer_ = nullptr;
	Graphics::Buffer* debugBuffer_ = nullptr;

	Graphics::Srv* srvRadiuses_ = nullptr;
	Graphics::Srv* srvBeziers_ = nullptr;
	Graphics::Srv* srvPulse_ = nullptr;
	Graphics::Srv* srvMatrix_ = nullptr;
	Graphics::Uav* uavVertexPos_ = nullptr;

	Nodes* nodes_ = nullptr;
	float nodeRadius_ = 1.0f;
	float ringThickness = .1f;
	unsigned int nMats_ = 0;
	unsigned int nVerts_ = 0;
	unsigned int nIndices_ = 0;
	void _initBeziers(Graphics::GdiContext* gdiCtx);
	void _initMatrixBuffer(Graphics::GdiContext* ctx);
	void _initRadiusBuffers(Graphics::GdiContext*);
public:

	//Game::Object
	void loadData(Graphics::GdiContext* gdiContext) override;
	void unloadData(Graphics::GdiContext* gdiContext) override;
	void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext ) override;
	void updateGfx(Graphics::GdiContext* gdiContext) override; 
	//Game::Updater
	void start() override;
	void update(const Game::UpdateContext* uctx) override;
	void stop() override;
};

}
#endif

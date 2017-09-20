#ifndef NEURON_TRAVELCAMERA_DEF
#define NEURON_TRAVELCAMERA_DEF
/*
 * (C) Copytight 2017 Marek Bielawski
 * Neuron::TravelCamera
 */
#include <Util/Utils.h>
#include <Util/RandomGenerator.h>
#include <Game/Updater.h>

namespace Game { class Scene; class Camera; class Transform; }

namespace Neuron {

struct Nodes;

class TravelCamera
	: public Game::Updater
{
	static const unsigned int InvalidTarget = 0xffffffff;
	static const float Speed;
	static const float ChangeDist;
	static const float ChangeFactor;
	Nodes* nodes_ = nullptr;
	Game::Transform * rootTransform_ = nullptr;
	Game::Scene * scene_ = nullptr;
	Game::Camera* cam_ = nullptr;
	vmath::Vector3 pos_;
	vmath::Vector3 front_;
	vmath::Vector3 side_;
	Util::RandomGenerator gen_;
	unsigned int targetIndex_ = InvalidTarget;
	float changeTimer_ = .0f;
public:
	TravelCamera( Game::Scene* scene, Nodes* nodes, const char* name );
	~TravelCamera();
	void start() override;
	void update(const Game::UpdateContext* uctx) override;
	void stop() override;
};

}
#endif

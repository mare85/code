#ifndef GAMEUTILS_FREEROAMCAMERA_DEF
#define GAMEUTILS_FREEROAMCAMERA_DEF
/*
 * (C) Copytight 2017 Marek Bielawski
 * GameUtils::FreeRoamCamera
 */
#include <Util/Utils.h>
#include <Game/Updater.h>
namespace Game { class Scene; class Camera; class Transform; }

namespace GameUtils {

class FreeRoamCamera
	: public Game::Updater
{
	static const float MaxSpeed;
	static const float Acceleration;
	static const float Damping;
	static const float AngleDamping;
	static const float MaxRotSpeed;
	static const float AngleAcceleration;
	Game::Transform * rootTransform_ = nullptr;
	Game::Scene * scene_ = nullptr;
	Game::Camera* cam_;
	vmath::Vector3 position_;
	vmath::Vector3 speed_;
	vmath::Vector3 front_;
	float yaw_;
	float pitch_;
	float yawSpeed_;
	float pitchSpeed_;
public:
	FreeRoamCamera( Game::Scene* scene, const char* name );
	~FreeRoamCamera() {}
	void start();
	void update( const Game::UpdateContext* uctx );
	void stop();
	vmath::Vector3 getFront() { return front_; }
	vmath::Vector3 getPosition() { return position_; }
};

}
#endif

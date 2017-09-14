#pragma once

#include <Game/Updater.h>

#include <Util/Utils.h>

namespace Game
{
	class Scene;
	class Transform;
	class Camera;
}

namespace MusicRoom
{

	class Character
		: public Game::Updater
	{
		Game::Transform * rootTransform_ = nullptr;
		Game::Transform * cameraTransform_ = nullptr;
		Game::Transform * extraXModifyTransform_ = nullptr;
		Game::Scene * scene_ = nullptr;
		Game::Camera* cam_;
		vmath::Vector3 position_;
		vmath::Vector3 front_;
		float yaw_;
		float pitch_;
		

	public:
		Character( Game::Scene* scene, const char* name);
		~Character() {}

		void start();
		void update(const Game::UpdateContext* uctx);

		void stop();
		vmath::Vector3 getFront() { return front_; }
		vmath::Vector3 getPosition() { return position_; }
		void setExtraTransform( Game::Transform* t ) { extraXModifyTransform_ = t; }
	};


}

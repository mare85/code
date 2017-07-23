#pragma once

#include <Game/Updater.h>
#include <Util/Utils.h>
#include <Util/RandomGenerator.h>
#include <Input/CharacterInput.h>

namespace Game
{
	class Scene;
	class Transform;
	class Camera;
}

namespace Sound
{
	class Buffer;
}

namespace Physics
{
	struct Ground;
}
namespace Black
{
	//class UpdatealbeMesh;
	class CharacterObject;
	class Character 
		: public Game::Updater
	{
		Input::CharacterInput input_;
		enum
		{
			eHairCount = 7,
			eHairSteps = 6,
			eBodySteps = 24,
			eHairVerts = 2 * eHairCount * eHairSteps
		};
		Util::RandomGenerator gen;

		vmath::Vector3 position_ = vmath::Vector3( .0f);
		vmath::Vector3 lastGroundTangent_ = vmath::Vector3(1.0f, 0.0f, .0f);
		vmath::Vector3 lastGroundNormal_ = vmath::Vector3(.0f, 1.0f, .0f);
		vmath::Vector3 velocity_ = vmath::Vector3(.0f);

		vmath::Vector3 headCenter_ = vmath::Vector3(.0f, 1.f, .0f);
		vmath::Vector3 waveCenterLowPass_ = vmath::Vector3( .0f );
		float waveLowPas_ = .0f;
		
		bool onGround_ = true;
		float rundownProgress_ = .0f;
		bool runningDown_ = false;
		unsigned int jumpSoundId_ = 0;
		Game::Scene* scene_ = nullptr;
		Game::Transform* transform_ = nullptr;
		Black::CharacterObject* mesh_ = nullptr;

		struct _Camera
		{
			Game::Camera* cam_ = nullptr;
			vmath::Vector3 pos_ = vmath::Vector3(.0f, .0f, .0f);
			vmath::Vector3 vel_ = vmath::Vector3(.0f, .0f, .0f);
			Game::Transform* transform_ = nullptr;
			float extend_ = 2.0f;
			float maxSpeed_ = 15.0f;
			float damping_ = 2.0f;
			void _Update(float dt, const vmath::Vector3& pos);
		} cam_;

		float groundSpeedDamping_ = 4.0f;
		float maxSpeed_ = 3.0f;
		float targetMaxSpeed_ = 3.0f;

		float walkPhase_ = .0f;
		float walkDir_ = .0f;
		float walkDirSmooth_ = .0f;
		float gravity_ = 10.0f;
		float jumpTimer_ = .1f;
		float jumpBlend_ = 0.0f;
		bool switchedPlane_ = true;
		bool switchPlaneAllowed_ = true;
			
		void _UpdateMesh();
		void _UpdateWalking(float dt, float left, bool jumpButton);
		void _UpdateRunningDown(float dt, float left, bool jumpButton);
		void _UpdateFlying(float dt, float left, bool jumpButton, float up);

		struct _Hair
		{
			Character* thisNode_ = nullptr;;
			vmath::Vector3 start_[eHairCount];
			vmath::Vector3 nextPointBase_[eHairCount];
			vmath::Vector3 lastPointBase_ [eHairCount];
			vmath::Vector3 velocity1_[eHairCount];
			vmath::Vector3 velocity2_[eHairCount];
			vmath::Vector3 offset1_[eHairCount];
			vmath::Vector3 offset2_[eHairCount];
			float forceAngle_[eHairCount];
			float damping_[eHairCount];
			float massInv_[eHairCount];

			float maxOffset_ = .15f;
			void init(Character* thisNode);
			void update(float dt );
		} hair_;

	public:
		Character(Game::Scene* scene, const char* name);
		~Character() {};

		void start() override;
		void update(const Game::UpdateContext* uctx) override;
		void stop() override;

		static Character* GetInstance();
		void teleport(unsigned int layerId, float x);
		void setVisible(bool val);
		void setRundown(bool val) { runningDown_ = val; }
		void setTargetMaxSpeed(float val) { targetMaxSpeed_ = val; }
		void setswitchPlaneAllowed(bool val) { switchPlaneAllowed_ = val; }
		vmath::Vector3 cameraPosition() const { return cam_.pos_; }
	};

}
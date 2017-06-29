#pragma once
#include <Black/UpdatealbeMesh.h>
#include <Game/Updater.h>
#include <Black/GroundManager.h>
#include <Util/Utils.h>
#include <Util/RandomGenerator.h>
namespace Game
{
	class Scene;
}
namespace Black
{
	
	class BirdsManager
		: public Game::Updater
	{
		enum {
			nVertsPerBoid = 42,
		};
		UpdatealbeMesh* mesh_[GroundManager::eNLayers];
		Game::Scene* scene_ = nullptr;
		vmath::Vector3* positions_ = nullptr;
		vmath::Vector3* velocities_ = nullptr;
		float* sizes_ = nullptr;
		float* phases_ = nullptr;
		float* ttls_ = nullptr;
		unsigned int numBoids_[GroundManager::eNLayers];
		unsigned int maxBoidPerLayer_ = 0;
		float rate_ = 0.0f;
		float burstMax_ = .0f;
		float burstTime_ = .0f;
		float burstProgress_ = .0f;
		float toEmit_ = .0f;
		Util::RandomGenerator gen;

		void _UpdateLayer(const Game::UpdateContext* uctx, unsigned int layerIndex);
		void _DrawBird(unsigned int layerIndex, unsigned int inLayerIndex, vmath::Vector4* vertBuffer);
	public:
		BirdsManager(unsigned int maxBoidCount, Game::Scene* scene);
		~BirdsManager();
		void start();
		void update(const Game::UpdateContext* uctx);
		void stop();
		void burst( float maxRate, float time );
		static BirdsManager* Instance();
		static void Release();

	};
}


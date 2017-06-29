#pragma once
#include <Game/Updater.h>
#include <Game/Scene.h>
#include <Util/Utils.h>
#include <Black/GroundManager.h>

namespace Game
{
	class Scene;
}

namespace Black
{
	class Event;
	class EventSystem
		: public Game::Updater
	{
		enum {
			nEventsPerSector = 20,
			nSectorsPerLayer = GroundManager::eNSectors,
			nLayers = GroundManager::eNLayers
		};

		Event* sectors_[nEventsPerSector * nSectorsPerLayer * nLayers];
		unsigned int sectorCounts_[nSectorsPerLayer * nLayers];

		vmath::Vector3 position_ = vmath::Vector3(.0f, .0f, .0f);
		float x_ = .0f;
		unsigned int layerId_ = 0;
		unsigned int levelId_ = 0;
		unsigned int currentSector_ = 0;

		Event* currentEvents[nSectorsPerLayer];
		unsigned int nCurrentEvents = 0;
		Game::Scene* scene_;
		EventSystem();
	public:
		static void StartUp( Game::Scene* scene);
		static void ShutDown();
		static EventSystem* GetInstance();
		~EventSystem() {};


		void addEvent(Event* evt);
		void removeEvent(Event* evt);
		void feedPosition(const vmath::Vector3 &pos, unsigned int layerId, unsigned int levelId);
		void start() {};
		void update(const Game::UpdateContext* uctx);
		void stop() {};
		bool isInEvent(Event* evt);
	};


}
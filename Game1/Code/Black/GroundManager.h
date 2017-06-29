#pragma once

#include <windows.h>
namespace Game
{
	class Scene;
}
namespace Black
{
	class GroundMesh;
	struct Ground;
	class GroundManager
	{
	public:
		enum {
			eNLayers = 20,
			eNSectors = 100
		};
		static float SegmentWidth;
		static float HalfSegmentWidth;
		static float WorldWrap;
		static float WorldWidth;
	private:
		GroundManager() {}
		~GroundManager() {}
		GroundMesh* meshes_[eNLayers];
		Black::Ground* grounds_[eNLayers];
		int layerIndex_ = 5;
		float heights_[eNLayers * eNSectors];
		unsigned int levelId_ = 0;


	public:
		static void startUp( Game::Scene* scene , unsigned int levelId );
		static void shutDown();
		static GroundManager* getInstance();
		int getLayerIndex() const { return layerIndex_; }
		Black::Ground* getCurrentGround() { 
			return grounds_[( layerIndex_ + eNLayers ) % eNLayers];
		}
		unsigned int getLevelId() { return levelId_;}

		Black::Ground* getGround( unsigned int i ) {
			return grounds_[( i + eNLayers ) % eNLayers];
		}


		bool switchToNextLowerGround(float x, float y, float dir);
		void inc() { 
			layerIndex_ = (layerIndex_ + eNLayers + 1) % eNLayers;
		}
		void dec() { 
			layerIndex_ = (layerIndex_ + eNLayers - 1) % eNLayers;
		}
		void setLayer(unsigned int val) { layerIndex_ = val % eNLayers; }
		void elevate(unsigned int index, float value);
		void saveLayer( unsigned int layerId );
		void save();
		

	};

}


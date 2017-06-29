#pragma once

#include <Game/StaticMesh.h>
namespace Game
{

	class Transform;
	class Scene;
}
namespace Black
{

	class GrassMesh
		: public Game::StaticMesh
	{
		unsigned int layerId_ = 0;
		unsigned int sectorId_ = 0;
		unsigned int grassSteps_ = 8;
		unsigned int grassCount_ = 250;
		float height_ = .1f;
		float xCenter_ = .0f;
		void _generateGrass(vmath::Vector4* outData);
		static void __OnParamChange(Game::Object* obj);
	public:
		GrassMesh( unsigned int layerId, unsigned int sectorId);
		~GrassMesh();

		bool hasSerialize() { return true; }
		void serialize(unsigned char* buffer, unsigned int* outBytes, unsigned int maxBytes);
		Game::Transform* createAndAddTransform( Game::Scene* scene ) const ;

	};



}
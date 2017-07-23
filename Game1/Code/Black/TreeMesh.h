#pragma once


#include <Game/StaticMesh.h>

namespace Black
{
	class TreeMesh
		: public Game::StaticMesh
	{
	public:
		float elevation = -.3f;
		TreeMesh(const char* name);
		~TreeMesh();


		bool hasSerialize() override { return true;}
		void serialize( unsigned char* buffer, unsigned int* outBytes, unsigned int maxBytes ) override;

		static void __SetElevationCallback(Game::Object* obj);
	};
}

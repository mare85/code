#pragma once

namespace Black
{
	class TreeMesh;
	class BlackSprite;
	class GrassMesh;
	class Event;
	namespace Editor
	{
		class add
		{
		public:
			static TreeMesh* tree();
			static GrassMesh* grass();
			static BlackSprite* sprite(const char* path);
			static Event* event();
		};

	}
}

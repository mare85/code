#pragma once
#include <Game\Object.h>
#include <cstring>
namespace Game
{
	class Sprite :
		public Game::Object
	{
		
	public:
		void loadData(Graphics::GdiContext* gdiContext);
		void unloadData(Graphics::GdiContext* gdiContext);
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);
		Sprite(const char* name, const char* filename, int layer) 
			: Object(name) 
		{
			strcpy_s(filename_, filename); layer_ = layer;
		}
		~Sprite() {};
		static void SetDotsPerUnit(float val) { _DotsPerUnit = val; }
	protected:
		char filename_[255];
		Graphics::Shader* sh_ = nullptr;
		Graphics::Buffer* vBuff_ = nullptr;
		Graphics::Buffer* cBuff_ = nullptr;
		Graphics::Texture* texture_ = nullptr;

		static float _DotsPerUnit;
	};
}


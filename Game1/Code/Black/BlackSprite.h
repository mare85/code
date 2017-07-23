#pragma once

#include <Game/Object.h>
#include <Graphics/Texture.h>
#include <Graphics/Shader.h>
#include <Graphics/GdiContext.h>

namespace Black
{
	class BlackSprite :
		public Game::Object
	{
		
	public:
		void loadData(Graphics::GdiContext* gdiContext) override;
		void unloadData(Graphics::GdiContext* gdiContext) override;
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext) override;
		bool hasSerialize() { return true; }
		void serialize(unsigned char* buffer, unsigned int* outBytes, unsigned int maxBytes );
		const char* getFilename() {return filename_; }
		BlackSprite(const char* name, const char* filename, int layer, float height);

		~BlackSprite() {};
	protected:
		float elevation = .0f;
		char filename_[255];
		Graphics::Shader* sh_ = nullptr;
		Graphics::Buffer* vBuff_ = nullptr;
		Graphics::Buffer* cBuff_ = nullptr;
		Graphics::Texture* texture_ = nullptr;
		float height_ = 1.0f;
		static void __SetParamCallback(Game::Object * obj);
		
	};
}


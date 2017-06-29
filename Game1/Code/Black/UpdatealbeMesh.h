#pragma once

#include <Game/Object.h>
#include <Graphics/GdiContext.h>

namespace Graphics
{
	class Shader;
}
namespace Black
{

	class UpdatealbeMesh
		: public Game::Object
	{

		Graphics::Shader* sh_;
		Graphics::Buffer* vBuff_[2];
		Graphics::Buffer* cBuff_;
		unsigned int vertCount_ = 0;
		unsigned char currRender_ = 0;
		unsigned char currUpdate_ = 1;
		char needUpdate = 2;
		vmath::Vector4* vertsGpu_ = nullptr;


	public:
		UpdatealbeMesh(const char* name, unsigned int vertCount)
			: Game::Object(name)
			, vertCount_(vertCount)
			{}
		~UpdatealbeMesh() {};

		void loadData(Graphics::GdiContext* gdiContext);
		void unloadData(Graphics::GdiContext* gdiContext);
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);
		void updateGfx(Graphics::GdiContext* gdiContext);
		vmath::Vector4* getMappedData() { return vertsGpu_; }

	};


}
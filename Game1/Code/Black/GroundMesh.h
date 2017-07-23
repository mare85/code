#pragma once


#include <Game/Object.h>
#include <Graphics/GdiContext.h>

namespace Graphics
{
	class Shader;
}

namespace Black
{

	struct Ground;
	class GroundMesh
		: public Game::Object
	{

		enum {
			eNSteps = 15
		};

		Graphics::Shader* sh_;
		Graphics::Buffer* vBuff_;
		Graphics::Buffer* cBuff_;
		Black::Ground* ground_;
		unsigned int vertCount_;
		bool forceUpdate_ = false;
		void _FillBuffer( vmath::Vector4* buff);

	public:
		GroundMesh(const char* name, Black::Ground* ground)
			: Game::Object(name)
			, ground_( ground )
		{}
		~GroundMesh();

		void loadData(Graphics::GdiContext* gdiContext) override;
		void unloadData(Graphics::GdiContext* gdiContext) override;
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext) override;
		void updateGfx(Graphics::GdiContext* gdiContext) override;
		void requestUpdate() { forceUpdate_ = true; }
	};

}

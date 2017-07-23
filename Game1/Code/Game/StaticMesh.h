#pragma once
#include <Game/Object.h>
#include <Graphics/VertexTypes.h>
#include <Graphics/GdiContext.h>

namespace Game
{
	class StaticMesh :
		public Game::Object
	{
	protected:
		unsigned int vertCount_ = 0;
		void* data_ = nullptr;
		char shaderFilename_[ 255 ];
		char shaderCachename_[ 255 ];
		Graphics::VertexDesc vdesc_;
		Graphics::Shader* sh_ = nullptr;
		Graphics::Buffer* vBuff_ = nullptr;
		Graphics::Buffer* cBuff_ = nullptr;
		float minx_ = -1.0f;
		float maxx_ = -1.0f;
	public:
		StaticMesh( const char* name ) : Object(name){}
		~StaticMesh() {
			if (data_)
			{
				delete[] data_;
			}
		}
		void loadData(Graphics::GdiContext* gdiContext) override;
		void unloadData(Graphics::GdiContext* gdiContext) override;
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext) override;
		void* allocateData(unsigned int vertCount, Graphics::VertexDesc* vdesc);
		void setShader(const char* shaderName, const char* shaderCachename = nullptr);
		void setBounds( float minv, float maxv ) { minx_ = minv; maxx_ = maxv; }
	};
}
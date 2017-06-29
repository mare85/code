#pragma once
#include <Graphics/GdiContext.h>
#include <Graphics/Shader.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <Util/Utils.h>

namespace Graphics
{
	class Texture;

	

	class RenderTarget
	{
		friend class GdiContext;
	
	protected: 
		Buffer* vbuff_ = nullptr;
		Graphics::Buffer* cBuff_ = nullptr;
		
		ID3D11RenderTargetView* view_ = nullptr;
		ID3D11Texture2D* texture_ = nullptr;
		ID3D11ShaderResourceView* resourceView_ = nullptr;
		ID3D11SamplerState* sampler_;
		unsigned int width_;
		unsigned int heigth_;
		RenderTarget() {};
		~RenderTarget() {};

	public:
		
		void draw(Shader* sh, Graphics::GdiContext * gdiContext, void* cbData);
		void draw(Shader* sh, Graphics::GdiContext * gdiContext, void* cbData, RenderTarget* other);
		void draw(Shader* sh, Graphics::GdiContext * gdiContext, void* cbData, RenderTarget* other, RenderTarget* other2);
		void draw(Shader* sh, Graphics::GdiContext * gdiContext, void* cbData, RenderTarget* other, RenderTarget* other2, RenderTarget* other3);
		void draw(Shader* sh, Graphics::GdiContext * gdiContext, void* cbData, RenderTarget* other, RenderTarget* other2, RenderTarget* other3, RenderTarget* other4);
		unsigned int getWidth() const { return width_; }
		unsigned int getHeight() const { return heigth_; }
	};

	class Postprocess
		: public RenderTarget
	{
		friend class GdiContext;
		Shader* sh_ = nullptr;
	public:
		void draw( Graphics::GdiContext * gdiContext, void* cbData);
		void draw( Graphics::GdiContext * gdiContext, void* cbData, RenderTarget* other);
		void draw( Graphics::GdiContext * gdiContext, void* cbData, RenderTarget* other, RenderTarget* other2);
	};

}
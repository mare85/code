#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

namespace Graphics
{
	class Texture
	{
		friend class GdiContext;
	private:
		ID3D11ShaderResourceView* tex_;
		ID3D11SamplerState* sampler_;
		bool stored_ = false;
		Texture() {}
		~Texture() {}
	public:
		void getSize(unsigned int* width, unsigned int* height);
	};
	enum {
		nShaderTextureSlots = 4

	};

	struct TextureDesc
	{
		friend class GdiContext;
		enum Mode {
			eWrap = 0,
			eClamp,
			eMirror,
			eModeNum
		} mode_;
		enum {
			eLinear,
			ePoint,
			eAnisotropic,
			eFilterNum,
		} filter_;
	private:
		D3D11_TEXTURE_ADDRESS_MODE getGdiMode();
		D3D11_FILTER getGdiFilter();
	};

}

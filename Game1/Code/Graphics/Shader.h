#pragma once

#include <Graphics/Texture.h>
#include <Graphics/VertexTypes.h>

namespace Graphics
{

	class Shader
	{
		friend class GdiContext;
	private:
		ID3D11VertexShader* vs_;
		ID3D11PixelShader* ps_;
		ID3D11InputLayout* il_;
		Texture* textures_[nShaderTextureSlots];
		VertexDesc desc_;
		bool stored_ = false;
		Shader() { clearTextures(); }
		~Shader() {}
	public:
		void clearTextures() { for (int i = 0; i < nShaderTextureSlots; ++i) unsetTexture(i); }
		void setTexture(unsigned int slot, Texture* tex) { textures_[slot] = tex; }
		void unsetTexture(unsigned int slot) {
			textures_[slot] = nullptr;
		}
	};

	class ComputeShader
	{
		friend class GdiContext;
	private:
		ID3D11ComputeShader* cs_;
		bool stored_ = false;
	};
}
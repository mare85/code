#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <Util/Utils.h>
namespace Graphics
{
	enum VertexDescPreset
	{
		POS4 = 0,
		POS4UV4 = 1,
		POS4COL4 = 2,
		POS3COL3UV2 = 3,
		COL4 = 4
	};

	struct VertexDesc
	{

		D3D11_INPUT_ELEMENT_DESC* layout_;
		unsigned int nElems_;
		unsigned int stride_;

		static VertexDesc get(VertexDescPreset preset);
		void copyFrom(VertexDesc* other);
	};

	struct VertexP4UV4
	{
		vmath::Vector4 pos;
		vmath::Vector4 uv;
	};
}

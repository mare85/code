#include "VertexTypes.h"
namespace Graphics
{
	namespace __Layouts
	{
		static D3D11_INPUT_ELEMENT_DESC pos4[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		static D3D11_INPUT_ELEMENT_DESC pos4uv4[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		static D3D11_INPUT_ELEMENT_DESC pos4col4[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		static D3D11_INPUT_ELEMENT_DESC pos3col3uv2[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		static D3D11_INPUT_ELEMENT_DESC col4[] =
		{
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
	}
	VertexDesc VertexDesc::get(VertexDescPreset preset)
	{
		if (preset == POS4)
		{
			VertexDesc desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos4;
			desc.nElems_ = 1;
			desc.stride_ = 16;
			return desc;
		}
		else if (preset == POS4UV4)
		{
			VertexDesc desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos4uv4;
			desc.nElems_ = 2;
			desc.stride_ = 32;
			return desc;
		}
		else if(preset == POS4COL4)
		{
			VertexDesc desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos4col4;
			desc.nElems_ = 2;
			desc.stride_ = 32;
			return desc;
		}
		else if (preset == POS3COL3UV2)
		{
			VertexDesc desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos3col3uv2;
			desc.nElems_ = 3;
			desc.stride_ = 32;
			return desc;
		}
		else if (preset == COL4)
		{
			VertexDesc desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::col4;
			desc.nElems_ = 1;
			desc.stride_ = 16;
			return desc;
		}
		//ASSERT_PRIMARY(false);
		VertexDesc desc;
		desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos4;
		desc.nElems_ = 1;
		desc.stride_ = 16;
		return desc;
	}
	void VertexDesc::copyFrom(VertexDesc * other)
	{
		layout_ = other->layout_;
		nElems_ = other->nElems_;
		stride_ = other->stride_;
	}


}

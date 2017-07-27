#include "VertexTypes.h"
#include <assert.h>
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

	VertexDescDeprecated VertexDescDeprecated::get(VertexDescPreset preset)
	{
		if (preset == POS4)
		{
			VertexDescDeprecated desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos4;
			desc.nElems_ = 1;
			desc.stride_ = 16;
			return desc;
		}
		else if (preset == POS4UV4)
		{
			VertexDescDeprecated desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos4uv4;
			desc.nElems_ = 2;
			desc.stride_ = 32;
			return desc;
		}
		else if(preset == POS4COL4)
		{
			VertexDescDeprecated desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos4col4;
			desc.nElems_ = 2;
			desc.stride_ = 32;
			return desc;
		}
		else if (preset == POS3COL3UV2)
		{
			VertexDescDeprecated desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos3col3uv2;
			desc.nElems_ = 3;
			desc.stride_ = 32;
			return desc;
		}
		else if (preset == COL4)
		{
			VertexDescDeprecated desc;
			desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::col4;
			desc.nElems_ = 1;
			desc.stride_ = 16;
			return desc;
		}
		//ASSERT_PRIMARY(false);
		VertexDescDeprecated desc;
		desc.layout_ = (D3D11_INPUT_ELEMENT_DESC*)__Layouts::pos4;
		desc.nElems_ = 1;
		desc.stride_ = 16;
		return desc;
	}
	void VertexDescDeprecated::copyFrom(VertexDescDeprecated * other)
	{
		layout_ = other->layout_;
		nElems_ = other->nElems_;
		stride_ = other->stride_;
	}


	VertexDesc::VertexDesc(BufferSemanticList list)
	{
		nElems_ = 0;
		nBuffers_ = 0;
		D3D11_INPUT_ELEMENT_DESC* layoutPtr = layout_;
		unsigned int posCounter = 0;
		unsigned int nrmCounter = 0;
		unsigned int texCounter = 0;
		unsigned int colCounter = 0;

		for (SemanticList bufferList : list)
		{
			unsigned int& stride = strides_[nBuffers_];
			stride = 0;
			for (Semantic semantic : bufferList)
			{
				switch (semantic) {
				case Semantic::Pos3:
					*layoutPtr = {
						"POSITION", posCounter, DXGI_FORMAT_R32G32B32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 12; ++posCounter;
					break;
				case Semantic::Pos4:
					*layoutPtr = {
						"POSITION", posCounter, DXGI_FORMAT_R32G32B32A32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 16; ++posCounter;
					break;
				case Semantic::Nrm3:
					*layoutPtr = { 
						"NORMAL", nrmCounter, DXGI_FORMAT_R32G32B32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 12; ++nrmCounter;
					break;
				case Semantic::Nrm4:
					*layoutPtr = { 
						"NORMAL", nrmCounter, DXGI_FORMAT_R32G32B32A32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 16; ++nrmCounter;
					break;
				case Semantic::Col1:
					*layoutPtr = {
						"COLOR", colCounter, DXGI_FORMAT_R32G32B32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 4; ++colCounter;
					break;
				case Semantic::Col3:
					*layoutPtr = { 
						"COLOR", colCounter, DXGI_FORMAT_R32G32B32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 12; ++colCounter;
					break;
				case Semantic::Col4:
					*layoutPtr = { 
						"COLOR", colCounter, DXGI_FORMAT_R32G32B32A32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 16; ++nrmCounter;
					break;
				case Semantic::Uv2:
					*layoutPtr = { 
						"TEXCOORD", texCounter, DXGI_FORMAT_R32G32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 8; ++texCounter;
					break;
				case Semantic::Uv3:
					*layoutPtr = { 
						"TEXCOORD", texCounter, DXGI_FORMAT_R32G32B32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 12; ++texCounter;
					break;
				case Semantic::Uv4:
					*layoutPtr = { 
						"TEXCOORD", texCounter, DXGI_FORMAT_R32G32B32A32_FLOAT,
						nBuffers_, stride, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					stride += 16; ++texCounter;
					break;
				}
				++layoutPtr;
				++nElems_;
				assert(nElems_ <= nMaxBuffers);
			};
			++nBuffers_;
			assert(nBuffers_ <= nMaxBuffers);
		}
	}

	VertexDesc::VertexDesc(const VertexDesc & orig)
	{
		nElems_ = orig.nElems_;
		nBuffers_ = orig.nBuffers_;
		D3D11_INPUT_ELEMENT_DESC layout_[nMaxElems];
		for (unsigned int i = 0; i < nElems_; ++i) layout_[i] = orig.layout_[i];
		for (unsigned int i = 0; i < nBuffers_; ++i) strides_[i] = orig.strides_[i];
	}

}

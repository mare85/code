#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <Util/Utils.h>
#include <initializer_list>
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



	enum class Semantic {
		Pos3, Pos4,
		Nrm3, Nrm4,
		Col1, Col3, Col4,
		Uv2, Uv3, Uv4
	};

	typedef std::initializer_list<Semantic> SemanticList;
	typedef std::initializer_list<SemanticList> BufferSemanticList;
	struct VertexDescDeprecated
	{
		D3D11_INPUT_ELEMENT_DESC* layout_;
		unsigned int nElems_;
		unsigned int stride_;

		static VertexDescDeprecated get(VertexDescPreset preset);
		void copyFrom(VertexDescDeprecated* other);
	};

	struct VertexDesc
	{
		enum {
			nMaxElems = 10,
			nMaxBuffers = 6
		};
		D3D11_INPUT_ELEMENT_DESC layout_[nMaxElems];
		unsigned int strides_[nMaxBuffers];
		unsigned int nElems_ = 0;
		unsigned int nBuffers_ = 0;
		VertexDesc(BufferSemanticList list);
		VertexDesc() {};
		VertexDesc(const VertexDesc& orig);

	};

	struct VertexP4UV4
	{
		vmath::Vector4 pos;
		vmath::Vector4 uv;
	};
}

#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <Util/Utils.h>
#include <Graphics/VertexTypes.h>
#include <Graphics/Texture.h>
#include <Graphics/Shader.h>
#include <initializer_list>

//int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

namespace Graphics {
	
	struct ConstantBufferData
	{
		vmath::Matrix4 world_ = vmath::Matrix4::identity();
		vmath::Matrix4 view_ = vmath::Matrix4::identity();
		vmath::Matrix4 projection_ = vmath::Matrix4::identity();
		vmath::Vector4 userData0_ = vmath::Vector4( .0f );
	};
	

	class Postprocess;
	class RenderTarget;
	typedef ID3D11Buffer Buffer;
	typedef ID3D11ShaderResourceView Srv;
	typedef ID3D11UnorderedAccessView Uav;

	enum class BufferType {
		Vertex = 0,
		Constant = 1,
		DynamicVertex = 2,
		ComputeVertex = 3,
		ComputeByteAddress = 4
	};

	enum RTFormat {
		eRGBA32 = 0,
		eRGBA8 = 1,
		eR32 = 2,
		eR8 = 3,
	};

	class GdiContext;
	typedef ID3D11BlendState BlendState;
	enum BlendType {
		eNone,
		eAlphaOneMinusAlpha,
		eAlphaOne,
	};

	enum PrimitiveTopology
	{
		eTriangles = 0,
		eTriangleStrip = 1,
		eLines = 2,
		eLineStrip = 3,
	};

	class GdiContext
	{
	public:


		enum StartFlags
		{
			UseDepthBuffer = 1,
			
		};

		enum ShaderType
		{
			eVertexShader = 0,
			ePixelShader = 1,
		};

		GdiContext();
		~GdiContext();
		bool startUp(HINSTANCE appInstance, HWND windowHandle, unsigned int flags, unsigned int w, unsigned int h);
		void shutDown();
		void update(float deltaTime);
		void clearRender();
		void swapRender();

		void updateBuffer(Buffer* dst, void* src);
		void setConstantBuffer(Buffer* dts);


		BlendState* createBlendState(BlendType type);
		void releaseBlendState(BlendState*&);
		void setBlendState(BlendState* state, const vmath::Vector4& blendColor );

		Postprocess* createPostprocess(const char* shaderName, unsigned int width, unsigned int height, unsigned int cbsize);
		RenderTarget* createRenderTarget( unsigned int width, unsigned int height, unsigned int cbsize, RTFormat rtFormat = eRGBA32);
		void releasePostprocess(Postprocess*& out);
		void releaseRenderTarget(RenderTarget*& out);
		
		void setTextureRenderTarget(RenderTarget* rt);
		void resetRenderTarget();
		void setViewport(unsigned int width, unsigned int height);

		Shader* createShader(const char* filename, VertexDescDeprecated* desc, const char* storeName = nullptr );
		Shader* createShader(const char* filename, VertexDesc* desc, const char* storeName = nullptr);
		void releaseShader(Shader*& sh, bool byStore = false);
		Buffer* createBuffer(void* data, unsigned int size, BufferType bufferType);
		void releaseBuffer(Buffer*&);
		Srv* createByteAddressSrv(Buffer* buff);
		void releaseSrv(Srv*&);
		Uav* createByteAddressUav(Buffer* buff);
		void releaseUav(Uav*&);

		Texture* createTexture(const char* filename, TextureDesc *desc,const char* storeName = nullptr);
		void releaseTexture(Texture*& tex, bool byStore = false);
		void* mapWrite(Buffer* buff);
		void unmap(Buffer* buff);
		float getAspect() const { return aspect_; }
		unsigned int getWidth() { return width_; }
		unsigned int getHeight() { return height_; }

		void bindShader(Shader* shader, PrimitiveTopology topology = eTriangles );
		
		void bindAsTexture(RenderTarget* rt, unsigned int slot);
		void unbindTextureSlot( unsigned int slot);
		void drawTriangles(Buffer* buffer, unsigned int vertCount);
		void drawTriangles(std::initializer_list<Buffer*>buffers, unsigned int vertCount);
		void setClearColor(float r, float g, float b, float a = 1.0f)
		{
			clearColor[0] = r;
			clearColor[1] = g;
			clearColor[2] = b;
			clearColor[3] = a;
		}

				
	private:
		HINSTANCE appInstance_;
		HWND windowHandle_;
		D3D_DRIVER_TYPE driverType_;
		D3D_FEATURE_LEVEL featureLevel_;
		ID3D11Device* d3dDevice_;
		ID3D11DeviceContext* d3dContext_;
		IDXGISwapChain* swapChain_;
		ID3D11RenderTargetView* currentRenderTarget_ = nullptr;
		ID3D11RenderTargetView* backBufferTarget_ = nullptr;
		ID3D11Texture2D* depthTexture_;
		float aspect_ = 1.0f;
		unsigned int width_ = 0;
		unsigned int height_ = 0;
		ID3D11DepthStencilView* depthBufferTarget_ = nullptr;

		unsigned int strides_[VertexDesc::nMaxBuffers];
		float clearColor[4];

		bool _CompileShader(const char* filename, const char* entryPoint, const char* shaderModel, ID3D10Blob** out);
		
	
	};
};

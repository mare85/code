#include "GdiContext.h"
#include <D3DCompiler.h>
#include <Graphics/Store.h>
#include <Graphics/Postprocess.h>
#include <assert.h>

Graphics::GdiContext::GdiContext()
{
	clearColor[0] = .0f;
	clearColor[1] = .0f;
	clearColor[2] = .0f;
	clearColor[3] = 1.0f;
}

Graphics::GdiContext::~GdiContext()
{
}

bool Graphics::GdiContext::startUp(
	HINSTANCE appInstance, HWND windowHandle, 
	unsigned int flags, unsigned int w, unsigned int h,
	bool windowed)
{
	appInstance_ = appInstance;
	windowHandle_ = windowHandle;
	RECT dimensions;
	GetClientRect(windowHandle_, &dimensions);
	//width_ = 852;
	//height_ = 480;
	width_ = w;
	height_ = h;
	aspect_ = (float)width_ / (float)height_;
	windowed_ = windowed;
	swapFlags_ = windowed ? 0 : DXGI_PRESENT_RESTART;
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_SOFTWARE
	};
	unsigned int nDriverTypes = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	unsigned int nFeatureLevels = ARRAYSIZE(featureLevels);
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width_;
	swapChainDesc.BufferDesc.Height= height_;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowHandle_;
	swapChainDesc.Windowed = windowed;

// #ifndef _DEBUG
// 	swapChainDesc.Windowed = false;
// #endif
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	unsigned int creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT res;
	for (unsigned int driver = 0; driver < nDriverTypes; ++driver)
	{
		res = D3D11CreateDeviceAndSwapChain(0, driverTypes[driver], 0,
			creationFlags, featureLevels, nFeatureLevels, 
			D3D11_SDK_VERSION, &swapChainDesc, &swapChain_, 
			&d3dDevice_, &featureLevel_, &d3dContext_);

		if (SUCCEEDED(res))
		{
			driverType_ = driverTypes[driver];
			driver = nDriverTypes;
		}
	}
	if (FAILED(res))
	{
		DXTRACE_MSG("failed to create the direct3d device!");
		return false;
	}
	ID3D11Texture2D* backBufferTexture;
	res = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);
	if (FAILED(res))
	{
		DXTRACE_MSG("failed to get the swap chain back buffer!");
		return false;
	}
	res = d3dDevice_->CreateRenderTargetView(backBufferTexture, 0, &backBufferTarget_);
	if (backBufferTexture)
	{
		backBufferTexture->Release();
	}
	if (FAILED(res))
	{
		DXTRACE_MSG("Failed to create render target view!");
		return false;
	}
	currentRenderTarget_ = backBufferTarget_;
	
	if (flags & UseDepthBuffer )
	{
		D3D11_TEXTURE2D_DESC depthTexDesc;
		ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
		depthTexDesc.Width = width_;
		depthTexDesc.Height = height_;
		depthTexDesc.MipLevels = 1;
		depthTexDesc.ArraySize = 1;
		depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTexDesc.SampleDesc.Count = 1;
		depthTexDesc.SampleDesc.Quality = 0;
		depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTexDesc.CPUAccessFlags = 0;
		depthTexDesc.MiscFlags = 0;
		res = d3dDevice_->CreateTexture2D(&depthTexDesc, NULL, &depthTexture_);
		if (FAILED(res))
		{
			DXTRACE_MSG("Failed to create the depth texture!");

			return false;
		}
		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = depthTexDesc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		res = d3dDevice_->CreateDepthStencilView(depthTexture_, &descDSV,
			&depthBufferTarget_);
		if (FAILED(res))
		{
			DXTRACE_MSG("Failed to create the depth stencil target view!");
			return false;
		}
		d3dContext_->OMSetRenderTargets(1, &backBufferTarget_, depthBufferTarget_);

	}
	else
	{
		d3dContext_->OMSetRenderTargets(1, &backBufferTarget_, nullptr);
	}
	
	setViewport(width_, height_);
	

	return false;


}

void Graphics::GdiContext::shutDown()
{
	if (backBufferTarget_) backBufferTarget_->Release();
	if (depthTexture_) depthTexture_->Release();
	if (depthBufferTarget_) depthBufferTarget_->Release();
	if (swapChain_) swapChain_->Release();
	if (d3dContext_) d3dContext_->Release();
	if (d3dDevice_) d3dDevice_->Release();
	d3dDevice_ = 0;
	d3dContext_ = 0;
	swapChain_ = 0;
	backBufferTarget_ = 0;
}

void Graphics::GdiContext::update(float deltaTime)
{
	(void)deltaTime;

}

void Graphics::GdiContext::clearRender()
{
	d3dContext_->ClearRenderTargetView(currentRenderTarget_, clearColor);
	if (depthBufferTarget_)
	{
		d3dContext_->ClearDepthStencilView(depthBufferTarget_, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
}

void Graphics::GdiContext::swapRender()
{
	swapChain_->Present(1, swapFlags_);
}

Graphics::Postprocess * Graphics::GdiContext::createPostprocess(const char * shaderName, unsigned int width, unsigned int height, unsigned int cbsize)
{
	Postprocess* out = new Postprocess();
	Graphics::VertexP4UV4 points[] = {
		{ vmath::Vector4(1.0f, 1.0f, .0f, 1.0f), vmath::Vector4(1.0f, 0.0f, .0f, .0f) },
		{ vmath::Vector4(1.0f,-1.0f, .0f, 1.0f), vmath::Vector4(1.0f, 1.0f, .0f, .0f) },
		{ vmath::Vector4(-1.0f,-1.0f, .0f, 1.0f), vmath::Vector4(0.0f, 1.0f, .0f, .0f) },
		{ vmath::Vector4(1.0f, 1.0f, .0f, 1.0f), vmath::Vector4(1.0f, 0.0f, .0f, .0f) },
		{ vmath::Vector4(-1.0f,-1.0f, .0f, 1.0f), vmath::Vector4(0.0f, 1.0f, .0f, .0f) },
		{ vmath::Vector4(-1.0f,1.0f, .0f, 1.0f), vmath::Vector4(0.0f, 0.0f, .0f, .0f) }
	};
	unsigned int bufferSize = sizeof(Graphics::VertexP4UV4) * 6;
	out->vbuff_ = createBuffer(points, bufferSize, Graphics::BufferType::Vertex);
	typedef Graphics::Semantic S;
	Graphics::VertexDesc vDesc = {{S::Pos4, S::Uv4}};
	out->sh_ = createShader(shaderName, &vDesc);
	out->cBuff_ = createBuffer(nullptr, cbsize, Graphics::BufferType::Constant);

	
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = d3dDevice_->CreateTexture2D(&textureDesc, NULL, &out->texture_);
	if (FAILED(result))
	{
		return nullptr;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = d3dDevice_->CreateRenderTargetView(out->texture_, &renderTargetViewDesc, &out->view_);
	if (FAILED(result))
	{
		return nullptr;
	}


	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = d3dDevice_->CreateShaderResourceView(out->texture_, &shaderResourceViewDesc, &out->resourceView_);
	if (FAILED(result))
	{
		return nullptr;
	}

	TextureDesc desc;
	desc.filter_ = TextureDesc::eLinear;
	desc.mode_ = TextureDesc::eClamp;

	D3D11_TEXTURE_ADDRESS_MODE mode = desc.getGdiMode();
	D3D11_FILTER filter = desc.getGdiFilter();
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.AddressU = mode;
	samplerDesc.AddressV = mode;
	samplerDesc.AddressW = mode;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.Filter = filter;
	samplerDesc.MipLODBias = .0f;
	samplerDesc.MaxAnisotropy= 1;

	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT d3dResult;
	d3dResult = d3dDevice_->CreateSamplerState(&samplerDesc,
		&out->sampler_);
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to create sampler state!");
		return nullptr;
	}

	return out;
}

Graphics::RenderTarget * Graphics::GdiContext::createRenderTarget(unsigned int width, unsigned int height, unsigned int cbsize, RTFormat rtFormat )
{
	RenderTarget* out = new RenderTarget();
	Graphics::VertexP4UV4 points[] = {
		{ vmath::Vector4(1.0f, 1.0f, .0f, 1.0f), vmath::Vector4(1.0f, 0.0f, .0f, .0f) },
		{ vmath::Vector4(1.0f,-1.0f, .0f, 1.0f), vmath::Vector4(1.0f, 1.0f, .0f, .0f) },
		{ vmath::Vector4(-1.0f,-1.0f, .0f, 1.0f), vmath::Vector4(0.0f, 1.0f, .0f, .0f) },
		{ vmath::Vector4(1.0f, 1.0f, .0f, 1.0f), vmath::Vector4(1.0f, 0.0f, .0f, .0f) },
		{ vmath::Vector4(-1.0f,-1.0f, .0f, 1.0f), vmath::Vector4(0.0f, 1.0f, .0f, .0f) },
		{ vmath::Vector4(-1.0f,1.0f, .0f, 1.0f), vmath::Vector4(0.0f, 0.0f, .0f, .0f) }
	};
	unsigned int bufferSize = sizeof(Graphics::VertexP4UV4) * 6;
	out->vbuff_ = createBuffer(points, bufferSize, Graphics::BufferType::Vertex);
	out->cBuff_ = createBuffer(nullptr, cbsize, Graphics::BufferType::Constant);


	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;

	DXGI_FORMAT rtFormats[] = {
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R8G8B8A8_UINT,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R8_UINT
	};

	textureDesc.Format = rtFormats[ rtFormat ];
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = d3dDevice_->CreateTexture2D(&textureDesc, NULL, &out->texture_);
	if (FAILED(result))
	{
		return nullptr;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = d3dDevice_->CreateRenderTargetView(out->texture_, &renderTargetViewDesc, &out->view_);
	if (FAILED(result))
	{
		return nullptr;
	}


	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = d3dDevice_->CreateShaderResourceView(out->texture_, &shaderResourceViewDesc, &out->resourceView_);
	if (FAILED(result))
	{
		return nullptr;
	}

	TextureDesc desc;
	desc.filter_ = TextureDesc::eLinear;
	desc.mode_ = TextureDesc::eClamp;

	D3D11_TEXTURE_ADDRESS_MODE mode = desc.getGdiMode();
	D3D11_FILTER filter = desc.getGdiFilter();
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.AddressU = mode;
	samplerDesc.AddressV = mode;
	samplerDesc.AddressW = mode;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.Filter = filter;
	samplerDesc.MipLODBias = .0f;
	samplerDesc.MaxAnisotropy = 1;

	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT d3dResult;
	d3dResult = d3dDevice_->CreateSamplerState(&samplerDesc,
		&out->sampler_);
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to create sampler state!");
		return nullptr;
	}

	return out;
}

void Graphics::GdiContext::releasePostprocess(Postprocess *& out)
{
	releaseShader(out->sh_);
	releaseBuffer(out->vbuff_);
	releaseBuffer(out->cBuff_);
	

	if( out->view_ )
	{
		out->view_->Release();
		out->view_ = nullptr;
	}
	if (out->texture_)
	{
		out->texture_->Release();
		out->texture_ = nullptr;
	}
	if (out->resourceView_)
	{
		out->resourceView_->Release();
		out->resourceView_ = nullptr;
	}
	if (out->sampler_)
	{
		out->sampler_->Release();
		out->sampler_ = nullptr;
	}
}

void Graphics::GdiContext::releaseRenderTarget(RenderTarget *& out)
{
	releaseBuffer(out->vbuff_);
	releaseBuffer(out->cBuff_);

	if (out->view_)
	{
		out->view_->Release();
		out->view_ = nullptr;
	}
	if (out->texture_)
	{
		out->texture_->Release();
		out->texture_ = nullptr;
	}
	if (out->resourceView_)
	{
		out->resourceView_->Release();
		out->resourceView_ = nullptr;
	}
	if (out->sampler_)
	{
		out->sampler_->Release();
		out->sampler_ = nullptr;
	}
}

void Graphics::GdiContext::setTextureRenderTarget(RenderTarget * rt)
{
	currentRenderTarget_ = rt->view_;
	if (depthBufferTarget_)
	{
		d3dContext_->OMSetRenderTargets(1, &currentRenderTarget_, depthBufferTarget_);
	}
	else
	{
		d3dContext_->OMSetRenderTargets(1, &currentRenderTarget_, nullptr);
	}
}

void Graphics::GdiContext::resetRenderTarget()
{
	currentRenderTarget_ = backBufferTarget_;
	if (depthBufferTarget_)
	{
		d3dContext_->OMSetRenderTargets(1, &currentRenderTarget_, depthBufferTarget_);
	}
	else
	{
		d3dContext_->OMSetRenderTargets(1, &currentRenderTarget_, nullptr);
	}
}

void Graphics::GdiContext::setViewport(unsigned int width, unsigned int height)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	d3dContext_->RSSetViewports(1, &viewport);
}

Graphics::Shader * Graphics::GdiContext::createShader(const char * filename, VertexDesc * desc, const char * storeName)
{
	unsigned int storeHash = 0;
	if (storeName)
	{
		storeHash = Util::createHash(storeName);
	}
	Store* store = Store::getInstance();
	if (storeName)
	{
		Shader* sh = store->getShader(storeHash);
		if (sh)
			return  sh;
	}
	Shader* out = new Shader();
	ID3D10Blob* compiledShaderVS = nullptr;
	ID3D10Blob* compiledShaderPS = nullptr;
	{
		bool compileRes = _CompileShader(filename, "vs_main", "vs_4_0", &compiledShaderVS);
		if (compileRes == false)
		{
			MessageBox(0, "error loading vertex shader!", "Compile Error", MB_OK);
		}
		HRESULT d3dResult;
		d3dResult = d3dDevice_->CreateVertexShader(
			compiledShaderVS->GetBufferPointer(),
			compiledShaderVS->GetBufferSize(), 0, &(out->vs_)
		);
		if (FAILED(d3dResult))
		{
			if (compiledShaderVS)
				compiledShaderVS->Release();
			delete out;
			return nullptr;
		}
	}
	{
		bool compileRes = _CompileShader(filename, "ps_main", "ps_4_0", &compiledShaderPS);
		if (compileRes == false)
		{
			MessageBox(0, "error loading vertex shader!", "Compile Error", MB_OK);
		}
		HRESULT d3dResult;
		d3dResult = d3dDevice_->CreatePixelShader(
			compiledShaderPS->GetBufferPointer(),
			compiledShaderPS->GetBufferSize(), 0, &(out->ps_)
		);
		if (FAILED(d3dResult))
		{
			if (compiledShaderVS)
				compiledShaderVS->Release();
			if (compiledShaderPS)
				compiledShaderPS->Release();
			delete out;
			return nullptr;
		}
	}
	HRESULT d3dResult = d3dDevice_->CreateInputLayout(desc->layout_,
		desc->nElems_, compiledShaderVS->GetBufferPointer(),
		compiledShaderVS->GetBufferSize(), &(out->il_));
	compiledShaderPS->Release();
	compiledShaderVS->Release();
	if (FAILED(d3dResult))
	{
		delete out;
		return nullptr;
	}
	out->desc_ = *desc;
	if (storeName)
	{
		out->stored_ = true;
		store->addShader(storeHash, out);
	}

	return out;
}

void Graphics::GdiContext::releaseShader(Shader *&sh, bool byStore)
{
	if (sh && ( !sh->stored_ || byStore) )
	{
		if( sh->vs_ )sh->vs_->Release();
		if (sh->ps_)sh->ps_->Release();
		if (sh->il_)sh->il_->Release();
		delete sh;
		sh = nullptr;
	}
}

Graphics::ComputeShader * Graphics::GdiContext::createComputeShader(const char * filename, const char* entryname, const char * storeName)
{
	unsigned int storeHash = 0;
	if (storeName)
	{
		storeHash = Util::createHash(storeName);
	}
	Store* store = Store::getInstance();
	if (storeName)
	{
		ComputeShader* sh = store->getComputeShader(storeHash);
		if (sh)
			return sh;
	}
	ComputeShader* out = new ComputeShader();
	ID3D10Blob* compiledShader = nullptr;
	{
		LPCSTR profile = "cs_4_0";//( d3dDevice_->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? "cs_5_0" : "cs_4_0";
		bool compileRes = _CompileShader(filename, entryname, profile, &compiledShader);
		if (compileRes == false)
		{
			MessageBox(0, "error loading compute shader!", "Compile Error", MB_OK);
		}
		HRESULT d3dResult;
		d3dResult = d3dDevice_->CreateComputeShader(
			compiledShader->GetBufferPointer(),
			compiledShader->GetBufferSize(), 0, &(out->cs_)
		);
		if (FAILED(d3dResult))
		{
			if (compiledShader)
				compiledShader->Release();
			delete out;
			return nullptr;
		}
	}
	if (storeName)
	{
		out->stored_ = true;
		store->addComputeShader(storeHash, out);
	}

	return out;
}

void Graphics::GdiContext::releaseComputeShader(Graphics::ComputeShader *& sh, bool byStore)
{
	if (sh && ( !sh->stored_ || byStore) )
	{
		if (sh->cs_) sh->cs_->Release();
		delete sh;
		sh = nullptr;
	}
}

void Graphics::GdiContext::dispatchComputeShader(
	ComputeShader * sh, 
	std::initializer_list<Srv*> srvs, 
	std::initializer_list<Uav*> uavs,
	unsigned int countX,unsigned int countY,unsigned int countZ)
{
	d3dContext_->CSSetShader(sh->cs_, 0, 0);
	unsigned int srvInd = 0;
	unsigned int uavInd = 0;
	for(Srv* srv : srvs)
	{
		d3dContext_->CSSetShaderResources(srvInd, 1, &srv);
		++srvInd;
	}

	for(Uav* uav : uavs)
	{
		d3dContext_->CSSetUnorderedAccessViews(uavInd, 1, &uav, nullptr);
		++uavInd;
	}	
	d3dContext_->Dispatch(countX,countY,countZ);

	//unbinding
	Srv* nullSrv = nullptr;
	Uav* nullUav = nullptr;
	for(srvInd = 0; srvInd != srvs.size(); ++srvInd) d3dContext_->CSSetShaderResources(srvInd, 1, &nullSrv);
	for(uavInd = 0; uavInd != uavs.size(); ++uavInd) d3dContext_->CSSetUnorderedAccessViews(uavInd, 1, &nullUav, nullptr);
	d3dContext_->CSSetShader( nullptr, nullptr, 0 );
}

Graphics::Buffer * Graphics::GdiContext::createBuffer(void * data, unsigned int size, BufferType bufferType)
{
	D3D11_BUFFER_DESC vbDesc;
	HRESULT d3dResult;
	Buffer* buff = nullptr;
	if (bufferType == BufferType::Vertex)
	{
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		vbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;// D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		vbDesc.ByteWidth = size;
	}
	else if (bufferType == BufferType::Constant)
	{
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		vbDesc.ByteWidth = size;
	}
	else if (bufferType == BufferType::CpuToVertex)
	{
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.ByteWidth = size;
	}
	else if (bufferType == BufferType::ComputeToVertex)
	{
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		vbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		vbDesc.ByteWidth = size;
	}
	else if (bufferType == BufferType::CpuToCompute)
	{
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		vbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;// D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		vbDesc.ByteWidth = size;
	}
	else if (bufferType == BufferType::Compute)
	{
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		vbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;// D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		vbDesc.ByteWidth = size;
	}
	else if (bufferType == BufferType::Index)
	{
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		vbDesc.ByteWidth = size;
	}
	else if ( bufferType == BufferType::Debug)
	{
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_STAGING;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		vbDesc.ByteWidth = size;
	}
	else
	{
		return nullptr;
	}

	D3D11_SUBRESOURCE_DATA resourceData;
	if (data)
	{
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = data;
	}
	d3dResult = d3dDevice_->CreateBuffer(&vbDesc, (data ? (&resourceData) : 0 ), &buff);
	
	if (FAILED(d3dResult))
	{
		return nullptr;
	}
	return buff;
}

Graphics::Srv * Graphics::GdiContext::createByteAddressSrv(Buffer * buff)
{	
	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
	buff->GetDesc(&buffDesc);
	unsigned int numElements = buffDesc.ByteWidth / 4;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	desc.BufferEx.NumElements = numElements;
	Srv* out = nullptr;
	d3dDevice_->CreateShaderResourceView(buff, &desc, &out);
	return out;
}

void Graphics::GdiContext::releaseSrv(Srv *& srv)
{
	if (srv)
		srv->Release();
	srv = nullptr;
}

Graphics::Uav * Graphics::GdiContext::createByteAddressUav(Buffer * buff)
{
	Uav *out;
	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
	buff->GetDesc(&buffDesc);
	unsigned int numElements = buffDesc.ByteWidth / 4;
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	desc.Buffer.NumElements = numElements;
	d3dDevice_->CreateUnorderedAccessView(buff, &desc, &out);
	return out;
}

void Graphics::GdiContext::releaseUav(Uav *&uav)
{
	if (uav)
		uav->Release();
	uav = nullptr;
}

void Graphics::GdiContext::updateBuffer(Buffer * dts, void * src)
{
	d3dContext_->UpdateSubresource(dts, 0, 0, src, 0, 0);
}

void Graphics::GdiContext::setConstantBuffer(Buffer * buff)
{
	d3dContext_->VSSetConstantBuffers(0, 1, &buff);
}

Graphics::BlendState * Graphics::GdiContext::createBlendState(BlendType type)
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	if (type == eAlphaOneMinusAlpha)
	{
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	}
	else if (type == eAlphaOne)
	{
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	}
	else if (type == eNone)
	{
		blendDesc.RenderTarget[0].BlendEnable = false;
	}
	BlendState* out;
	d3dDevice_->CreateBlendState(&blendDesc, &out);
	return out;
}

void Graphics::GdiContext::releaseBlendState(Graphics::BlendState *& blendState)
{
	if (blendState)
	{
		blendState->Release();
	}
	blendState = nullptr;
}

void Graphics::GdiContext::setBlendState(BlendState * state, const vmath::Vector4 & blendColor)
{
	const float* vec = (const float*)&blendColor;
	d3dContext_->OMSetBlendState(state, vec, 0xFFFFFFFF);
}

void Graphics::GdiContext::releaseBuffer(Buffer *& buff)
{
	if( buff )
		buff->Release();
	buff = nullptr;
}

void Graphics::GdiContext::copyBuffer(Buffer * dst, Buffer * src)
{
	d3dContext_->CopyResource(dst, src);
}

Graphics::Texture * Graphics::GdiContext::createTexture(const char * filename, TextureDesc *desc, const char* storeName )
{
	unsigned int storeHash = 0;
	if (storeName)
	{
		storeHash = Util::createHash(storeName);
	}
	Store* store = Store::getInstance();
	if (storeName)
	{
		Texture* t = store->getTexture(storeHash);
		if (t)
			return  t;
	}


	Texture* tex = new Texture();
	HRESULT d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice_,
		filename, 0, 0, &tex->tex_, 0);

	
	
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to load the texture image!");
		return false;
	}
	D3D11_SAMPLER_DESC colorMapDesc;
	ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));

	D3D11_TEXTURE_ADDRESS_MODE mode = desc->getGdiMode();
	D3D11_FILTER filter = desc->getGdiFilter();

	colorMapDesc.AddressU = mode;
	colorMapDesc.AddressV = mode;
	colorMapDesc.AddressW = mode;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	colorMapDesc.Filter = filter;
	
	colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	d3dResult = d3dDevice_->CreateSamplerState(&colorMapDesc,
		&tex->sampler_);
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to create color map sampler state!");
		delete tex;
		return nullptr;
	}
	if (storeName)
	{
		tex->stored_ = true;
		store->addTexture(storeHash, tex);
	}
	return tex;
}


void Graphics::GdiContext::releaseTexture(Texture *& tex, bool byStore)
{
	if (!tex || ( !byStore && tex->stored_ ))
		return;
	if (tex->tex_) tex->tex_->Release();
	if (tex->tex_) tex->sampler_->Release();
	delete tex;
	tex = nullptr;
}

void * Graphics::GdiContext::mapWrite(Buffer * buff)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT d3dResult = d3dContext_->Map(buff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to map resource!");
		return nullptr;
	}
	return mappedSubresource.pData;
}
void * Graphics::GdiContext::mapRead(Buffer * buff)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT d3dResult = d3dContext_->Map(buff, 0, D3D11_MAP_READ, 0, &mappedSubresource);
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to map resource!");
		return nullptr;
	}
	return mappedSubresource.pData;
}


void Graphics::GdiContext::unmap(Buffer * buff)
{
	d3dContext_->Unmap(buff, 0);
}

void Graphics::GdiContext::bindShader(Shader * shader, PrimitiveTopology topology)
{
	d3dContext_->IASetInputLayout(shader->il_);
	D3D_PRIMITIVE_TOPOLOGY tops[] = {
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
	};

	d3dContext_->IASetPrimitiveTopology(tops[ topology ]);
	d3dContext_->VSSetShader(shader->vs_, 0, 0);
	d3dContext_->PSSetShader(shader->ps_, 0, 0);
	for (unsigned int i = 0; i < nShaderTextureSlots; ++i)
	{
		Texture* tex = shader->textures_[i];
		if (tex)
		{
			d3dContext_->PSSetShaderResources(i, 1, &tex->tex_);
			d3dContext_->PSSetSamplers(i, 1, &tex->sampler_);
		}
	}
	for( unsigned int i = 0; i < shader->desc_.nBuffers_; ++i)
		strides_[i] = shader->desc_.strides_[i];
}

void Graphics::GdiContext::bindAsTexture(RenderTarget * rt, unsigned int slot)
{
	d3dContext_->PSSetShaderResources(slot, 1, &rt->resourceView_);
	d3dContext_->PSSetSamplers(slot, 1, &rt->sampler_);
}

void Graphics::GdiContext::unbindTextureSlot(unsigned int slot)
{
	ID3D11ShaderResourceView *tab[] = { nullptr };
	d3dContext_->PSSetShaderResources(slot, 1, tab);
}




void Graphics::GdiContext::drawTriangles(Buffer * buffer, unsigned int vertCount)
{
	unsigned int offset = 0;
	d3dContext_->IASetVertexBuffers(0, 1, &buffer, strides_, &offset);
	d3dContext_->Draw(vertCount, 0);
	// unbinding - for compute shaders to have a free way
	Buffer* nullbuff = nullptr;
	d3dContext_->IASetVertexBuffers(0,1,&nullbuff, &offset,&offset);
}

void Graphics::GdiContext::drawTriangles(Buffer * indexBuffer, Buffer * vbuffer, unsigned int indexCount)
{
	unsigned int offset = 0;
	d3dContext_->IASetVertexBuffers(0, 1, &vbuffer, strides_, &offset);
	d3dContext_->IASetIndexBuffer(indexBuffer,DXGI_FORMAT_R32_UINT, 0);
	d3dContext_->DrawIndexed( indexCount, 0, 0);
	// unbinding - for compute shaders to have a free way
	Buffer* nullbuff = nullptr;
	d3dContext_->IASetVertexBuffers(0,1,&nullbuff, &offset,&offset);
}

void Graphics::GdiContext::drawTriangles(std::initializer_list<Buffer*> buffers, unsigned int vertCount)
{
	unsigned int i = 0;
	unsigned int offset = 0;
	for (Buffer* buffer : buffers)
	{
		d3dContext_->IASetVertexBuffers(i, 1, &buffer, strides_ + i, &offset);
		++i;
	}
	d3dContext_->Draw(vertCount, 0);
	// unbinding - for compute shaders to have a free way
	Buffer* nullbuff = nullptr;
	for ( i = 0; i < buffers.size(); ++i) d3dContext_->IASetVertexBuffers(i,1,&nullbuff, &offset,&offset);
}

void Graphics::GdiContext::drawTriangles(Buffer * indexBuffer, std::initializer_list<Buffer*> buffers, unsigned int indexCount)
{
	unsigned int offset = 0;
	unsigned int i = 0;
	for (Buffer* buffer : buffers)
	{
		d3dContext_->IASetVertexBuffers(i, 1, &buffer, strides_ + i, &offset);
		++i;
	}
	d3dContext_->IASetIndexBuffer(indexBuffer,DXGI_FORMAT_R32_UINT, 0);
	d3dContext_->DrawIndexed( indexCount, 0, 0);
	// unbinding - for compute shaders to have a free way
	Buffer* nullbuff = nullptr;
	for ( i = 0; i < buffers.size(); ++i) d3dContext_->IASetVertexBuffers(i,1,&nullbuff, &offset,&offset);
}

bool Graphics::GdiContext::_CompileShader(const char * filename, const char * entryPoint, const char * shaderModel, ID3D10Blob ** out)
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* errorBuffer = 0;
	HRESULT result;
	result = D3DX11CompileFromFile(filename, 0, 0, entryPoint, shaderModel, shaderFlags, 0, 0, out, &errorBuffer, 0);
	if (FAILED(result))
	{
		if (errorBuffer != 0)
		{
			OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
			errorBuffer->Release();
		}
		return false;
	}
	if (errorBuffer != 0)
		errorBuffer->Release();
	return true;
}

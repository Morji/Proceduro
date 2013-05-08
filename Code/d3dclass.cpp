////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"


D3DClass::D3DClass()
{
	m_device = nullptr;
	m_swapChain = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	mRasterizerSolid = nullptr;
	mTransparentBS = nullptr;
	mDepthDisabledStencilState = nullptr;
	mDrawReflectionBS = nullptr;
	mDrawMirrorDSS = nullptr;
	mDrawReflectionDSS = nullptr;
	mCurrentRasterizer = nullptr;
	mDynamicRasterizer = nullptr;
	mRasterizerWireframe = nullptr;
	mRasterizerCullCWRS = nullptr;
	mAlphaEnableBlendingState = nullptr;
	mAlphaDisableBlendingState = nullptr;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

bool D3DClass::BuildBlendStates(){
	//create the transparent BS
	D3D10_BLEND_DESC blendDesc = {0};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.BlendEnable[0] = true;
	blendDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	blendDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	HRESULT result = m_device->CreateBlendState(&blendDesc, &mTransparentBS);

	if (FAILED(result)){
		return false;
	}
	//create the reflection BS
	blendDesc.SrcBlend = D3D10_BLEND_BLEND_FACTOR;
	blendDesc.DestBlend = D3D10_BLEND_INV_BLEND_FACTOR;

	result = m_device->CreateBlendState(&blendDesc, &mDrawReflectionBS);

	if (FAILED(result)){
		return false;
	}

	// Clear the blend state description.
	ZeroMemory(&blendDesc, sizeof(D3D10_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendDesc.BlendEnable[0] = TRUE;
	blendDesc.SrcBlend = D3D10_BLEND_ONE;
	blendDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask[0] = 0x0f;

	// Create the blend state using the description.
	result = m_device->CreateBlendState(&blendDesc, &mAlphaEnableBlendingState);
	if(FAILED(result)){
		return false;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendDesc.BlendEnable[0] = FALSE;

	// Create the second blend state using the description.
	result = m_device->CreateBlendState(&blendDesc, &mAlphaDisableBlendingState);
	if(FAILED(result)){
		return false;
	}

	return true;
}

bool D3DClass::BuildDepthStencilStates(){
	D3D10_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D10_COMPARISON_LESS;
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;

	// Always pass the stencil test, and replace the
	// current stencil value with the stencil reference value 1.
	dsDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// We are not rendering backfacing polygons, so these
	// settings do not matter.
	dsDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	// Create the depth/stencil state used to draw the mirror
	// to the stencil buffer.
	HRESULT result = m_device->CreateDepthStencilState(&dsDesc, &mDrawMirrorDSS);

	if (FAILED(result)){
		return false;
	}

	// Only pass the stencil test if the value in the stencil
	// buffer equals the stencil reference value.
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D10_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;
	// Create the depth/stencil state used to draw the reflected
	// crate to the back buffer.
	result = m_device->CreateDepthStencilState(&dsDesc, &mDrawReflectionDSS);

	if (FAILED(result)){
		return false;
	}

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&dsDesc, sizeof(dsDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D10_COMPARISON_LESS;
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = m_device->CreateDepthStencilState(&dsDesc, &mDepthDisabledStencilState);
	if(FAILED(result)){
		return false;
	}

	return true;
}

bool D3DClass::BuildRasterizers(){
	//build solid rasterizer
	D3D10_RASTERIZER_DESC rasterizerState;
	rasterizerState.CullMode = D3D10_CULL_BACK;
	rasterizerState.FillMode = D3D10_FILL_SOLID;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = true;

	HRESULT result = m_device->CreateRasterizerState( &rasterizerState, &mRasterizerSolid);

	if (FAILED(result)){
		return false;
	}

	//build wireframe rasterizer
	rasterizerState.FillMode = D3D10_FILL_WIREFRAME;
	result = m_device->CreateRasterizerState( &rasterizerState, &mRasterizerWireframe);

	if (FAILED(result)){
		return false;
	}

	D3D10_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
	rsDesc.CullMode = D3D10_CULL_BACK;
	rsDesc.FillMode = D3D10_FILL_SOLID;
	rsDesc.FrontCounterClockwise = true;
	result = m_device->CreateRasterizerState( &rsDesc, &mRasterizerCullCWRS);

	if (FAILED(result)){
		return false;
	}

	mCurrentRasterizer = mRasterizerSolid;

	return true;
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
						  float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D10Texture2D* backBufferPtr;
	D3D10_TEXTURE2D_DESC depthBufferDesc;
	D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	float fieldOfView, screenAspect;

	// populate screen info
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
	mScreenDepth = screenDepth;
	mScreenNear = screenNear;

	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}

	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if(m_vsync_enabled)
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if(fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Create the swap chain and the Direct3D device.
	result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION, 
										   &swapChainDesc, &m_swapChain, &m_device);
	if(FAILED(result))
	{
		return false;
	}

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if(FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		return false;
	}

	if (!BuildDepthStencilStates()){
		return false;
	}

	// Set the depth stencil state on the D3D device.
	m_device->OMSetDepthStencilState(m_depthStencilState, 1);

	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		return false;
	}

	//Create blend states
	if (!BuildBlendStates()){
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Setup the raster description which will determine how and what polygons will be drawn.
	if(!BuildRasterizers())
	{
		return false;
	}

	// Now set the rasterizer state.
	m_device->RSSetState(mCurrentRasterizer);

	// Setup the viewport for rendering.
    mViewport.Width = screenWidth;
    mViewport.Height = screenHeight;
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;
    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;

	// Create the viewport.
    m_device->RSSetViewports(1, &mViewport);

	// Setup the projection matrix.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

    // Initialize the world matrix to the identity matrix.
    D3DXMatrixIdentity(&m_worldMatrix);

	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;
}


void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if(mTransparentBS)
	{
		mTransparentBS->Release();
		mTransparentBS = 0;
	}

	if(mDrawMirrorDSS)
	{
		mDrawMirrorDSS->Release();
		mDrawMirrorDSS = 0;
	}

	ReleaseCOM(mAlphaDisableBlendingState);
	ReleaseCOM(mAlphaEnableBlendingState);
	ReleaseCOM(mDepthDisabledStencilState);

	if(mDrawReflectionDSS)
	{
		mDrawReflectionDSS->Release();
		mDrawReflectionDSS = 0;
	}

	if(mDynamicRasterizer)
	{
		mDynamicRasterizer->Release();
		mDynamicRasterizer = 0;
	}

	if(mRasterizerWireframe)
	{
		mRasterizerWireframe->Release();
		mRasterizerWireframe = 0;
	}

	if(mRasterizerCullCWRS)
	{
		mRasterizerCullCWRS->Release();
		mRasterizerCullCWRS = 0;
	}

	if(mRasterizerSolid)
	{
		mRasterizerSolid->Release();
		mRasterizerSolid = 0;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	return;
}


void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_device->ClearRenderTargetView(m_renderTargetView, color);
    
	// Clear the depth buffer.
	m_device->ClearDepthStencilView(m_depthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if(m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}

void D3DClass::TurnZBufferOn(){
	m_device->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}


void D3DClass::TurnZBufferOff(){
	m_device->OMSetDepthStencilState(mDepthDisabledStencilState, 1);
	return;
}


void D3DClass::TurnOnAlphaBlending(){
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	m_device->OMSetBlendState(mAlphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}

void D3DClass::TurnWireframeOn(){
	mCurrentRasterizer = mRasterizerWireframe;
	m_device->RSSetState(mCurrentRasterizer);
}

void D3DClass::TurnWireframeOff(){	
	mCurrentRasterizer = mRasterizerSolid;
	m_device->RSSetState(mCurrentRasterizer);
}

void D3DClass::TurnCullingOff(){
	D3D10_RASTERIZER_DESC rsDesc;
	mCurrentRasterizer->GetDesc(&rsDesc);
	rsDesc.CullMode = D3D10_CULL_NONE;
	m_device->CreateRasterizerState( &rsDesc, &mCurrentRasterizer);//should check for return value here
	m_device->RSSetState(mCurrentRasterizer);
}

void D3DClass::TurnCullingOn(){
	D3D10_RASTERIZER_DESC rsDesc;
	mCurrentRasterizer->GetDesc(&rsDesc);
	rsDesc.CullMode = D3D10_CULL_BACK;
	m_device->CreateRasterizerState( &rsDesc, &mCurrentRasterizer);//should check for return value here
	m_device->RSSetState(mCurrentRasterizer);
}

void D3DClass::TurnOffAlphaBlending(){
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn off the alpha blending.
	m_device->OMSetBlendState(mAlphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

void D3DClass::SetBackBufferRenderTarget(){
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}

D3D10_VIEWPORT	*D3DClass::GetViewport(){
	return &mViewport;
}

void D3DClass::ResetViewport(){
	// Set the viewport.
	m_device->RSSetViewports(1, &mViewport);

	return;
}

bool D3DClass::Screenshot(){
	HRESULT hr;
	ID3D10Resource *backbufferRes;
	m_renderTargetView->GetResource(&backbufferRes);
	
	D3D10_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = 0;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Width = 800;  // must be same as backbuffer
	texDesc.Height = 600; // must be same as backbuffer
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D10_USAGE_DEFAULT;
	
	ID3D10Texture2D *texture;
	hr =  m_device->CreateTexture2D(&texDesc, 0, &texture);
	if (FAILED(hr)){
		return false;
	}
	m_device->CopyResource(texture, backbufferRes);
	
	hr = D3DX10SaveTextureToFile(texture, D3DX10_IFF_PNG, L"ProceduroScreenshot.png");
	if (FAILED(hr)){
		return false;
	}
	texture->Release();
	backbufferRes->Release();
	return true;
}

ID3D10Device* D3DClass::GetDevice()
{
	return m_device;
}

D3DXMATRIX* D3DClass::GetProjectionMatrix()
{
	return &m_projectionMatrix;
}

D3DXMATRIX* D3DClass::GetWorldMatrix()
{
	return &m_worldMatrix;
}

D3DXMATRIX* D3DClass::GetOrthoMatrix()
{
	return &m_orthoMatrix;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix){
	projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix){
	worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix){
	orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetScreenDimensions(int *width, int *height){
	if (width != nullptr){
		*width = mScreenWidth;
	}
	if (height != nullptr){
		*height = mScreenHeight;
	}
}

void D3DClass::GetScreenDepthInfo(float *nearVal, float *farVal){
	if (nearVal != nullptr){
		*nearVal = mScreenNear;
	}
	if (farVal != nullptr){
		*farVal = mScreenDepth;
	}
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}
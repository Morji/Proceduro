#include "GlowEffect.h"


GlowEffect::GlowEffect()
{
	pd3dObject = nullptr;
	mOrthoTexShader = nullptr;
	mRetainBrightnessShader = nullptr;
	mBlurShader = nullptr;
	mRenderTexture = nullptr;
	mDownSampleTexture = nullptr;
	mHorizontalBlurTexture = nullptr;
	mVerticalBlurTexture = nullptr;
	mBrightnessRetainedTexture = nullptr;
	mUpSampleTexture = nullptr;
	mGlowShader = nullptr;
}

GlowEffect::~GlowEffect(void)
{
	pd3dObject = nullptr;
	while (!mTextureCollection.empty()){
		RenderTexture *texture = mTextureCollection.back();
		if (texture){
			texture->Shutdown();
			delete texture;
			texture = nullptr;
		}
		mTextureCollection.pop_back();
	}
	mTextureCollection.clear();

	while (!mShaderCollection.empty()){
		IShader *shader = mShaderCollection.back();
		if (shader){
			delete shader;
			shader = nullptr;
		}
		mShaderCollection.pop_back();
	}
	mShaderCollection.clear();
}

bool GlowEffect::Initialize(ID3DObject *d3dObject, HWND hwnd){
	pd3dObject = d3dObject;

	int screenWidth,screenHeight;
	float screenNear,screenFar;
	d3dObject->GetScreenDimensions(&screenWidth,&screenHeight);
	d3dObject->GetScreenDepthInfo(&screenNear,&screenFar);

	int downSampleWidth = screenWidth/2;
	int downSampleHeight = screenHeight/2;

	////CREATE RENDER TEXTURE OBJECTS
	// Create the render to texture object.
	mRenderTexture = new RenderTexture;
	if(!mRenderTexture){
		return false;
	}
	// Initialize the render to texture object.
	bool result = mRenderTexture->Initialize(d3dObject->GetDevice(), screenWidth, screenHeight, screenFar, screenNear);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}
	mTextureCollection.push_back(mRenderTexture);

	// Create the down sample render to texture object.
	mDownSampleTexture = new RenderTexture;
	if(!mDownSampleTexture){
		return false;
	}
	// Initialize the down sample render to texture object.
	result = mDownSampleTexture->Initialize(d3dObject->GetDevice(), downSampleWidth, downSampleHeight, screenFar, screenNear);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}
	mTextureCollection.push_back(mDownSampleTexture);

	// create the texture which will retain the brightness
	// Create the down sample render to texture object.
	mBrightnessRetainedTexture = new RenderTexture;
	if(!mBrightnessRetainedTexture){
		return false;
	}
	// Initialize the down sample render to texture object.
	result = mBrightnessRetainedTexture->Initialize(d3dObject->GetDevice(), downSampleWidth, downSampleHeight, screenFar, screenNear);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}
	mTextureCollection.push_back(mBrightnessRetainedTexture);

	// Create the horizontal blur render to texture object.
	mHorizontalBlurTexture = new RenderTexture;
	if(!mHorizontalBlurTexture){
		return false;
	}
	// Initialize the horizontal blur render to texture object.
	result = mHorizontalBlurTexture->Initialize(d3dObject->GetDevice(), downSampleWidth, downSampleHeight, screenFar, screenNear);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}
	mTextureCollection.push_back(mHorizontalBlurTexture);

	// Create the vertical blur render to texture object.
	mVerticalBlurTexture = new RenderTexture;
	if(!mVerticalBlurTexture){
		return false;
	}
	// Initialize the vertical blur render to texture object.
	result = mVerticalBlurTexture->Initialize(d3dObject->GetDevice(), downSampleWidth, downSampleHeight, screenFar, screenNear);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}
	mTextureCollection.push_back(mVerticalBlurTexture);

	// Create the up sample render to texture object.
	mUpSampleTexture = new RenderTexture;
	if(!mUpSampleTexture){
		return false;
	}
	// Initialize the up sample render to texture object.
	result = mUpSampleTexture->Initialize(d3dObject->GetDevice(), screenWidth, screenHeight, screenFar, screenNear);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}
	mTextureCollection.push_back(mUpSampleTexture);
	////RENDER TEXTURE OBJECT CREATION FINISHED

	////CREATE SHADERS
	// ortho shader
	mOrthoTexShader = new OrthoTextureShader();
	if (!mOrthoTexShader){
		return false;
	}
	result = mOrthoTexShader->Initialize(d3dObject->GetDevice(),hwnd);
	if (!result){
		MessageBox(hwnd, L"Error while initializing ortho shader ", L"Error", MB_OK);
		return false;
	}
	mShaderCollection.push_back(mOrthoTexShader);
	// blur shader
	mBlurShader = new BlurShader();
	if (!mBlurShader){
		return false;
	}
	result = mBlurShader->Initialize(d3dObject->GetDevice(),hwnd);
	if (!result){
		MessageBox(hwnd, L"Error while initializing blur shader ", L"Error", MB_OK);
		return false;
	}
	mShaderCollection.push_back(mBlurShader);
	//retain brightness shader
	mRetainBrightnessShader = new RetainBrightnessShader();
	if (!mRetainBrightnessShader){
		return false;
	}
	result = mRetainBrightnessShader->Initialize(d3dObject->GetDevice(),hwnd);
	if (!result){
		MessageBox(hwnd, L"Error while initializing retain brightness shader ", L"Error", MB_OK);
		return false;
	}
	mShaderCollection.push_back(mRetainBrightnessShader);
	//glow shader
	mGlowShader = new GlowShader();
	if (!mGlowShader){
		return false;
	}
	result = mGlowShader->Initialize(d3dObject->GetDevice(),hwnd);
	if (!result){
		MessageBox(hwnd, L"Error while initializing glow shader ", L"Error", MB_OK);
		return false;
	}
	mShaderCollection.push_back(mGlowShader);
	////SHADER CREATION FINISHED
	return true;
}

void GlowEffect::PrepareForSceneRender(){
	// Set the render target to be the render to texture.
	mRenderTexture->SetRenderTarget(pd3dObject->GetDevice());

	// Clear the render to texture.
	mRenderTexture->ClearRenderTarget(pd3dObject->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);
}

void GlowEffect::SceneRenderFinished(){
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	pd3dObject->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	pd3dObject->ResetViewport();
}

void GlowEffect::RenderToWindow(OrthoWindow *orthoWindow){
	pOrthoWindow = orthoWindow;

	// Down sample the render texture to a smaller sized texture.
	DownSampleTexture();

	RetainTextureBrightness();

	// Render the blur effect to the texture
	RenderBlurToTexture();

	// Up sample the final blurred render texture to screen size again.
	UpSampleTexture();

	// Render the blurred up sampled render texture to the screen.
	Render2DTextureScene();
}

void GlowEffect::DownSampleTexture(){
	// Set the render target to be the render to texture.
	mDownSampleTexture->SetRenderTarget(pd3dObject->GetDevice());

	// Clear the render to texture.
	mDownSampleTexture->ClearRenderTarget(pd3dObject->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	pd3dObject->TurnZBufferOff();

	// Put the ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	pOrthoWindow->Render(pd3dObject->GetDevice());

	// Render the ortho window using the texture shader and the render to texture of the scene as the texture resource.
	mOrthoTexShader->Render(pd3dObject->GetDevice(), pOrthoWindow->GetIndexCount(),mRenderTexture->GetShaderResourceView());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	pd3dObject->TurnZBufferOn();
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	pd3dObject->SetBackBufferRenderTarget();
}

void GlowEffect::RetainTextureBrightness(){
	// Set the render target to be the render to texture.
	mBrightnessRetainedTexture->SetRenderTarget(pd3dObject->GetDevice());

	// Clear the render to texture.
	mBrightnessRetainedTexture->ClearRenderTarget(pd3dObject->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	pd3dObject->TurnZBufferOff();

	// Put the ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	pOrthoWindow->Render(pd3dObject->GetDevice());

	// Render the ortho window using the texture shader and the render to texture of the scene as the texture resource.
	mRetainBrightnessShader->Render(pd3dObject->GetDevice(), pOrthoWindow->GetIndexCount(),mDownSampleTexture->GetShaderResourceView(),LUMINANCE_THRESHOLD);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	pd3dObject->TurnZBufferOn();
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	pd3dObject->SetBackBufferRenderTarget();
}

void GlowEffect::RenderBlurToTexture(){
	float textureSizeX,textureSizeY;

	// Store the screen height in a float that will be used in the vertical blur shader.
	textureSizeY = (float)mVerticalBlurTexture->GetTextureHeight();
	textureSizeX = (float)mHorizontalBlurTexture->GetTextureWidth();
	
	// Set the render target to be the render to texture.
	mHorizontalBlurTexture->SetRenderTarget(pd3dObject->GetDevice());

	// Clear the render to texture.
	mHorizontalBlurTexture->ClearRenderTarget(pd3dObject->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	pd3dObject->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	pOrthoWindow->Render(pd3dObject->GetDevice());
	
	// Render the horizontal blur to the texture
	mBlurShader->RenderHorizontalBlur(pd3dObject->GetDevice(), pOrthoWindow->GetIndexCount(),mBrightnessRetainedTexture->GetShaderResourceView(), textureSizeX);

	// Set the render target to be the render to texture.
	mVerticalBlurTexture->SetRenderTarget(pd3dObject->GetDevice());

	// Clear the render to texture.
	mVerticalBlurTexture->ClearRenderTarget(pd3dObject->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Render the vertical blur to the texture
	mBlurShader->RenderVerticalBlur(pd3dObject->GetDevice(), pOrthoWindow->GetIndexCount(),mHorizontalBlurTexture->GetShaderResourceView(), textureSizeY);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	pd3dObject->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	pd3dObject->SetBackBufferRenderTarget();	
}

void GlowEffect::UpSampleTexture(){
	// Set the render target to be the render to texture.
	mUpSampleTexture->SetRenderTarget(pd3dObject->GetDevice());

	// Clear the render to texture.
	mUpSampleTexture->ClearRenderTarget(pd3dObject->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	pd3dObject->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	pOrthoWindow->Render(pd3dObject->GetDevice());

	// Render the full screen ortho window using the texture shader and the small sized final blurred render to texture resource.
	mOrthoTexShader->Render(pd3dObject->GetDevice(), pOrthoWindow->GetIndexCount(), mVerticalBlurTexture->GetShaderResourceView());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	pd3dObject->TurnZBufferOn();
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	pd3dObject->SetBackBufferRenderTarget();
}

void GlowEffect::Render2DTextureScene(){
	// Turn off the Z buffer to begin all 2D rendering.
	pd3dObject->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	pOrthoWindow->Render(pd3dObject->GetDevice());

	// Render the full screen ortho window using the glow shader and the full screen sized blurred render to texture resource.
	mGlowShader->Render(pd3dObject->GetDevice(), pOrthoWindow->GetIndexCount(), mRenderTexture->GetShaderResourceView(),mUpSampleTexture->GetShaderResourceView(),GLOW_STRENGTH);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	pd3dObject->TurnZBufferOn();
}
////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsClass.h"

long stop, remaining;//for timer class

GraphicsClass::GraphicsClass(){
	mD3D = 0;
	mTerrain = 0;
	mCamera = 0;
	mMultiTexShader = 0;
	mInput = 0;
	mCpu = 0;
	mText = 0;
	mRegularTexShader = 0;
	mTree = 0;	
	mOrthoTexShader = 0;

	mHorizontalBlurShader = 0;
	mVerticalBlurShader = 0;
	m_RenderTexture = 0;
	m_DownSampleTexure = 0;
	m_HorizontalBlurTexture = 0;
	m_VerticalBlurTexture = 0;
	m_UpSampleTexure = 0;
	mFullScreenWindow = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, HINSTANCE hinstance)
{
	bool result;
	D3DXMATRIX basemViewMatrix;
	char videoCard[128];
	int videoMemory;

	int downSampleWidth, downSampleHeight;
	// Set the size to sample down to.
	downSampleWidth = screenWidth / 2;
	downSampleHeight = screenHeight / 2;

	// prepare timer
	mTimer.reset();

	// Create the Direct3D object.
	mD3D = new D3DClass;
	if(!mD3D){
		return false;
	}

	// Initialize the Direct3D object.
	result = mD3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	mInput = new InputClass;
	if(!mInput){
		return false;
	}

	// Initialize the input object.
	result = mInput->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the cpu object.
	mCpu = new CpuClass;
	if (!mCpu){
		return false;
	}
	mCpu->Initialize();

	// Create the text object.
	mText = new TextClass;
	if(!mText)
	{
		return false;
	}
	
	InitLights();
	InitTerrain(hwnd);
	InitShaders(hwnd);
	InitCameras();
	InitMisc(hwnd);

	// Create the render to texture object.
	m_RenderTexture = new RenderTexture;
	if(!m_RenderTexture){
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(mD3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the down sample render to texture object.
	m_DownSampleTexure = new RenderTexture;
	if(!m_DownSampleTexure){
		return false;
	}

	// Initialize the down sample render to texture object.
	result = m_DownSampleTexure->Initialize(mD3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the horizontal blur render to texture object.
	m_HorizontalBlurTexture = new RenderTexture;
	if(!m_HorizontalBlurTexture){
		return false;
	}

	// Initialize the horizontal blur render to texture object.
	result = m_HorizontalBlurTexture->Initialize(mD3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertical blur render to texture object.
	m_VerticalBlurTexture = new RenderTexture;
	if(!m_VerticalBlurTexture){
		return false;
	}

	// Initialize the vertical blur render to texture object.
	result = m_VerticalBlurTexture->Initialize(mD3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the up sample render to texture object.
	m_UpSampleTexure = new RenderTexture;
	if(!m_UpSampleTexure){
		return false;
	}

	// Initialize the up sample render to texture object.
	result = m_UpSampleTexure->Initialize(mD3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the full screen ortho window object.
	mFullScreenWindow = new OrthoWindow;
	if(!mFullScreenWindow){
		return false;
	}

	// Initialize the full screen ortho window object.
	result = mFullScreenWindow->Initialize(mD3D->GetDevice(), -1.0f, 1.0f, -1.0f, 1.0f);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the text object.
	mCamera->Render();
	mCamera->GetViewMatrix(basemViewMatrix);
	result = mText->Initialize(mD3D->GetDevice(), hwnd, screenWidth, screenHeight, basemViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Retrieve the video card information.
	mD3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	result = mText->SetVideoCardInfo(videoCard, videoMemory);
	if(!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}
	mCamera->SetPosition(-86.0f, 85.0f, -107.0f);
	return true;
}

void GraphicsClass::InitCameras(){
	// Create the god camera object.
	mCamera = new GameCamera();
	// Set the initial position of the camera.
	mCamera->SetPosition(0.0f, 0.0f, -10.0f);
}

void GraphicsClass::InitShaders(HWND hwnd){
	mMultiTexShader = new MultiTextureShader();
	shaderCollection.push_back(mMultiTexShader);

	mRegularTexShader = new TexShader();
	shaderCollection.push_back(mRegularTexShader);

	mOrthoTexShader = new OrthoTextureShader();
	shaderCollection.push_back(mOrthoTexShader);

	// Create the horizontal blur shader object.
	mHorizontalBlurShader = new HorizontalBlurShader;
	shaderCollection.push_back(mHorizontalBlurShader);

	// Create the vertical blur shader object.
	mVerticalBlurShader = new VerticalBlurShader;
	shaderCollection.push_back(mVerticalBlurShader);

	// Init all shaders
	for (int i = 0; i < shaderCollection.size(); i++){
		IShader *shader = shaderCollection[i];
		bool result = shader->Initialize(mD3D->GetDevice(),hwnd);
		if (!result){
			MessageBox(hwnd, L"Error while initializing shader ", L"Error", MB_OK);
		}
	}
}

void GraphicsClass::InitLights(){
	// Parallel mLight.
	mLight.dir = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mLight.ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	mLight.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

void GraphicsClass::InitTerrain(HWND hwnd){
	bool result;

	srand(time(NULL));

	//create a mTerrain generator 
	TerrainGenerator *generator = new TerrainGenerator;
	result = generator->GenerateGridDiamondSquare(8,20.0f,0.65f, true);
	if(!result){
		MessageBox(hwnd, L"Could properly generate mTerrain.", L"Error", MB_OK);
	}

	generator->NoiseOverTerrain(10.0f,50.0f);
	
	//create mTerrain
	mTerrain = new Terrain;
	result = mTerrain->InitializeWithMultiTexture(mD3D->GetDevice(),L"assets/textures/defaultspec.dds", NULL,L"assets/textures/stone2.dds",
																						 L"assets/textures/ground0.dds",
																						 L"assets/textures/grass0.dds");
	if(!result){
		MessageBox(hwnd, L"Could not initialize the mTerrain object.", L"Error", MB_OK);
	}
	//supply mTerrain with mTerrain generator
	result = mTerrain->CreateTerrain(generator);
	if(!result){
		MessageBox(hwnd, L"Could properly generate mTerrain.", L"Error", MB_OK);
	}
	delete generator;
	generator = nullptr;	
}

void GraphicsClass::InitMisc(HWND hwnd){
	Vector3f mTreePos = mTerrain->GetRandomPoint();
	mTree = new Tree(mD3D->GetDevice(),hwnd,mTreePos);
	bool result = mTree->GenerateTreeSpaceExploration(0.55f,0.0009f,45.0f);
	if(!result){
		MessageBox(hwnd, L"Could not initialize mTree object.", L"Error", MB_OK);
	}
}

void GraphicsClass::Shutdown()
{
	// Release the D3D object.
	if(mD3D){
		mD3D->Shutdown();
		delete mD3D;
		mD3D = nullptr;
	}

	if (mTree){
		delete mTree;
		mTree = nullptr;
	}

	if (mInput){
		mInput->Shutdown();
		delete mInput;
		mInput = nullptr;
	}

	if (mTerrain){
		mTerrain->Shutdown();
		delete mTerrain;
		mTerrain = nullptr;
	}

	if (mCamera){
		delete mCamera;
		mCamera = nullptr;
	}

	//clean-up shaders
	while (!shaderCollection.empty()){
		IShader *shader = shaderCollection.back();
		if (shader){
			delete shader;
			shader = nullptr;
		}
		shaderCollection.pop_back();
	}
	shaderCollection.clear();
	// Release the full screen ortho window object.
	if(mFullScreenWindow){
		mFullScreenWindow->Shutdown();
		delete mFullScreenWindow;
		mFullScreenWindow = 0;
	}

	
	// Release the up sample render to texture object.
	if(m_UpSampleTexure){
		m_UpSampleTexure->Shutdown();
		delete m_UpSampleTexure;
		m_UpSampleTexure = 0;
	}

	// Release the vertical blur render to texture object.
	if(m_VerticalBlurTexture){
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}

	// Release the horizontal blur render to texture object.
	if(m_HorizontalBlurTexture){
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}

	// Release the down sample render to texture object.
	if(m_DownSampleTexure){
		m_DownSampleTexure->Shutdown();
		delete m_DownSampleTexure;
		m_DownSampleTexure = 0;
	}
	
	// Release the render to texture object.
	if(m_RenderTexture){
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	mTimer.tick();

	mCpu->Frame();
	mText->SetFps(mTimer.getFps());
	mText->SetCpu(mCpu->GetCpuPercentage());

	//artificially limit to 60fps - no need for more
	stop = mTimer.getCurrTime() + 15;
	// Otherwise do the frame processing.

	// Read the user input.
	result = mInput->Frame();
	if(!result){
		return false;
	}

	//update scene
	Update(mTimer.getDeltaTime());

	// Render the graphics scene.
	result = Render();
	//result = BlurRender();
	
	if(!result)	{
		return false;
	}	

	remaining = stop - mTimer.getCurrTime();
	if (remaining > 0)
		Sleep(remaining);		

	return true;
}

bool GraphicsClass::BlurRender(){
	
	// First render the scene to a render texture.
	RenderSceneToTexture();
	
	// Next down sample the render texture to a smaller sized texture.
	DownSampleTexture();

	// Perform a horizontal blur on the down sampled render texture.
	RenderHorizontalBlurToTexture();

	// Now perform a vertical blur on the horizontal blur render texture.
	RenderVerticalBlurToTexture();

	// Up sample the final blurred render texture to screen size again.
	UpSampleTexture();

	// Render the blurred up sampled render texture to the screen.
	Render2DTextureScene();

	return true;
}

void GraphicsClass::RenderSceneToTexture()
{
	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);
	
	RenderScene();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();

	return;
}

void GraphicsClass::DownSampleTexture()
{
	// Set the render target to be the render to texture.
	m_DownSampleTexure->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_DownSampleTexure->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());

	// Render the ortho window using the texture shader and the render to texture of the scene as the texture resource.
	mOrthoTexShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(),	m_RenderTexture->GetShaderResourceView());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();

	return;
}

void GraphicsClass::RenderHorizontalBlurToTexture()
{
	float screenSizeX;

	// Store the screen width in a float that will be used in the horizontal blur shader.
	screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();
	
	// Set the render target to be the render to texture.
	m_HorizontalBlurTexture->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_HorizontalBlurTexture->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());

	// Render the small ortho window using the horizontal blur shader and the down sampled render to texture resource.
	mHorizontalBlurShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(),m_DownSampleTexure->GetShaderResourceView(), screenSizeX);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();
	
	return;
}

void GraphicsClass::RenderVerticalBlurToTexture()
{
	float screenSizeY;

	// Store the screen height in a float that will be used in the vertical blur shader.
	screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();
	
	// Set the render target to be the render to texture.
	m_VerticalBlurTexture->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_VerticalBlurTexture->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());
	
	// Render the small ortho window using the vertical blur shader and the horizontal blurred render to texture resource.
	mVerticalBlurShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(),m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();
	
	return;
}

void GraphicsClass::UpSampleTexture(){
	// Set the render target to be the render to texture.
	m_UpSampleTexure->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_UpSampleTexure->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());

	// Render the full screen ortho window using the texture shader and the small sized final blurred render to texture resource.
	mOrthoTexShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(), m_VerticalBlurTexture->GetShaderResourceView());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();

	return;
}

void GraphicsClass::Render2DTextureScene(){

	// Clear the buffers to begin the scene.
	mD3D->BeginScene(1.0f, 0.0f, 0.0f, 0.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	mOrthoTexShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(), m_UpSampleTexure->GetShaderResourceView());

	RenderGUI();
	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();
	
	// Present the rendered scene to the screen.
	mD3D->EndScene();

	return;
}

bool GraphicsClass::Render()
{
	mD3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);

	if (!RenderScene()){
		return false;
	}
	if (!RenderGUI()){
		return false;
	}

	mD3D->EndScene();
	return true;
}

bool GraphicsClass::RenderScene(){
	mCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mCamera->GetViewMatrix(mViewMatrix);
	mD3D->GetWorldMatrix(mWorldMatrix);
	mD3D->GetProjectionMatrix(mProjectionMatrix);
	mD3D->GetOrthoMatrix(mOrthoMatrix);
	
	D3DXVECTOR3 camPos;
	mCamera->GetPosition(camPos);

	mTerrain->Render(mWorldMatrix);
	mMultiTexShader->Render(mD3D->GetDevice(),mTerrain->GetIndexCount(),mTerrain->objMatrix,mViewMatrix,mProjectionMatrix,camPos,mLight,
																															 NULL,
																															 NULL,
																															 mTerrain->GetDiffuseMap(0),
																															 mTerrain->GetDiffuseMap(1),
																															 mTerrain->GetDiffuseMap(2),
																															 mTerrain->GetMaxHeight());
	mTree->Render(mWorldMatrix,mViewMatrix,mProjectionMatrix, camPos,mLight,0);
	return true;
}

bool GraphicsClass::RenderGUI(){
	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();
		
	// Turn on the alpha blending before rendering the text.
	mD3D->TurnOnAlphaBlending();

	// Render the text user interface elements.
	mText->Render(mD3D->GetDevice(), mWorldMatrix, mOrthoMatrix);

	// Turn off alpha blending after rendering the text.
	mD3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();
	return true;
}

void GraphicsClass::Update(float dt){
	UpdateCamera(dt);
	if (mInput->IsSpacePressed()){
		mD3D->TurnWireframeOn();
		/*bool result = mTerrain->NoiseOvermTerrain(25.0f,50.0f);
		if(result){
			mInput->SetKeyState(DIK_SPACE,false);
		}*/
	}
	if (mInput->IsPgUpPressed()){
		mD3D->TurnWireframeOff();
	}
	if (mInput->IsRightPressed()){
		mTerrain->AnimateTerrain(dt);
	}
	if (mInput->IsPgDownPressed()){
		mD3D->Screenshot();
	}
}

void GraphicsClass::UpdateCamera(float dt){
	if (mInput->IsWPressed()){
		mCamera->moveBackForward += mCamera->camMoveFactor*dt;
	}
	if (mInput->IsSPressed()){
		mCamera->moveBackForward -= mCamera->camMoveFactor*dt;
	}
	if (mInput->IsAPressed()){
		mCamera->moveLeftRight -= mCamera->camMoveFactor*dt;
	}
	if (mInput->IsDPressed()){
		mCamera->moveLeftRight += mCamera->camMoveFactor*dt;
	}
	if (mInput->IsMouseLeftPressed()){
		int mouseX,mouseY;
		mInput->GetMouseChange(mouseX,mouseY);
		mCamera->MoveYawPitch(mouseX/500.0f,mouseY/500.0f);
	}
	mCamera->ModifyCamMovement(mInput->GetMouseScroll()/20.0f);
	D3DXVECTOR3 camPos;
	mCamera->GetPosition(camPos); 
	mText->SetCameraPosition(camPos.x,camPos.y,camPos.z);
}